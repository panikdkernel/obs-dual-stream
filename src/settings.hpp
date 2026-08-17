#pragma once

#include <string>
#include <obs.h>

struct DualStreamConfig {
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
