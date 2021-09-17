export class Main
{
    public static urlParams: URLSearchParams;
    private static themeColours: HTMLStyleElement;
    private static darkMode:
    {
        container: HTMLLabelElement,
        checkbox: HTMLInputElement,
        checkmark: HTMLSpanElement
    };

    constructor()
    {
        console.log(`
            !   /\\                   ,'|
            o--'O \`.                /  /
             \`--.   \`-----------._,' ,'
                \\                ,--'
                 ) )    _,--(    |
                /,^.---'     )/ \\\\
               ((   \\\\      ((   \\\\
                \\)   \\)      \\)  (/
            -What are you doing here?
        `);

        Main.urlParams = new URLSearchParams(location.search);
        Main.themeColours = Main.ThrowIfNullOrUndefined(document.querySelector('#themeColours'));
        Main.darkMode =
        {
            container: Main.ThrowIfNullOrUndefined(document.querySelector('#darkMode')),
            checkbox: Main.ThrowIfNullOrUndefined(document.querySelector('#darkMode > input[type=checkbox]')),
            checkmark: Main.ThrowIfNullOrUndefined(document.querySelector('#darkMode > span'))
        };
        if (Main.RetreiveCache('ESP_DARK') != 'false') { Main.DarkTheme(true); }
        else { Main.DarkTheme(false); }
        Main.darkMode.container.addEventListener('click', () => { Main.DarkTheme(!Main.darkMode.checkbox.checked); });

        //Better than loading it straight away and getting a longer flashbang (if you have dark mode enabled).
        window.addEventListener('load', () =>
        {
            var staticStyles = document.createElement('style');
            staticStyles.innerHTML = `
                *
                {
                    transition:
                        background 400ms ease 0s,
                        background-color 400ms ease 0s,
                        color 100ms ease 0s;
                }
            `;
            document.head.appendChild(staticStyles);
        });
    }

    public static async XHR<DataType>(data:
    {
        url: string,
        method: 'GET' | 'POST',
        parameters?: Dictionary<any>,
        headers?: Dictionary<string>,
    }): Promise<{xhr: XMLHttpRequest, response: DataType}>
    {
        return new Promise<{xhr: XMLHttpRequest, response: DataType}>((resolve, reject) =>
        {
            var xhr = new XMLHttpRequest();
            xhr.open(data.method, data.url, true);
            
            if (data.headers !== undefined)
            {
                for (var hKey in data.headers)
                {
                    xhr.setRequestHeader(hKey, data.headers[hKey]);
                }
            }

            xhr.onreadystatechange = () =>
            {
                if (xhr.readyState == 4)
                {
                    if (xhr.status == 200)
                    {
                        try
                        {
                            resolve({
                                xhr: xhr,
                                response: JSON.parse(xhr.responseText)
                            });
                        }
                        catch (e)
                        {
                            reject({
                                status: xhr.status,
                                text: xhr.responseText,
                                error: e
                            });
                        }
                    }
                    else
                    {
                        reject({
                            status: xhr.status,
                            text: xhr.statusText,
                            error: undefined
                        });
                    }
                }
            };

            if (data.parameters !== undefined)
            {
                var params = new URLSearchParams();
                for (var pKey in data.parameters)
                {
                    params.set(encodeURIComponent(pKey), encodeURIComponent(data.parameters[pKey]));
                }
                xhr.send(params.toString());
            }
            else
            {
                xhr.send();
            }
        });
    }

    // public static XHTTP<DataType>(url: string, method: string, data: any, callback: (response: DataType) => void)
    // {
    //     var xhttp = new XMLHttpRequest();
    //     xhttp.onreadystatechange = function ()
    //     {
    //         if (this.readyState == 4 && this.status == 200)
    //         {
    //             callback(JSON.parse(this.responseText));
    //         }
    //     }
    //     xhttp.open(method, url, true);
    //     xhttp.setRequestHeader('Content-Type', 'application/json');
    //     xhttp.send(JSON.stringify(data));
    // }

    public static ThrowIfNullOrUndefined(variable: any): any
    {
        if (variable === null || variable === undefined) { throw new TypeError(`${variable} is null or undefined`); }
        return variable;
    }

    public static SetCache(cookie_name: string, value: string, time: number, path: string = '/'): void
    {
        var domain = window.location.hostname;
        var expDate = new Date();
        expDate.setDate(expDate.getDate() + time);
        document.cookie = `${cookie_name}=${value}; expires=${expDate.toUTCString()}; path=${path}; domain=${domain};`;
    }

    public static RetreiveCache(cookie_name: string): string
    {
        var i, x, y, ARRcookies = document.cookie.split(';');
        for (i = 0; i < ARRcookies.length; i++)
        {
            x = ARRcookies[i].substr(0, ARRcookies[i].indexOf('='));
            y = ARRcookies[i].substr(ARRcookies[i].indexOf('=') + 1);
            x = x.replace(/^\s+|\s+$/g, '');
            if (x == cookie_name) { return unescape(y); }
        }
        return '';
    }

    public static DarkTheme(dark: boolean): void
    {
        Main.SetCache('ESP_DARK', dark ? 'true' : 'false', 365);
        Main.darkMode.checkbox.checked = dark;
        Main.themeColours.innerHTML = `
            :root
            {
                --foregroundColour: ${dark ? '255, 255, 255' : '0, 0, 0'};
                --backgroundColour: ${dark ? '13, 17, 23' : '255, 255, 255'};
                --backgroundAltColour: ${dark ? '30, 37, 46' : '225, 225, 225'};
                --accentColour: ${dark ? '100, 0, 255' : '255, 120, 0'};
            }
        `;
    }
}

export type Dictionary<T> = { [key: string]: T };

export interface IXHRReject
{
    status: number,
    text: string,
    error: any
}

export interface IReturnData<DataType>
{
    error: boolean;
    data: DataType
}