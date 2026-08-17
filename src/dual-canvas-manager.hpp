#pragma once

#include <obs.h>
#include <string>

class DualCanvasManager {
public:
    DualCanvasManager();
    ~DualCanvasManager();

    bool initialize();
    void destroy();

    bool create_horizontal_canvas();
    bool create_vertical_canvas();

    void set_horizontal_resolution(int width, int height);
    void set_vertical_resolution(int width, int height);
    
    void set_horizontal_fps(int fps_num, int fps_den);
    void set_vertical_fps(int fps_num, int fps_den);

    void set_horizontal_scene(const std::string& scene_name);
    void set_vertical_scene(const std::string& scene_name);

private:
    obs_video_info h_ovi;
    obs_video_info v_ovi;
    
    std::string h_scene_name;
    std::string v_scene_name;
};
