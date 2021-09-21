#include "display.h"

bool Display::configured = false;
//This check is nessecary otherwise the ESP will panic and reboot.
bool Display::isEnabled = false;
U8X8_SSD1306_128X64_NONAME_SW_I2C *Display::u8x8;

bool Display::IsEnabled()
{
    return isEnabled;
}

void Display::Enable()
{
    if (isEnabled) { return; }

    if (!configured)
    {
        Display::u8x8 = new U8X8_SSD1306_128X64_NONAME_SW_I2C(15, 4, 16);
        configured = Display::u8x8->begin();
        isEnabled = true;
    }
    else
    {
        Display::u8x8->display();
        isEnabled = true;
    }
}

void Display::Disable()
{
    if (!isEnabled) { return; }

    // Display::u8x8->clear();
    Display::u8x8->noDisplay();
    isEnabled = false;
}

void Display::Clear()
{
    if (!isEnabled) { return; }

    Display::u8x8->clearLine(0);
    Display::u8x8->clearLine(1);
    Display::u8x8->clearLine(2);
    Display::u8x8->clearLine(3);
    Display::u8x8->clearLine(4);
    Display::u8x8->clearLine(5);
    Display::u8x8->clearLine(6);
    //Keeps logs on display.
    // Display::u8x8->clearLine(7);
}

U8X8_SSD1306_128X64_NONAME_SW_I2C Display::U8X8()
{
    return *Display::u8x8;
}

//Make a buffer for the display and wrap new text onto new lines if the space is available, if it isn't try to push the lines down.
//Max line length is 16 characters.
//Create a line priority so that more important messages don't get overwritten until that line is cleared.
void Display::DrawString(int x, int y, const char *string, const uint8_t *font)
{
    if (!isEnabled) { return; }

	Display::u8x8->clearLine(y);
	Display::u8x8->setFont(font);
    Display::u8x8->drawString(x, y, String(string).substring(0, 16).c_str());
}

void Display::DrawTwoPartString(int x, int y, const char *part1, const char *part2, const uint8_t *part1Font, const uint8_t *part2Font)
{
    if (!isEnabled) { return; }

    int part1Length = strlen(part1);
    part1Length = part1Length > 16 ? 16 : part1Length;
	Display::u8x8->clearLine(y);
    Display::u8x8->setFont(part1Font);
    Display::u8x8->drawString(x, y, String(part1).substring(0, 16).c_str());
    if (part1Length < 16)
    {
        Display::u8x8->setFont(part2Font);
        Display::u8x8->drawString(x + strlen(part1), y, String(part2).substring(0, 16 - part1Length).c_str());
    }
}

//Line 7 should be reserved for logs.
//Max characters shown is 12.
void Display::Log(const char *string)
{
    if (!isEnabled) { return; }

    Display::u8x8->setFont(u8x8_font_5x7_f);
	//Clearing the line is slow but this function shouldn't be called often.
    Display::u8x8->clearLine(7);
    //This is probably slow.
    Display::u8x8->drawString(0, 7, (String(millis() % 100) + ": " + String(string).substring(0, 12)).c_str());
    //This should be done with the usual method (Serial.println()).
    // Serial.println(string);
}

// void Display::DrawXBM(int16_t xMove, int16_t yMove, int16_t width, int16_t height, const uint8_t *xbm)
// {
//     Display::u8x8->setFont(u8x8_font_5x7_f);

//     int16_t widthInXbm = (width + 7) / 8;
//     uint8_t data = 0;

//     for(int16_t y = 0; y < height; y++)
//     {
//         for(int16_t x = 0; x < width; x++ )
//         {
//             if (x & 7)
//             {
//                 data >>= 1; //Move a bit
//             }
//             else
//             {
//                 //Read new data every 8 bit
//                 data = pgm_read_byte(xbm + (x / 8) + y * widthInXbm);
//             }
//             // if there is a bit draw it
//             if (data & 0x01)
//             {
//                 // Display::u8x8->drawPixel(xMove + x, yMove + y);
//                 // Display::u8x8->drawTile(xMove + x, yMove + y, 1, 1);
//                 Display::u8x8->drawString(xMove + x, yMove + y, "■");
//             }
//             else
//             {
//                 // Display::u8x8->drawPixel(xMove + x, yMove + y);
//                 Display::u8x8->drawString(xMove + x, yMove + y, " ");
//             }
//         }
//     }
// }