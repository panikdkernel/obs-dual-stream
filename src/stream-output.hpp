#pragma once

#include <obs.h>
#include <string>

class StreamOutput {
public:
    StreamOutput(const char* name);
    ~StreamOutput();

    bool initialize(int width, int height, int fps_num, int fps_den, int bitrate, const std::string& server, const std::string& key);
    bool start();
    void stop();
    
    bool is_active() const;
    void update_status();

    struct Status {
        bool active = false;
        double fps = 0.0;
        int dropped_frames = 0;
        int total_frames = 0;
        double bitrate_kbps = 0.0;
    };
    
    Status get_status() const { return current_status; }

private:
    std::string output_name;
    
    obs_output_t* output = nullptr;
    obs_encoder_t* video_encoder = nullptr;
    obs_encoder_t* audio_encoder = nullptr;
    obs_service_t* service = nullptr;

    Status current_status;
    
    static void output_start_cb(void* data, calldata_t* params);
    static void output_stop_cb(void* data, calldata_t* params);
    static void output_reconnect_cb(void* data, calldata_t* params);
    static void output_reconnect_success_cb(void* data, calldata_t* params);
};
