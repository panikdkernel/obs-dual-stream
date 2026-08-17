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

private slots:
    void on_start_both_clicked();
    void on_start_h_clicked();
    void on_start_v_clicked();
    
    void on_h_scene_changed(const QString& scene);
    void on_v_scene_changed(const QString& scene);

    void save_settings();
    void load_settings();
    
    void update_status();
    void refresh_scenes();

private:
    void setup_ui();
    void populate_scenes(QComboBox* box, const QString& current);

    DualCanvasManager canvas_manager;
    StreamOutput* h_output = nullptr;
    StreamOutput* v_output = nullptr;

    DualStreamConfig config;

    // UI Elements
    QComboBox* h_scene_box;
    QComboBox* v_scene_box;
    
    QLineEdit* h_server_edit;
    QLineEdit* h_key_edit;
    QLineEdit* h_bitrate_edit;
    
    QLineEdit* v_server_edit;
    QLineEdit* v_key_edit;
    QLineEdit* v_bitrate_edit;
    
    QPushButton* start_h_btn;
    QPushButton* start_v_btn;
    QPushButton* start_both_btn;
    
    QLabel* h_status_lbl;
    QLabel* v_status_lbl;

    QTimer* status_timer;
};
