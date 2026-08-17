#include "dual-canvas-manager.hpp"
#include <obs-frontend-api.h>

DualCanvasManager::DualCanvasManager() {
    obs_get_video_info(&h_ovi);
    h_ovi.base_width = 1920;
    h_ovi.base_height = 1080;
    h_ovi.output_width = 1920;
    h_ovi.output_height = 1080;
    h_ovi.fps_num = 60;
    h_ovi.fps_den = 1;

    v_ovi = h_ovi;
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

bool DualCanvasManager::create_horizontal_canvas() {
    // In OBS 30 fallback mode, we use the main canvas implicitly.
    return true;
}

bool DualCanvasManager::create_vertical_canvas() {
    // In OBS 30 fallback mode, we use the main canvas implicitly.
    return true;
}

void DualCanvasManager::set_horizontal_resolution(int width, int height) {
    h_ovi.base_width = width;
    h_ovi.base_height = height;
    h_ovi.output_width = width;
    h_ovi.output_height = height;
}

void DualCanvasManager::set_vertical_resolution(int width, int height) {
    v_ovi.base_width = width;
    v_ovi.base_height = height;
    v_ovi.output_width = width;
    v_ovi.output_height = height;
}

void DualCanvasManager::set_horizontal_fps(int fps_num, int fps_den) {
    h_ovi.fps_num = fps_num;
    h_ovi.fps_den = fps_den;
}

void DualCanvasManager::set_vertical_fps(int fps_num, int fps_den) {
    v_ovi.fps_num = fps_num;
    v_ovi.fps_den = fps_den;
}

void DualCanvasManager::set_horizontal_scene(const std::string& scene_name) {
    h_scene_name = scene_name;
    // Fallback mode: switching main scene not handled safely here
}

void DualCanvasManager::set_vertical_scene(const std::string& scene_name) {
    v_scene_name = scene_name;
    // Fallback mode: switching main scene not handled safely here
}
