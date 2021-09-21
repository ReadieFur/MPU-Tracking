#pragma once

#include <U8x8lib.h>

class Display
{
    static bool configured;
    static bool isEnabled;
    static U8X8_SSD1306_128X64_NONAME_SW_I2C *u8x8;

    Display() {};
    ~Display() {};

    public:
        static bool IsEnabled();
        static void Enable();
        static void Disable();
        static void Clear();
        static U8X8_SSD1306_128X64_NONAME_SW_I2C U8X8();
        static void DrawString(int x, int y, const char *string, const uint8_t *font = u8x8_font_5x7_f);
        static void DrawTwoPartString(int x, int y, const char *part1, const char *part2, const uint8_t *part1Font = u8x8_font_amstrad_cpc_extended_f, const uint8_t *part2Font = u8x8_font_5x7_f);
        static void Log(const char *string);
        // static void DrawXBM(int16_t xMove, int16_t yMove, int16_t width, int16_t height, const uint8_t *xbm);
};