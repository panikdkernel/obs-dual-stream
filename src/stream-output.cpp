#include "stream-output.hpp"
#include <util/bmem.h>

StreamOutput::StreamOutput(const char* name) : output_name(name) {}

StreamOutput::~StreamOutput() {
    stop();
    if (service) {
        obs_service_release(service);
        service = nullptr;
    }
    if (output) {
        obs_output_release(output);
        output = nullptr;
    }
    if (video_encoder) {
        obs_encoder_release(video_encoder);
        video_encoder = nullptr;
    }
    if (audio_encoder) {
        obs_encoder_release(audio_encoder);
        audio_encoder = nullptr;
    }
}

bool StreamOutput::initialize(int width, int height, int fps_num, int fps_den, int bitrate, const std::string& server, const std::string& key) {

    // Create RTMP output
    obs_data_t* output_settings = obs_data_create();
    output = obs_output_create("rtmp_output", (output_name + "_output").c_str(), output_settings, nullptr);
    obs_data_release(output_settings);
    
    if (!output) return false;

    // Connect callbacks
    signal_handler_t* handler = obs_output_get_signal_handler(output);
    signal_handler_connect(handler, "start", output_start_cb, this);
    signal_handler_connect(handler, "stop", output_stop_cb, this);
    signal_handler_connect(handler, "reconnect", output_reconnect_cb, this);
    signal_handler_connect(handler, "reconnect_success", output_reconnect_success_cb, this);

    // Setup Video Encoder
    obs_data_t* v_settings = obs_data_create();
    obs_data_set_int(v_settings, "bitrate", bitrate);
    video_encoder = obs_video_encoder_create("obs_x264", (output_name + "_video").c_str(), v_settings, nullptr);
    obs_data_release(v_settings);
    
    if (!video_encoder) return false;
    
    // Fallback: use main OBS video output since canvases aren't supported in OBS 30
    video_t* video = obs_get_video();
    obs_encoder_set_video(video_encoder, video);

    // Setup Audio Encoder
    obs_data_t* a_settings = obs_data_create();
    obs_data_set_int(a_settings, "bitrate", 160);
    audio_encoder = obs_audio_encoder_create("ffmpeg_aac", (output_name + "_audio").c_str(), a_settings, 0, nullptr);
    obs_data_release(a_settings);
    
    if (!audio_encoder) return false;

    // OBS main audio
    audio_t* audio = obs_get_audio();
    obs_encoder_set_audio(audio_encoder, audio);

    // Set Encoders to Output
    obs_output_set_video_encoder(output, video_encoder);
    obs_output_set_audio_encoder(output, audio_encoder, 0);

    // Setup RTMP Service
    obs_data_t* s_settings = obs_data_create();
    obs_data_set_string(s_settings, "server", server.c_str());
    obs_data_set_string(s_settings, "key", key.c_str());
    
    service = obs_service_create("rtmp_custom", (output_name + "_service").c_str(), s_settings, nullptr);
    obs_data_release(s_settings);
    
    if (!service) return false;
    obs_output_set_service(output, service);
    
    return true;
}

bool StreamOutput::start() {
    if (!output) return false;
    return obs_output_start(output);
}

void StreamOutput::stop() {
    if (!output) return;
    obs_output_stop(output);
}

bool StreamOutput::is_active() const {
    return output ? obs_output_active(output) : false;
}

void StreamOutput::update_status() {
    if (!output) return;
    
    current_status.active = obs_output_active(output);
    if (current_status.active) {
        current_status.total_frames = obs_output_get_total_frames(output);
        current_status.dropped_frames = obs_output_get_frames_dropped(output);
        // Average bitrate calculation would be more involved, simplistic approach:
        // Or using output congestion info:
        // current_status.bitrate_kbps = ...
    }
}

void StreamOutput::output_start_cb(void* data, calldata_t* params) {
    (void)params;
    auto* self = static_cast<StreamOutput*>(data);
    self->current_status.active = true;
}

void StreamOutput::output_stop_cb(void* data, calldata_t* params) {
    (void)params;
    auto* self = static_cast<StreamOutput*>(data);
    self->current_status.active = false;
}

void StreamOutput::output_reconnect_cb(void* data, calldata_t* params) {
    (void)params;
    (void)data;
}

void StreamOutput::output_reconnect_success_cb(void* data, calldata_t* params) {
    (void)params;
    (void)data;
}
