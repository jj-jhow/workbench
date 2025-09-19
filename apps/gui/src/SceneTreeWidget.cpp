#include "SceneTreeWidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <pxr/usd/usd/prim.h>

SceneTreeWidget::SceneTreeWidget(QWidget *parent)
    : QTreeWidget(parent)
{
    setColumnCount(1);
    setHeaderLabel("Scene Tree");
}

void SceneTreeWidget::setStage(const pxr::UsdStageRefPtr& stage)
{
    clear();
    if (!stage) return;
    auto rootPrim = stage->GetDefaultPrim();
    populateTree(rootPrim);
}

void SceneTreeWidget::populateTree(const pxr::UsdPrim& prim, QTreeWidgetItem* parentItem)
{
    if (!prim) return;
    // Skip the pseudo-root itself, but process its children
    // if (prim.IsPseudoRoot()) {
    //     for (const auto& child : prim.GetChildren()) {
    //         populateTree(child, nullptr);
    //     }
    //     return;
    // }
    auto* item = new QTreeWidgetItem();
    item->setText(0, QString::fromStdString(prim.GetName().GetString()));
    if (parentItem) {
        parentItem->addChild(item);
    } else {
        addTopLevelItem(item);
    }
    for (const auto& child : prim.GetChildren()) {
        populateTree(child, item);
    }
}
