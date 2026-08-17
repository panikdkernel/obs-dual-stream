#include "dual-stream-dock.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QMessageBox>
#include <QPaintEngine>

static void add_display(QWidget* widget, obs_display_t** display, DualStreamDock* dock) {
    gs_init_data info = {};
    info.cx = widget->width();
    info.cy = widget->height();
    info.format = GS_RGBA;
    info.zsformat = GS_ZS_NONE;
#ifdef _WIN32
    info.window.hwnd = (void*)widget->winId();
#endif

    *display = obs_display_create(&info, 0);
    if (*display) {
        obs_display_add_draw_callback(*display, DualStreamDock::render_preview, dock);
    }
}

DualStreamDock::DualStreamDock(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("DualStreamDock");

    v_output = new StreamOutput("dual_vertical");

    setup_ui();
    load_settings();
    refresh_scenes();

    canvas_manager.set_vertical_resolution(config.v_width, config.v_height);
    canvas_manager.set_vertical_fps(config.v_fps_num, config.v_fps_den);

    canvas_manager.create_vertical_canvas();
    
    canvas_manager.set_vertical_scene(config.v_scene.c_str());

    status_timer = new QTimer(this);
    connect(status_timer, &QTimer::timeout, this, &DualStreamDock::update_status);
    status_timer->start(1000);
}

DualStreamDock::~DualStreamDock() {
    save_settings();
    if (display) {
        obs_display_remove_draw_callback(display, DualStreamDock::render_preview, this);
        obs_display_destroy(display);
    }
    delete v_output;
}

void DualStreamDock::setup_ui() {
    QWidget* main_widget = new QWidget();
    QVBoxLayout* main_layout = new QVBoxLayout(main_widget);

    preview_widget = new QWidget();
    preview_widget->setMinimumSize(180, 320);
    preview_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // Crucial attributes for OBS display rendering over Qt
    preview_widget->setAttribute(Qt::WA_PaintOnScreen);
    preview_widget->setAttribute(Qt::WA_StaticContents);
    preview_widget->setAttribute(Qt::WA_NoSystemBackground);
    preview_widget->setAttribute(Qt::WA_OpaquePaintEvent);
    preview_widget->setAttribute(Qt::WA_DontCreateNativeAncestors);
    preview_widget->setAttribute(Qt::WA_NativeWindow);
    
    main_layout->addWidget(preview_widget);
    
    auto create_section = [this](const QString& title, QLineEdit*& server, QLineEdit*& key, QLineEdit*& bitrate, QPushButton*& btn, QLabel*& status) {
        QGroupBox* box = new QGroupBox(title);
        QFormLayout* layout = new QFormLayout(box);
        
        server = new QLineEdit();
        layout->addRow("Server:", server);

        key = new QLineEdit();
        key->setEchoMode(QLineEdit::Password);
        layout->addRow("Key:", key);

        bitrate = new QLineEdit();
        layout->addRow("Bitrate (kbps):", bitrate);

        status = new QLabel("● Stopped");
        layout->addRow("Status:", status);

        btn = new QPushButton("Start");
        layout->addRow(btn);
        
        return box;
    };

    main_layout->addWidget(create_section("VERTICAL STREAM (1080x1920)", v_server_edit, v_key_edit, v_bitrate_edit, start_v_btn, v_status_lbl));

    QWidget* wrapper_widget = new QWidget();
    QVBoxLayout* wrapper_layout = new QVBoxLayout(wrapper_widget);
    wrapper_layout->setContentsMargins(0, 0, 0, 0);
    wrapper_layout->addWidget(main_widget);

    connect(start_v_btn, &QPushButton::clicked, this, &DualStreamDock::on_start_v_clicked);

    add_display(preview_widget, &display, this);
}

void DualStreamDock::load_settings() {
    config = SettingsManager::load();

    v_server_edit->setText(QString::fromStdString(config.v_server));
    v_key_edit->setText(QString::fromStdString(config.v_key));
    v_bitrate_edit->setText(QString::number(config.v_bitrate));
}

void DualStreamDock::save_settings() {
    config.v_server = v_server_edit->text().toStdString();
    config.v_key = v_key_edit->text().toStdString();
    config.v_bitrate = v_bitrate_edit->text().toInt();

    SettingsManager::save(config);
}

static bool enum_scenes(void* param, obs_source_t* source) {
    QStringList* list = static_cast<QStringList*>(param);
    list->append(obs_source_get_name(source));
    return true;
}

void DualStreamDock::on_v_scene_changed(const QString& scene) {
    config.v_scene = scene.toStdString();
    canvas_manager.set_vertical_scene(config.v_scene);
}

std::string DualStreamDock::handle_main_scene_changed(obs_source_t* main_scene) {
    if (!main_scene) return "";
    
    const char* name = obs_source_get_name(main_scene);
    if (!name) return "";
    
    // Avoid infinite loop if somehow a vertical scene becomes the main scene
    std::string sname(name);
    if (sname.find(" - Vertical") != std::string::npos) return "";
    
    std::string v_name = sname + " - Vertical";
    
    obs_source_t* existing = obs_get_source_by_name(v_name.c_str());
    if (!existing) {
        obs_scene_t* scene = obs_scene_from_source(main_scene);
        if (scene) {
            obs_scene_t* dup = obs_scene_duplicate(scene, v_name.c_str(), OBS_SCENE_DUP_REFS);
            if (dup) {
                obs_scene_release(dup);
            }
        }
    } else {
        obs_source_release(existing);
    }
    
    return v_name;
}

void DualStreamDock::on_start_v_clicked() {
    save_settings();
    if (v_output->is_active()) {
        v_output->stop();
    } else {
        v_output->initialize(config.v_width, config.v_height, config.v_fps_num, config.v_fps_den, config.v_bitrate, config.v_server, config.v_key);
        v_output->start();
    }
}

void DualStreamDock::update_status() {
    auto format_status = [](StreamOutput* out) {
        out->update_status();
        auto s = out->get_status();
        if (s.active) {
            return QString("● LIVE - Dropped: %1 / %2").arg(s.dropped_frames).arg(s.total_frames);
        } else {
            return QString("● Stopped");
        }
    };

    v_status_lbl->setText(format_status(v_output));
    
    start_v_btn->setText(v_output->is_active() ? "Stop" : "Start");
}

void DualStreamDock::render_preview(void *data, uint32_t cx, uint32_t cy) {
    DualStreamDock* dock = static_cast<DualStreamDock*>(data);
    obs_source_t* source = dock->canvas_manager.get_vertical_scene_source();
    
    if (source) {
        uint32_t source_cx = obs_source_get_base_width(source);
        uint32_t source_cy = obs_source_get_base_height(source);
        if (source_cx == 0 || source_cy == 0) return;

        float scale_x = (float)cx / (float)source_cx;
        float scale_y = (float)cy / (float)source_cy;
        float scale = (scale_x < scale_y) ? scale_x : scale_y;
        
        gs_matrix_push();
        gs_matrix_scale3f(scale, scale, 1.0f);
        
        obs_source_video_render(source);
        
        gs_matrix_pop();
    }
}
