#include "splash.h"
//I couldnt seem to get the bitmap loading to work so I shall do it manually.
void Splash::Show()
{
    Display::Clear();
    Display::DrawString(0, 0, "MPU Tracking", u8x8_font_amstrad_cpc_extended_f);
    Display::DrawTwoPartString(0, 1, "By: ", "Readie");
    Display::DrawTwoPartString(0, 2, "Version: ", "1.0.0");
}