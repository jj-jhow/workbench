#pragma once
#include <QTreeWidget>
#include <pxr/usd/usd/stage.h>

class SceneTreeWidget : public QTreeWidget {
    Q_OBJECT
public:
    explicit SceneTreeWidget(QWidget *parent = nullptr);
    void setStage(const pxr::UsdStageRefPtr& stage);
private:
    void populateTree(const pxr::UsdPrim& prim, QTreeWidgetItem* parentItem = nullptr);
};
