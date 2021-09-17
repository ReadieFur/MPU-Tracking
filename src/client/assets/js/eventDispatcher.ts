export class EventDispatcher
{
    private events: Events = {};

    public AddEventListener(event: string, callback: (data?: any) => any): void
    {
        if (this.events[event] === undefined) { this.events[event] = { listeners: [] }; }
        this.events[event].listeners.push(callback);
    }
  
    public RemoveEventListener(event: string, callback: (data?: any) => any): void
    {
        if (this.events[event] === undefined) { return; }
        for (let i = 0; i < this.events[event].listeners.length; i++) //Modified to what I understand
        { if (this.events[event].listeners[i] === callback) { delete this.events[event].listeners[i]; } }
    }
  
    public DispatchEvent(event: string, data?: any): void
    {
        if (this.events[event] === undefined) { return; }
        this.events[event].listeners.forEach((listener: any) => { listener(data); });
    }
}

type Events =
{
    [eventName: string]:
    {
        listeners: { (data?: any): any; } []
    }
}