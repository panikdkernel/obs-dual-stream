#pragma once

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include "dual-canvas-manager.hpp"
#include "stream-output.hpp"
#include "settings.hpp"

class DualStreamDock : public QWidget {
    Q_OBJECT

public:
    DualStreamDock(QWidget* parent = nullptr);
    ~DualStreamDock();

    static void render_preview(void *data, uint32_t cx, uint32_t cy);

    void handle_main_scene_changed(obs_source_t* main_scene);

private slots:
    void on_start_v_clicked();
    
    void on_v_scene_changed(const QString& scene);

    void save_settings();
    void load_settings();
    
    void update_status();
    void refresh_scenes();

private:
    void setup_ui();
    void populate_scenes(QComboBox* box, const QString& current);

    DualCanvasManager canvas_manager;

    StreamOutput* v_output = nullptr;

    DualStreamConfig config;

    // UI Elements
    QWidget* preview_widget;
    obs_display_t* display = nullptr;
    
    QComboBox* v_scene_box;
    
    QLineEdit* v_server_edit;
    QLineEdit* v_key_edit;
    QLineEdit* v_bitrate_edit;
    
    QPushButton* start_v_btn;
    
    QLabel* v_status_lbl;

    QTimer* status_timer;
};
