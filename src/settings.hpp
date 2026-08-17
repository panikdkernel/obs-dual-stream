#pragma once

#include <string>
#include <obs.h>

struct DualStreamConfig {
    int h_width = 1920;
    int h_height = 1080;
    int h_fps_num = 60;
    int h_fps_den = 1;
    int h_bitrate = 8000;
    std::string h_scene;
    std::string h_server;
    std::string h_key;

    int v_width = 1080;
    int v_height = 1920;
    int v_fps_num = 60;
    int v_fps_den = 1;
    int v_bitrate = 6000;
    std::string v_scene;
    std::string v_server;
    std::string v_key;
};

class SettingsManager {
public:
    static DualStreamConfig load();
    static void save(const DualStreamConfig& config);
};
