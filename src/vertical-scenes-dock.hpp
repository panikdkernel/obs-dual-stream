#pragma once

#include <QWidget>
#include <QListWidget>
#include <QString>
#include <QVBoxLayout>
#include <obs.h>

class VerticalScenesDock : public QWidget {
    Q_OBJECT

public:
    VerticalScenesDock(QWidget* parent = nullptr);
    ~VerticalScenesDock();

    void select_scene(const QString& name);
    void populate_scenes();

signals:
    void scene_selected(const QString& name);

private slots:
    void on_item_clicked(QListWidgetItem* item);

private:
    QListWidget* scene_list;
};
