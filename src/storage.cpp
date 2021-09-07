#include "storage.h"

//Thanks GitHub copilot for making this much easier and faster to make.

//Preferences.
Preferences Storage::preferences;

//Clear all data.
bool Storage::Clear(const char* _namespace)
{
    if (!preferences.begin(_namespace)) { return false; }
    if (!preferences.clear()) { return false; }
    preferences.end();
    return true;
}

//Remove a key.
bool Storage::Remove(const char* _namespace, const char* _key)
{
    if (!preferences.begin(_namespace)) { return false; }
    if (!preferences.remove(_key)) { return false; }
    preferences.end();
    return true;
}

//Put a value.
bool Storage::PutChar(const char* _namespace, const char* key, int8_t value)
{
    if (!preferences.begin(_namespace)) { return false; }
    if (preferences.putChar(key, value) == 0) { return false; }
    preferences.end();
    return true;
}

bool Storage::PutUChar(const char* _namespace, const char* key, uint8_t value)
{
    if (!preferences.begin(_namespace)) { return false; }
    if (preferences.putUChar(key, value) == 0) { return false; }
    preferences.end();
    return true;
}

bool Storage::PutShort(const char* _namespace, const char* key, int16_t value)
{
    if (!preferences.begin(_namespace)) { return false; }
    if (preferences.putShort(key, value) == 0) { return false; }
    preferences.end();
    return true;
}

bool Storage::PutUShort(const char* _namespace, const char* key, uint16_t value)
{
    if (!preferences.begin(_namespace)) { return false; }
    if (preferences.putUShort(key, value) == 0) { return false; }
    preferences.end();
    return true;
}

bool Storage::PutInt(const char* _namespace, const char* key, int32_t value)
{
    if (!preferences.begin(_namespace)) { return false; }
    if (preferences.putInt(key, value) == 0) { return false; }
    preferences.end();
    return true;
}

bool Storage::PutUInt(const char* _namespace, const char* key, uint32_t value)
{
    if (!preferences.begin(_namespace)) { return false; }
    if (preferences.putUInt(key, value) == 0) { return false; }
    preferences.end();
    return true;
}

bool Storage::PutLong64(const char* _namespace, const char* key, int64_t value)
{
    if (!preferences.begin(_namespace)) { return false; }
    if (preferences.putLong64(key, value) == 0) { return false; }
    preferences.end();
    return true;
}

bool Storage::PutULong64(const char* _namespace, const char* key, uint64_t value)
{
    if (!preferences.begin(_namespace)) { return false; }
    if (preferences.putULong64(key, value) == 0) { return false; }
    preferences.end();
    return true;
}

bool Storage::PutFloat(const char* _namespace, const char* key, float value)
{
    if (!preferences.begin(_namespace)) { return false; }
    if (preferences.putFloat(key, value) == 0) { return false; }
    preferences.end();
    return true;
}

bool Storage::PutDouble(const char* _namespace, const char* key, double value)
{
    if (!preferences.begin(_namespace)) { return false; }
    if (preferences.putDouble(key, value) == 0) { return false; }
    preferences.end();
    return true;
}

bool Storage::PutBool(const char* _namespace, const char* key, bool value)
{
    if (!preferences.begin(_namespace)) { return false; }
    if (preferences.putBool(key, value) == 0) { return false; }
    preferences.end();
    return true;
}

bool Storage::PutString(const char* _namespace, const char* key, const char* value)
{
    if (!preferences.begin(_namespace)) { return false; }
    if (preferences.putString(key, value) == 0) { return false; }
    preferences.end();
    return true;
}

bool Storage::PutBytes(const char* _namespace, const char* key, const void* value, size_t length)
{
    if (!preferences.begin(_namespace)) { return false; }
    if (preferences.putBytes(key, value, length) == 0) { return false; }
    preferences.end();
    return true;
}

//Get a value.
int8_t Storage::GetChar(const char* _namespace, const char* key, int8_t defaultValue)
{
    if (!preferences.begin(_namespace)) { return defaultValue; }
    int8_t value = preferences.getChar(key, defaultValue);
    preferences.end();
    return value;
}

uint8_t Storage::GetUChar(const char* _namespace, const char* key, uint8_t defaultValue)
{
    if (!preferences.begin(_namespace)) { return defaultValue; }
    uint8_t value = preferences.getUChar(key, defaultValue);
    preferences.end();
    return value;
}

int16_t Storage::GetShort(const char* _namespace, const char* key, int16_t defaultValue)
{
    if (!preferences.begin(_namespace)) { return defaultValue; }
    int16_t value = preferences.getShort(key, defaultValue);
    preferences.end();
    return value;
}

uint16_t Storage::GetUShort(const char* _namespace, const char* key, uint16_t defaultValue)
{
    if (!preferences.begin(_namespace)) { return defaultValue; }
    uint16_t value = preferences.getUShort(key, defaultValue);
    preferences.end();
    return value;
}

int32_t Storage::GetInt(const char* _namespace, const char* key, int32_t defaultValue)
{
    if (!preferences.begin(_namespace)) { return defaultValue; }
    int32_t value = preferences.getInt(key, defaultValue);
    preferences.end();
    return value;
}

uint32_t Storage::GetUInt(const char* _namespace, const char* key, uint32_t defaultValue)
{
    if (!preferences.begin(_namespace)) { return defaultValue; }
    uint32_t value = preferences.getUInt(key, defaultValue);
    preferences.end();
    return value;
}

int64_t Storage::GetLong64(const char* _namespace, const char* key, int64_t defaultValue)
{
    if (!preferences.begin(_namespace)) { return defaultValue; }
    int64_t value = preferences.getLong64(key, defaultValue);
    preferences.end();
    return value;
}

uint64_t Storage::GetULong64(const char* _namespace, const char* key, uint64_t defaultValue)
{
    if (!preferences.begin(_namespace)) { return defaultValue; }
    uint64_t value = preferences.getULong64(key, defaultValue);
    preferences.end();
    return value;
}

float_t Storage::GetFloat(const char* _namespace, const char* key, float_t defaultValue)
{
    if (!preferences.begin(_namespace)) { return defaultValue; }
    float_t value = preferences.getFloat(key, defaultValue);
    preferences.end();
    return value;
}

double_t Storage::GetDouble(const char* _namespace, const char* key, double_t defaultValue)
{
    if (!preferences.begin(_namespace)) { return defaultValue; }
    double_t value = preferences.getDouble(key, defaultValue);
    preferences.end();
    return value;
}

bool Storage::GetBool(const char* _namespace, const char* key, bool defaultValue)
{
    if (!preferences.begin(_namespace)) { return defaultValue; }
    bool value = preferences.getBool(key, defaultValue);
    preferences.end();
    return value;
}

String Storage::GetString(const char* _namespace, const char* key, char* defaultValue)
{
    if (!preferences.begin(_namespace)) { return defaultValue; }
    String value = preferences.getString(key, defaultValue);
    preferences.end();
    return value;
}

size_t Storage::GetBytesLength(const char* _namespace, const char* key)
{
    if (!preferences.begin(_namespace)) { return 0; }
    size_t value = preferences.getBytesLength(key);
    preferences.end();
    return value;
}

bool Storage::GetBytes(const char* _namespace, const char* key, void * buf, size_t maxLen)
{
    if (!preferences.begin(_namespace)) { return NULL; }
    bool value = preferences.getBytes(key, buf, maxLen) == 0;
    preferences.end();
    return value;
}

//Misc.
size_t Storage::FreeEntries(const char* _namespace)
{
    if (!preferences.begin(_namespace)) { return 0; }
    size_t value = preferences.freeEntries();
    preferences.end();
    return value;
}