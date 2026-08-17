#include "dual-stream-dock.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QMessageBox>

DualStreamDock::DualStreamDock(QWidget* parent)
    : QDockWidget("Dual Stream", parent)
{
    setObjectName("DualStreamDock");
    setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    h_output = new StreamOutput("dual_horizontal");
    v_output = new StreamOutput("dual_vertical");

    setup_ui();
    load_settings();
    refresh_scenes();

    canvas_manager.set_horizontal_resolution(config.h_width, config.h_height);
    canvas_manager.set_vertical_resolution(config.v_width, config.v_height);
    canvas_manager.set_horizontal_fps(config.h_fps_num, config.h_fps_den);
    canvas_manager.set_vertical_fps(config.v_fps_num, config.v_fps_den);

    canvas_manager.create_horizontal_canvas();
    canvas_manager.create_vertical_canvas();
    
    canvas_manager.set_horizontal_scene(config.h_scene.c_str());
    canvas_manager.set_vertical_scene(config.v_scene.c_str());

    status_timer = new QTimer(this);
    connect(status_timer, &QTimer::timeout, this, &DualStreamDock::update_status);
    status_timer->start(1000);
}

DualStreamDock::~DualStreamDock() {
    save_settings();
    delete h_output;
    delete v_output;
}

void DualStreamDock::setup_ui() {
    QWidget* main_widget = new QWidget();
    QVBoxLayout* main_layout = new QVBoxLayout(main_widget);

    auto create_section = [this](const QString& title, QComboBox*& scene_box, QLineEdit*& server, QLineEdit*& key, QLineEdit*& bitrate, QPushButton*& btn, QLabel*& status) {
        QGroupBox* box = new QGroupBox(title);
        QFormLayout* layout = new QFormLayout(box);
        
        scene_box = new QComboBox();
        layout->addRow("Scene:", scene_box);

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

    main_layout->addWidget(create_section("HORIZONTAL (1920x1080)", h_scene_box, h_server_edit, h_key_edit, h_bitrate_edit, start_h_btn, h_status_lbl));
    main_layout->addWidget(create_section("VERTICAL (1080x1920)", v_scene_box, v_server_edit, v_key_edit, v_bitrate_edit, start_v_btn, v_status_lbl));

    start_both_btn = new QPushButton("START BOTH STREAMS");
    main_layout->addWidget(start_both_btn);

    QPushButton* refresh_btn = new QPushButton("Refresh Scenes");
    main_layout->addWidget(refresh_btn);

    setWidget(main_widget);

    connect(start_h_btn, &QPushButton::clicked, this, &DualStreamDock::on_start_h_clicked);
    connect(start_v_btn, &QPushButton::clicked, this, &DualStreamDock::on_start_v_clicked);
    connect(start_both_btn, &QPushButton::clicked, this, &DualStreamDock::on_start_both_clicked);
    connect(refresh_btn, &QPushButton::clicked, this, &DualStreamDock::refresh_scenes);

    connect(h_scene_box, &QComboBox::currentTextChanged, this, &DualStreamDock::on_h_scene_changed);
    connect(v_scene_box, &QComboBox::currentTextChanged, this, &DualStreamDock::on_v_scene_changed);
}

void DualStreamDock::load_settings() {
    config = SettingsManager::load();

    h_server_edit->setText(QString::fromStdString(config.h_server));
    h_key_edit->setText(QString::fromStdString(config.h_key));
    h_bitrate_edit->setText(QString::number(config.h_bitrate));

    v_server_edit->setText(QString::fromStdString(config.v_server));
    v_key_edit->setText(QString::fromStdString(config.v_key));
    v_bitrate_edit->setText(QString::number(config.v_bitrate));
}

void DualStreamDock::save_settings() {
    config.h_server = h_server_edit->text().toStdString();
    config.h_key = h_key_edit->text().toStdString();
    config.h_bitrate = h_bitrate_edit->text().toInt();

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

void DualStreamDock::populate_scenes(QComboBox* box, const QString& current) {
    box->blockSignals(true);
    box->clear();
    
    QStringList scenes;
    obs_enum_sources(
        [](void* param, obs_source_t* source) {
            if (strcmp(obs_source_get_id(source), "scene") == 0) {
                QStringList* list = static_cast<QStringList*>(param);
                list->append(obs_source_get_name(source));
            }
            return true;
        },
        &scenes
    );
    
    box->addItems(scenes);
    int idx = box->findText(current);
    if (idx >= 0) box->setCurrentIndex(idx);
    
    box->blockSignals(false);
}

void DualStreamDock::refresh_scenes() {
    populate_scenes(h_scene_box, QString::fromStdString(config.h_scene));
    populate_scenes(v_scene_box, QString::fromStdString(config.v_scene));
}

void DualStreamDock::on_h_scene_changed(const QString& scene) {
    config.h_scene = scene.toStdString();
    canvas_manager.set_horizontal_scene(config.h_scene);
}

void DualStreamDock::on_v_scene_changed(const QString& scene) {
    config.v_scene = scene.toStdString();
    canvas_manager.set_vertical_scene(config.v_scene);
}

void DualStreamDock::on_start_h_clicked() {
    save_settings();
    if (h_output->is_active()) {
        h_output->stop();
    } else {
        h_output->initialize(config.h_width, config.h_height, config.h_fps_num, config.h_fps_den, config.h_bitrate, config.h_server, config.h_key);
        h_output->start();
    }
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

void DualStreamDock::on_start_both_clicked() {
    if (!h_output->is_active()) on_start_h_clicked();
    if (!v_output->is_active()) on_start_v_clicked();
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

    h_status_lbl->setText(format_status(h_output));
    v_status_lbl->setText(format_status(v_output));
    
    start_h_btn->setText(h_output->is_active() ? "Stop" : "Start");
    start_v_btn->setText(v_output->is_active() ? "Stop" : "Start");
}
