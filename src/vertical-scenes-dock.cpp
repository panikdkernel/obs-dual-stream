#include "vertical-scenes-dock.hpp"
#include <QListWidgetItem>
#include <QStringList>

VerticalScenesDock::VerticalScenesDock(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("VerticalScenesDock");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    scene_list = new QListWidget();
    layout->addWidget(scene_list);

    connect(scene_list, &QListWidget::itemClicked, this, &VerticalScenesDock::on_item_clicked);

    populate_scenes();
}

VerticalScenesDock::~VerticalScenesDock() {
}

void VerticalScenesDock::populate_scenes() {
    scene_list->blockSignals(true);
    scene_list->clear();
    
    QStringList scenes;
    obs_enum_sources(
        [](void* param, obs_source_t* source) {
            if (strcmp(obs_source_get_id(source), "scene") == 0) {
                QString name = obs_source_get_name(source);
                if (name.endsWith(" - Vertical")) {
                    QStringList* list = static_cast<QStringList*>(param);
                    list->append(name);
                }
            }
            return true;
        },
        &scenes
    );
    
    scene_list->addItems(scenes);
    scene_list->blockSignals(false);
}

void VerticalScenesDock::select_scene(const QString& name) {
    QList<QListWidgetItem*> items = scene_list->findItems(name, Qt::MatchExactly);
    if (!items.isEmpty()) {
        scene_list->setCurrentItem(items.first());
        emit scene_selected(name);
    } else {
        // If not found, refresh and try again
        populate_scenes();
        items = scene_list->findItems(name, Qt::MatchExactly);
        if (!items.isEmpty()) {
            scene_list->setCurrentItem(items.first());
            emit scene_selected(name);
        }
    }
}

void VerticalScenesDock::on_item_clicked(QListWidgetItem* item) {
    if (item) {
        emit scene_selected(item->text());
    }
}
