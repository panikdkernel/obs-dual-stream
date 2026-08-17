#include <obs-module.h>
#include <obs-frontend-api.h>
#include "dual-stream-dock.hpp"
#include <QApplication>
#include <QMainWindow>

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-dual-stream", "en-US")

static DualStreamDock *dock = nullptr;

static void my_frontend_event_cb(enum obs_frontend_event event, void *private_data)
{
    (void)private_data;

    if (event == OBS_FRONTEND_EVENT_FINISHED_LOADING) {
        QMainWindow *main_window = (QMainWindow *)obs_frontend_get_main_window();
        if (main_window) {
            dock = new DualStreamDock(main_window);
            obs_frontend_add_dock_by_id("DualStreamDock", "Dual Stream", dock);
        }
    }
}

extern "C" {
    MODULE_EXPORT const char *obs_module_description(void)
    {
        return "OBS Dual Stream Plugin (Horizontal and Vertical)";
    }

    MODULE_EXPORT bool obs_module_load(void)
    {
        obs_frontend_add_event_callback(my_frontend_event_cb, nullptr);
        return true;
    }

    MODULE_EXPORT void obs_module_unload(void)
    {
        if (dock) {
            obs_frontend_remove_dock("DualStreamDock");
            delete dock;
            dock = nullptr;
        }
    }
}
