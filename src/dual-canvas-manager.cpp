#include "dual-canvas-manager.hpp"
#include <obs-frontend-api.h>

DualCanvasManager::DualCanvasManager() {
    obs_get_video_info(&v_ovi);
    v_ovi.base_width = 1080;
    v_ovi.base_height = 1920;
    v_ovi.output_width = 1080;
    v_ovi.output_height = 1920;
}

DualCanvasManager::~DualCanvasManager() {
    destroy();
}

bool DualCanvasManager::initialize() {
    return true;
}

void DualCanvasManager::destroy() {
}

bool DualCanvasManager::create_vertical_canvas() {
    // In OBS 30 fallback mode, we use the main canvas implicitly.
    return true;
}

void DualCanvasManager::set_vertical_resolution(int width, int height) {
    v_ovi.base_width = width;
    v_ovi.base_height = height;
    v_ovi.output_width = width;
    v_ovi.output_height = height;
}

void DualCanvasManager::set_vertical_fps(int fps_num, int fps_den) {
    v_ovi.fps_num = fps_num;
    v_ovi.fps_den = fps_den;
}

void DualCanvasManager::set_vertical_scene(const std::string& scene_name) {
    v_scene_name = scene_name;
    // Fallback mode: switching main scene not handled safely here
}
