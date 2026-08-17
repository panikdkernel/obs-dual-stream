#pragma once

#include <obs.h>
#include <string>

class DualCanvasManager {
public:
    DualCanvasManager();
    ~DualCanvasManager();

    bool initialize();
    void destroy();

    bool create_vertical_canvas();

    void set_vertical_resolution(int width, int height);
    
    void set_vertical_fps(int fps_num, int fps_den);

    void set_vertical_scene(const std::string& scene_name);
    obs_source_t* get_vertical_scene_source() const { return v_scene_source; }

private:
    std::string v_scene_name;
    obs_source_t* v_scene_source = nullptr;
};
