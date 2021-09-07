#pragma once

#include <Preferences.h>

class Storage
{
    static Preferences preferences;

    public:
        //Clear all data.
        static bool Clear(const char* _namespace);

        //Remove a key.
        static bool Remove(const char* _namespace, const char* _key);

        //Put a value.
        static bool PutChar(const char* _namespace, const char* key, int8_t value);
        static bool PutUChar(const char* _namespace, const char* key, uint8_t value);
        static bool PutShort(const char* _namespace, const char* key, int16_t value);
        static bool PutUShort(const char* _namespace, const char* key, uint16_t value);
        static bool PutInt(const char* _namespace, const char* key, const int32_t value);
        static bool PutUInt(const char* _namespace, const char* key, int32_t value);
        static bool PutUInt(const char* _namespace, const char* key, uint32_t value);
        static bool PutLong64(const char* _namespace, const char* key, int64_t value);
        static bool PutULong64(const char* _namespace, const char* key, const uint64_t value);
        static bool PutFloat(const char* _namespace, const char* key, const float_t value);
        static bool PutDouble(const char* _namespace, const char* key, const double_t value);
        static bool PutBool(const char* _namespace, const char* key, const bool value);
        static bool PutString(const char* _namespace, const char* key, const char* value);
        static bool PutBytes(const char* _namespace, const char* key, const void* value, size_t length);

        //Get a value.
        static int8_t GetChar(const char* _namespace, const char* key, int8_t defaultValue);
        static uint8_t GetUChar(const char* _namespace, const char* key, uint8_t defaultValue);
        static int16_t GetShort(const char* _namespace, const char* key, int16_t defaultValue);
        static uint16_t GetUShort(const char* _namespace, const char* key, uint16_t defaultValue);
        static int32_t GetInt(const char* _namespace, const char* key, int32_t defaultValue);
        static uint32_t GetUInt(const char* _namespace, const char* key, uint32_t defaultValue);
        static int64_t GetLong64(const char* _namespace, const char* key, int64_t defaultValue);
        static uint64_t GetULong64(const char* _namespace, const char* key, uint64_t defaultValue);
        static float_t GetFloat(const char* _namespace, const char* key, float_t defaultValue);
        static double_t GetDouble(const char* _namespace, const char* key, double_t defaultValue);
        static bool GetBool(const char* _namespace, const char* key, bool defaultValue);
        static String GetString(const char* _namespace, const char* key, char* defaultValue);
        static size_t GetBytesLength(const char* _namespace, const char* key);
        static bool GetBytes(const char* _namespace, const char* key, void * buf, size_t maxLen);

        //Misc.
        size_t FreeEntries(const char* _namespace);
};