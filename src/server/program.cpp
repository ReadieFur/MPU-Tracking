#include "program.h"

bool Program::initialized = false;
bool Program::networkPreConfigured = false;
WebServer *Program::webServer;
SetupServer *Program::setupServer;
ConfiguredServer *Program::configuredServer;
SemaphoreHandle_t Program::semaphore = nullptr;
std::vector<VoidFunc> Program::onDeepSleep = {};
RTC_DATA_ATTR bool Program::wasAsleep = false;
//This only keeps track of the times the device was put to sleep via the PRG button.
//Used to check if the device should be reset.
RTC_DATA_ATTR long Program::PRGSleepTimes[3] = {0, 0, 0};

void Program::Init()
{
    if (Program::initialized != true)
    {
    	Serial.begin(115200);
        while (!Serial)
        {
            delay(1);
        }

        //From some testing it seems that I had to start the MPU before the display otherwise the readings would be incorrect. I'm assuming the readings I was seeing from the MPU were from the display. I noticed that in the MPU begin function I could pass in a second wire instance but I am yet to figure out how to get it to work properly, so for now I shall start the MPU I2C bus before the display.
        //I have also noticed that when using both of the I2C buses the display becomes very flickery, so I shall add the option to turn the display off after the user has connected to the device.
        Motion::MPUFound();
        Display::Enable();
        Splash::Show();
        Display::DrawString(0, 6, "Loading...");

        SetupSleepButton();
        if (wasAsleep)
        {
            LogWakeupReason();

            if (PRGSleepTimes[0] > GetRTCMS() - 30000)
            {
                Serial.println("Reset warning");
                Display::Clear();
                Display::DrawString(0, 0, "Warning.", u8x8_font_amstrad_cpc_extended_f);
                Display::DrawString(0, 1, "You have slept");
                Display::DrawString(0, 2, "3 times in 30s.");
                Display::DrawString(0, 3, "If you press");
                Display::DrawString(0, 4, "PRG once more");
                Display::DrawString(0, 5, "the ESP will");
                Display::DrawString(0, 6, "reset.");
                delay(5000);
            }
        }

        Network::ScanNetworks();

        NetworkConfig networkConfig = Network::LoadConfig();
        webServer = new WebServer();

        networkPreConfigured = Storage::GetBool("network", "configured", false);

        if ((networkConfig.networkMode == wifi_mode_t::WIFI_STA || networkConfig.networkMode == wifi_mode_t::WIFI_AP) && networkPreConfigured)
        {
            //Run on existing network.
            configuredServer = new ConfiguredServer(&webServer);
        }
        else
        {
            //Configure new network.
            setupServer = new SetupServer(&webServer);
        }

        initialized = true;
    }
}

//https://www.esp32.com/viewtopic.php?t=8947
uint64_t Program::GetRTCMS()
{
    return rtc_time_slowclk_to_us(rtc_time_get(), esp_clk_slowclk_cal_get()) / 1000;
}

#pragma region Sleep functions
void Program::LogWakeupReason()
{
    esp_sleep_wakeup_cause_t reason = esp_sleep_get_wakeup_cause();

    switch(reason)
    {
        case ESP_SLEEP_WAKEUP_EXT0:
            Serial.println("Wakeup caused by external signal using RTC_IO");
            break;
        case ESP_SLEEP_WAKEUP_EXT1:
            Serial.println("Wakeup caused by external signal using RTC_CNTL");
            break;
        case ESP_SLEEP_WAKEUP_TIMER:
            Serial.println("Wakeup caused by timer");
            break;
        case ESP_SLEEP_WAKEUP_TOUCHPAD:
            Serial.println("Wakeup caused by touchpad");
            break;
        case ESP_SLEEP_WAKEUP_ULP:
            Serial.println("Wakeup reason: Caused by ULP program");
            break;
        default:
            Serial.printf("Wakeup was not caused by deep sleep: %d\n", reason);
            break;
    }
}

void Program::EnterDeepSleep()
{
    Serial.println("Deep sleep started");
    Display::Log("Sleep starting");

    Display::Clear();
    Display::DrawString(0, 0, "Going to sleep.", u8x8_font_amstrad_cpc_extended_f);
    Display::DrawString(0, 1, "ESP will reboot");
    Display::DrawString(0, 2, "if PRG is");
    Display::DrawString(0, 3, "pressed.");

    for (int i = 0; i < onDeepSleep.size(); i++)
    {
        onDeepSleep[i]();
    }
    
    rtc_gpio_deinit(GPIO_NUM_0);
    rtc_gpio_pulldown_en(GPIO_NUM_0);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0); //1 = High, 0 = Low

    //This delay prevents the button from being pressed twice and waking up the device too soon (unless the user holds it for 1 second).
    //Try to read the button state and loop until it is released.
    delay(1000);
    // while (digitalRead(GPIO_NUM_0) == 1)

    //I am not adding the display to the onDeepSleep vector because it will turn the display off too soon.
    Display::Disable();
    Serial.println("Sleeping");

    wasAsleep = true;
    esp_deep_sleep_start();
}

void IRAM_ATTR Program::SemaphoreHandler(void *arg)
{
	xSemaphoreGiveFromISR(semaphore, NULL);
}

void Program::PRGStateDownAwake(void *arg)
{
    UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );

	for(;;)
	{
		if(xSemaphoreTake(semaphore, portMAX_DELAY) == pdTRUE)
		{
            if (PRGSleepTimes[0] > GetRTCMS() - 30000)
            {
                //The user has pressed PRG more than 3 times in 30s so reset the ESP.
                ResetESP();
                //The code below is never run.
            }

            PRGSleepTimes[0] = PRGSleepTimes[1];
            PRGSleepTimes[1] = PRGSleepTimes[2];
            PRGSleepTimes[2] = GetRTCMS();

            EnterDeepSleep();
		}
	}
}

//https://onipot.altervista.org/how-to-program-boot-or-prg-button-on-esp32/
void Program::SetupSleepButton()
{
	//Create a binary semaphore
    semaphore = xSemaphoreCreateBinary();

    //Setup the button GPIO pin
	gpio_pad_select_gpio(GPIO_NUM_0);
	
	//Quite obvious, a button is a input
	gpio_set_direction(GPIO_NUM_0, GPIO_MODE_INPUT);
	
	//Trigger the interrupt when going from HIGH -> LOW ( == pushing button)
	gpio_set_intr_type(GPIO_NUM_0, GPIO_INTR_NEGEDGE);
	
	//Associate button_task method as a callback
    //4kb stack, just to make sure it is enough.
	xTaskCreate(Program::PRGStateDownAwake, "PRGStateDown", 4096, NULL, 10, NULL);
	
	//Install default ISR service 
	gpio_install_isr_service(0);
	
	//Add our custom button handler to ISR
	gpio_isr_handler_add(GPIO_NUM_0, Program::SemaphoreHandler, NULL);
}

//This function is to be called from elsewhere.
void Program::DeepSleep()
{
    EnterDeepSleep();
}
#pragma endregion

void Program::ResetESP()
{
    Display::Clear();
    Display::DrawString(0, 0, "Resetting ESP.", u8x8_font_amstrad_cpc_extended_f);
    Display::DrawString(0, 1, "ESP reset");
    Display::DrawString(0, 2, "has been");
    Display::DrawString(0, 3, "triggered.");
    Serial.println("Reset ESP");
    Storage::EraseFlash();
    //Give the user a moment to read the message.
    delay(1000);
    Restart();
}

//This can cause a stack error from some methods, for the most part it should be ok as the ESP is restarting anyway, this is just to cleanly dispose of the program.
void Program::Restart()
{
    //This isn't nessesary, because the value gets reset if the device reboots instead of sleeping.
    // wasAsleep = false;
    
    try
    {
        if (configuredServer != nullptr)
        {
            configuredServer->~ConfiguredServer();
            delete configuredServer;
        }
        if (setupServer != nullptr)
        {
            setupServer->~SetupServer();
            delete setupServer;
        }
        webServer->~WebServer();
    }
    catch (const std::exception &e)
    {
        Serial.println(e.what());
    }

	ESP.restart();
}