import { Main, IReturnData } from '../assets/js/main.js';

class Setup
{
    private networkDetails:
    {
        button: HTMLButtonElement,
        tbody: HTMLTableSectionElement,
        mode: HTMLLabelElement,
        ssid: HTMLLabelElement,
        ip: HTMLLabelElement,
        usesPassword: HTMLLabelElement,
        // rssi: HTMLLabelElement,
        channel: HTMLLabelElement,
        maxConnections: HTMLLabelElement,
        hidden: HTMLLabelElement,
        sleep: HTMLButtonElement,
        reboot: HTMLButtonElement,
        reset: HTMLButtonElement
    };
    private joinNetwork:
    {
        button: HTMLButtonElement,
        tbody: HTMLTableSectionElement,
        hiddenCheckbox:
        {
            label: HTMLLabelElement,
            checkbox: HTMLInputElement
        },
        foundNetworks: HTMLSelectElement,
        hiddenSSID: HTMLInputElement,
        password: HTMLInputElement,
        submit: HTMLButtonElement
    };
    private hostNetwork:
    {
        button: HTMLButtonElement,
        tbody: HTMLTableSectionElement,
        ssid: HTMLInputElement,
        password: HTMLInputElement,
        channel: HTMLSelectElement,
        maxConnections: HTMLInputElement,
        hiddenCheckbox:
        {
            label: HTMLLabelElement,
            checkbox: HTMLInputElement
        },
        submit: HTMLButtonElement
    }
    private message: HTMLParagraphElement;
    private link: HTMLAnchorElement;

    constructor()
    {
        new Main();

        this.networkDetails =
        {
            button: Main.ThrowIfNullOrUndefined(document.querySelector('#networkDetailsButton')),
            tbody: Main.ThrowIfNullOrUndefined(document.querySelector('#networkDetailsPage')),
            mode: Main.ThrowIfNullOrUndefined(document.querySelector('#networkDetailsPage .mode')),
            ssid: Main.ThrowIfNullOrUndefined(document.querySelector('#networkDetailsPage .ssid')),
            ip: Main.ThrowIfNullOrUndefined(document.querySelector('#networkDetailsPage .ip')),
            usesPassword: Main.ThrowIfNullOrUndefined(document.querySelector('#networkDetailsPage .usesPassword')),
            // rssi: Main.ThrowIfNullOrUndefined(document.querySelector('#networkDetailsPage .rssi')),
            channel: Main.ThrowIfNullOrUndefined(document.querySelector('#networkDetailsPage .channel')),
            maxConnections: Main.ThrowIfNullOrUndefined(document.querySelector('#networkDetailsPage .maxConnections')),
            hidden: Main.ThrowIfNullOrUndefined(document.querySelector('#networkDetailsPage .hidden')),
            sleep: Main.ThrowIfNullOrUndefined(document.querySelector('#networkDetailsPage .sleep')),
            reboot: Main.ThrowIfNullOrUndefined(document.querySelector('#networkDetailsPage .reboot')),
            reset: Main.ThrowIfNullOrUndefined(document.querySelector('#networkDetailsPage .reset'))
        };
        this.joinNetwork =
        {
            button: Main.ThrowIfNullOrUndefined(document.querySelector('#joinNetworkButton')),
            tbody: Main.ThrowIfNullOrUndefined(document.querySelector('#joinNetworkPage')),
            hiddenCheckbox:
            {
                label: Main.ThrowIfNullOrUndefined(document.querySelector('#joinNetworkPage .hiddenNetwork')),
                checkbox: Main.ThrowIfNullOrUndefined(document.querySelector('#joinNetworkPage .hiddenNetwork > input[type=checkbox]'))
            },
            foundNetworks: Main.ThrowIfNullOrUndefined(document.querySelector('#joinNetworkPage .foundNetworks')),
            hiddenSSID: Main.ThrowIfNullOrUndefined(document.querySelector('#joinNetworkPage .hiddenSSID')),
            password: Main.ThrowIfNullOrUndefined(document.querySelector('#joinNetworkPage .password')),
            submit: Main.ThrowIfNullOrUndefined(document.querySelector('#joinNetworkPage button[type=submit]'))
        };
        this.hostNetwork =
        {
            button: Main.ThrowIfNullOrUndefined(document.querySelector('#hostNetworkButton')),
            tbody: Main.ThrowIfNullOrUndefined(document.querySelector('#hostNetworkPage')),
            ssid: Main.ThrowIfNullOrUndefined(document.querySelector('#hostNetworkPage .ssid')),
            password: Main.ThrowIfNullOrUndefined(document.querySelector('#hostNetworkPage .password')),
            channel: Main.ThrowIfNullOrUndefined(document.querySelector('#hostNetworkPage .channel')),
            maxConnections: Main.ThrowIfNullOrUndefined(document.querySelector('#hostNetworkPage .maxConnections')),
            hiddenCheckbox:
            {
                label: Main.ThrowIfNullOrUndefined(document.querySelector('#hostNetworkPage .hiddenNetwork')),
                checkbox: Main.ThrowIfNullOrUndefined(document.querySelector('#hostNetworkPage .hiddenNetwork > input[type=checkbox]'))
            },
            submit: Main.ThrowIfNullOrUndefined(document.querySelector('#hostNetworkPage button[type=submit]'))
        };
        this.message = Main.ThrowIfNullOrUndefined(document.querySelector('#message'));
        this.link = Main.ThrowIfNullOrUndefined(document.querySelector('#link'));

        this.networkDetails.button.addEventListener('click', () => { this.SetPage(1); });
        this.joinNetwork.button.addEventListener('click', () => { this.SetPage(2); });
        this.hostNetwork.button.addEventListener('click', () => { this.SetPage(3); });
        this.SetPage(1);

        this.joinNetwork.hiddenCheckbox.checkbox.addEventListener('change', () =>
        {
            this.joinNetwork.hiddenSSID.style.display = this.joinNetwork.hiddenCheckbox.checkbox.checked ? 'block' : 'none';
        });

        this.joinNetwork.foundNetworks.addEventListener('change', (e) =>
        {
            var selected = this.joinNetwork.foundNetworks.selectedOptions[0];
            this.joinNetwork.password.style.display = selected.getAttribute('usesPassword') === 'true' ? 'block' : 'none';
        });

        this.joinNetwork.submit.addEventListener('click', (e) => { this.SubmitJoinNetwork(e); });
        this.hostNetwork.submit.addEventListener('click', (e) => { this.SubmitHostNetwork(e); });

        this.networkDetails.sleep.addEventListener('click', () =>
        {
            this.message.style.display = 'block';
            this.message.innerText = 'Putting ESP to sleep...';
            this.networkDetails.sleep.disabled = true;

            Main.XHR<IReturnData<{}>>({
                url: '/sleep',
                method: 'POST'
            })
            .then((value) =>
            {
                if (!value.response.error)
                {
                    this.message.innerText = 'ESP is now asleep.';
                }
                else
                {
                    this.message.innerText = 'Failed to put ESP to sleep.';
                }
                this.networkDetails.sleep.disabled = false;
            })
            .catch((reason) =>
            {
                this.message.style.display = 'none';
                this.message.innerText = 'Failed to put ESP to sleep.';
                this.networkDetails.sleep.disabled = false;
            });
        });

        this.networkDetails.reboot.addEventListener('click', () =>
        {
            this.message.style.display = 'block';
            this.message.innerText = 'Rebooting ESP...';
            this.networkDetails.reboot.disabled = true;

            Main.XHR<IReturnData<{}>>({
                url: '/reboot',
                method: 'POST'
            })
            .then((value) =>
            {
                if (!value.response.error)
                {
                    this.message.innerText = 'ESP is now rebooting.';
                }
                else
                {
                    this.message.innerText = 'Failed to reboot ESP.';
                }
                this.networkDetails.reboot.disabled = false;
            })
            .catch((reason) =>
            {
                this.message.style.display = 'none';
                this.message.innerText = 'Failed to reboot ESP.';
                this.networkDetails.reboot.disabled = false;
            });
        });

        var resetClickCounter = 0;
        document.body.addEventListener('click', (e) =>
        {
            if (e.target === this.networkDetails.reset)
            {
                resetClickCounter++;
                if (resetClickCounter > 1)
                {
                    this.message.style.display = 'block';
                    this.message.innerText = 'Resetting ESP...';
                    this.networkDetails.reset.disabled = true;

                    Main.XHR<IReturnData<{}>>({
                        url: '/reset',
                        method: 'POST'
                    })
                    .then((value) =>
                    {
                        if (value.response.error)
                        {
                            this.message.style.display = 'block';
                            this.message.innerText = 'Error resetting ESP.';
                        }
                        else
                        {
                            this.message.style.display = 'block';
                            this.message.innerText = 'ESP reset.';
                        }
                    })
                    .catch((reason) =>
                    {
                        console.error(reason);
                        this.message.style.display = 'none';
                        this.message.innerText = 'Error resetting ESP.';
                    });
                }
                else
                {
                    this.networkDetails.reset.innerText = 'Reset (Are you sure?)';
                }
            }
            else if (resetClickCounter < 2)
            {
                resetClickCounter = 0;
                this.networkDetails.reset.innerText = 'Reset';
            }
        });

        this.NetworkGET();
    }

    private SubmitJoinNetwork(e: Event)
    {
        e.preventDefault();

        this.message.style.display = 'none';

        if (this.joinNetwork.hiddenCheckbox.checkbox.checked && this.joinNetwork.hiddenSSID.value === '')
        {
            this.message.style.display = 'block';
            this.message.textContent = 'Please enter a network name.';
            return;
        }
        else if (this.joinNetwork.foundNetworks.value === '')
        {
            this.message.style.display = 'block';
            this.message.textContent = 'Please select a network.';
            return;
        }

        this.message.style.display = 'block';
        this.message.innerText = 'Attempting to join network...';
        this.joinNetwork.submit.disabled = true;

        Main.XHR<IReturnData<INetworkPOST>>({
            url: '/network/post',
            method: 'POST',
            parameters:
            {
                method: 'STA',
                ssid: this.joinNetwork.hiddenCheckbox.checkbox.checked ? this.joinNetwork.hiddenSSID.value : this.joinNetwork.foundNetworks.value,
                password: this.joinNetwork.password.value,
            },
            headers:
            {
                'Content-Type': 'application/x-www-form-urlencoded'
            }
        })
        .then((value) =>
        {
            if (!value.response.error)
            {
                this.message.style.display = 'block';
                this.message.textContent = 'Successfully joined network.';
                this.link.style.display = 'block';
                this.link.href = `http://${value.response.data.ip}/`;
                this.link.innerText = 'Go to home page.';
                window.location.replace(`http://${value.response.data.ip}/`);
            }
            else
            {
                this.message.style.display = 'block';
                this.message.textContent = 'Failed to join network.';
            }
            this.joinNetwork.submit.disabled = false;
        })
        .catch((reason) =>
        {
            console.error(reason);
            this.message.style.display = 'none';
            this.message.innerText = 'Failed to join network.';
            this.joinNetwork.submit.disabled = false;
        });
    }

    private SubmitHostNetwork(e: Event)
    {
        e.preventDefault();

        this.message.style.display = 'none';

        if (this.hostNetwork.ssid.value === '')
        {
            this.message.style.display = 'block';
            this.message.textContent = 'Please enter a network name.';
            return;
        }

        this.message.style.display = 'block';
        this.message.innerText = 'Attempting to host network...';
        this.hostNetwork.submit.disabled = true;

        Main.XHR<IReturnData<INetworkPOST>>({
            url: '/network/post',
            method: 'POST',
            parameters:
            {
                method: 'AP',
                ssid: this.hostNetwork.ssid.value,
                password: this.hostNetwork.password.value,
                channel: this.hostNetwork.channel.value,
                maxConnections: this.hostNetwork.maxConnections.value,
                hidden: this.hostNetwork.hiddenCheckbox.checkbox.checked
            }
        })
        .then((value) =>
        {
            if (!value.response.error)
            {
                this.message.style.display = 'block';
                this.message.textContent = 'Successfully hosted network.';
                this.link.style.display = 'block';
                this.link.href = `http://${value.response.data.ip}`;
                this.link.innerText = 'Go to home page.';
                window.location.replace(`http://${value.response.data.ip}`);
            }
            else
            {
                this.message.style.display = 'block';
                this.message.textContent = 'Failed to host network.';
            }
            this.hostNetwork.submit.disabled = false;
        })
        .catch((reason) =>
        {
            console.error(reason);
            this.message.style.display = 'none';
            this.message.innerText = 'Failed to host network.';
            this.hostNetwork.submit.disabled = false;
        });
    }

    private SetPage(page: 1 | 2 | 3)
    {
        this.message.style.display = 'none';

        switch (page)
        {
            case 1:
                this.networkDetails.button.classList.add('active');
                this.joinNetwork.button.classList.remove('active');
                this.hostNetwork.button.classList.remove('active');
                this.networkDetails.tbody.style.display = 'table-row-group';
                this.joinNetwork.tbody.style.display = 'none';
                this.hostNetwork.tbody.style.display = 'none';
                break;
            case 2:
                this.networkDetails.button.classList.remove('active');
                this.joinNetwork.button.classList.add('active');
                this.hostNetwork.button.classList.remove('active');
                this.networkDetails.tbody.style.display = 'none';
                this.joinNetwork.tbody.style.display = 'table-row-group';
                this.hostNetwork.tbody.style.display = 'none';
                break;
            case 3:
                this.networkDetails.button.classList.remove('active');
                this.joinNetwork.button.classList.remove('active');
                this.hostNetwork.button.classList.add('active');
                this.networkDetails.tbody.style.display = 'none';
                this.joinNetwork.tbody.style.display = 'none';
                this.hostNetwork.tbody.style.display = 'table-row-group';
                break;
        }
    }

    private NetworkGET()
    {
        Main.XHR<IReturnData<INetworkGET>>({
            url: `/network/get`,
            method: 'GET'
        })
        .then((value) =>
        {
            if (value.response.error)
            {
                this.message.style.display = 'block';
                this.message.textContent = 'Failed to get network information.';
                return;
            }

            // this.FillDetails(value.response.data);
            const details = value.response.data;

            for (const network of details.discoveredNetworks)
            {
                const option = document.createElement('option');
                option.value = network.ssid;
                option.textContent = `${network.ssid} (Secure: ${network.encryptionType != EncryptionType.WIFI_AUTH_OPEN})`;
                option.setAttribute('usesPassword', network.encryptionType != EncryptionType.WIFI_AUTH_OPEN ? 'true' : 'false');
                option.selected = network.ssid === details.config.ssid;
                this.joinNetwork.foundNetworks.appendChild(option);
            }

            if (details.config.networkMode === NetworkMode.WIFI_MODE_NULL)
            {
                this.networkDetails.mode.textContent = 'No network';
                this.networkDetails.ssid.textContent = 'N/A';
                this.networkDetails.ip.textContent = 'N/A';
                this.networkDetails.usesPassword.textContent = 'N/A';
                // this.networkDetails.rssi.textContent = 'N/A';
                this.networkDetails.channel.textContent = 'N/A';
                this.networkDetails.maxConnections.textContent = 'N/A';
                this.networkDetails.hidden.textContent = 'N/A';
            }
            else if (details.config.networkMode === NetworkMode.WIFI_MODE_AP)
            {
                this.networkDetails.mode.textContent = 'Hosting';
                this.networkDetails.ssid.textContent = details.config.ssid;
                this.networkDetails.ip.textContent = details.config.ip;
                this.networkDetails.usesPassword.textContent = details.config.usesPassword ? 'Yes' : 'No';
                // this.networkDetails.rssi.textContent = details.config.rssi;
                this.networkDetails.channel.textContent = details.config.channel.toString();
                this.networkDetails.maxConnections.textContent = details.config.maxConnections.toString();
                this.networkDetails.hidden.textContent = details.config.hidden ? 'Yes' : 'No';

                this.hostNetwork.ssid.value = details.config.ssid;
                for (const option of this.hostNetwork.channel.children)
                {
                    if ((<HTMLOptionElement>option).value === details.config.channel.toString())
                    {
                        (<HTMLOptionElement>option).selected = true;
                        break;
                    }
                }
                for (const option of this.hostNetwork.maxConnections.children)
                {
                    if ((<HTMLOptionElement>option).value === details.config.maxConnections.toString())
                    {
                        (<HTMLOptionElement>option).selected = true;
                        break;
                    }
                }
                this.hostNetwork.hiddenCheckbox.checkbox.checked = details.config.hidden;
            }
            else if (details.config.networkMode === NetworkMode.WIFI_MODE_STA)
            {
                this.networkDetails.mode.textContent = 'Connected';
                this.networkDetails.ssid.textContent = details.config.ssid;
                this.networkDetails.ip.textContent = details.config.ip;
                this.networkDetails.usesPassword.textContent = details.config.usesPassword ? 'Yes' : 'No';
                // this.networkDetails.rssi.textContent = details.config.rssi;
                this.networkDetails.channel.textContent = 'N/A';
                this.networkDetails.maxConnections.textContent = 'N/A';
                this.networkDetails.hidden.textContent = details.config.hidden ? 'Yes' : 'No';

                if (details.config.hidden)
                {
                    this.joinNetwork.hiddenCheckbox.checkbox.checked = true;
                    this.joinNetwork.hiddenSSID.value = details.config.ssid;
                }
                else
                {
                    this.joinNetwork.hiddenCheckbox.checkbox.checked = false;
                }
            }
            else
            {
                this.networkDetails.mode.textContent = 'Unknown';
                this.networkDetails.ssid.textContent = 'N/A';
                this.networkDetails.ip.textContent = 'N/A';
                this.networkDetails.usesPassword.textContent = 'N/A';
                // this.networkDetails.rssi.textContent = 'N/A';
                this.networkDetails.channel.textContent = 'N/A';
                this.networkDetails.maxConnections.textContent = 'N/A';
                this.networkDetails.hidden.textContent = 'N/A';
            }

            this.message.style.display = 'none';
        })
        .catch((reason) =>
        {
            console.error(reason);
            this.message.style.display = 'none';
            // this.message.textContent = 'Failed to get network details.';
        });
    }
}
new Setup();

interface INetworkGET
{
    config:
    {
        networkMode: NetworkMode;
        ssid: string;
        usesPassword: boolean;
        ip: string;
        channel: number;
        maxConnections: number;
        hidden: boolean;
    };
    discoveredNetworks:
    {
        ssid: string;
        rssi: number;
        encryptionType: EncryptionType;
    }[];
}

interface INetworkPOST
{
    ip: string
}

enum NetworkMode
{
    WIFI_MODE_NULL = 0,
    WIFI_MODE_STA,
    WIFI_MODE_AP,
    WIFI_MODE_APSTA,
    WIFI_MODE_MAX
}

enum EncryptionType
{
    WIFI_AUTH_OPEN = 0,
    WIFI_AUTH_WEP,
    WIFI_AUTH_WPA_PSK,
    WIFI_AUTH_WPA2_PSK,
    WIFI_AUTH_WPA_WPA2_PSK,
    WIFI_AUTH_WPA2_ENTERPRISE,
    WIFI_AUTH_MAX
}