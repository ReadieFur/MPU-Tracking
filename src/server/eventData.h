#ifndef EVENT_H
#define EVENT_H
template<class TDataType> class EventData
{
    public:
        void (*fptr)(void*, TDataType);
        void* context = nullptr;
    
        EventData(void (*fptr)(void*, TDataType), void* context = nullptr)
        {
            this->fptr = fptr;
            this->context = context;
        }

        bool operator==(const EventData& other)
        {
            return (
                this->fptr == other.fptr &&
                this->context == other.context
            );
        }
};
#endif