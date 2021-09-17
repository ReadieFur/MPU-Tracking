#include "EventData.h"
#include <algorithm>
#include <vector>

#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H
template<class TDataType> class EventDispatcher
{
    std::vector<EventData<TDataType>> events;

    public:
        void Register(EventData<TDataType> event)
        {
            if (std::find(events.begin(), events.end(), event) == events.end())
            {
                events.push_back(event);
            }
        }

        void Unregister(EventData<TDataType> event)
        {
            for (auto it = events.begin(); it != events.end(); it++)
            {
                if (*it == event)
                {
                    events.erase(it);
                    break;
                }
            }
        }

        void Dispatch(TDataType param)
        {
            for (EventData<TDataType> event : events)
            {
                event.fptr(event.context, param);
            }
        }
};
#endif