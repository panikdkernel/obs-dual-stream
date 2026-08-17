#include "settings.hpp"
#include <obs-frontend-api.h>
#include <util/config-file.h>

DualStreamConfig SettingsManager::load() {
    DualStreamConfig config;
    config_t* cfg = obs_frontend_get_profile_config();
    if (!cfg) return config;

    config.h_width = config_get_default_int(cfg, "DualStream", "h_width");
    if (config.h_width == 0) config.h_width = 1920;
    else config.h_width = config_get_int(cfg, "DualStream", "h_width");

    config.h_height = config_get_default_int(cfg, "DualStream", "h_height");
    if (config.h_height == 0) config.h_height = 1080;
    else config.h_height = config_get_int(cfg, "DualStream", "h_height");
    
    config.h_bitrate = config_get_default_int(cfg, "DualStream", "h_bitrate");
    if (config.h_bitrate == 0) config.h_bitrate = 8000;
    else config.h_bitrate = config_get_int(cfg, "DualStream", "h_bitrate");

    const char* h_scene = config_get_string(cfg, "DualStream", "h_scene");
    if (h_scene) config.h_scene = h_scene;

    const char* h_server = config_get_string(cfg, "DualStream", "h_server");
    if (h_server) config.h_server = h_server;

    const char* h_key = config_get_string(cfg, "DualStream", "h_key");
    if (h_key) config.h_key = h_key;

    // Vertical
    config.v_width = config_get_default_int(cfg, "DualStream", "v_width");
    if (config.v_width == 0) config.v_width = 1080;
    else config.v_width = config_get_int(cfg, "DualStream", "v_width");

    config.v_height = config_get_default_int(cfg, "DualStream", "v_height");
    if (config.v_height == 0) config.v_height = 1920;
    else config.v_height = config_get_int(cfg, "DualStream", "v_height");
    
    config.v_bitrate = config_get_default_int(cfg, "DualStream", "v_bitrate");
    if (config.v_bitrate == 0) config.v_bitrate = 6000;
    else config.v_bitrate = config_get_int(cfg, "DualStream", "v_bitrate");

    const char* v_scene = config_get_string(cfg, "DualStream", "v_scene");
    if (v_scene) config.v_scene = v_scene;

    const char* v_server = config_get_string(cfg, "DualStream", "v_server");
    if (v_server) config.v_server = v_server;

    const char* v_key = config_get_string(cfg, "DualStream", "v_key");
    if (v_key) config.v_key = v_key;

    return config;
}

void SettingsManager::save(const DualStreamConfig& config) {
    config_t* cfg = obs_frontend_get_profile_config();
    if (!cfg) return;

    config_set_int(cfg, "DualStream", "h_width", config.h_width);
    config_set_int(cfg, "DualStream", "h_height", config.h_height);
    config_set_int(cfg, "DualStream", "h_bitrate", config.h_bitrate);
    config_set_string(cfg, "DualStream", "h_scene", config.h_scene.c_str());
    config_set_string(cfg, "DualStream", "h_server", config.h_server.c_str());
    config_set_string(cfg, "DualStream", "h_key", config.h_key.c_str());

    config_set_int(cfg, "DualStream", "v_width", config.v_width);
    config_set_int(cfg, "DualStream", "v_height", config.v_height);
    config_set_int(cfg, "DualStream", "v_bitrate", config.v_bitrate);
    config_set_string(cfg, "DualStream", "v_scene", config.v_scene.c_str());
    config_set_string(cfg, "DualStream", "v_server", config.v_server.c_str());
    config_set_string(cfg, "DualStream", "v_key", config.v_key.c_str());

    config_save_safe(cfg, "tmp", nullptr);
}
