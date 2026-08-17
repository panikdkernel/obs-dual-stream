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

private:
    std::string v_scene_name;
};
