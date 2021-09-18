#pragma once

//I dont have SPIFFS up yet so I shall be putting the data into variables for now.
//This won't work for long as I will quickly excced the sketch size limit.
class WebPages
{
public:
    static const char* main_css;
    static const char* main_js;

    static const char* network_html;
    static const char* network_js;

    static const char* motion_html;
    static const char* motion_js;

    static const char* client_js;

    static const char* three_min_js;
};