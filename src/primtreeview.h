#pragma once
#include <QTreeWidget>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/stage.h>

class PrimTreeView : public QTreeWidget
{
    Q_OBJECT
public:
    explicit PrimTreeView(QWidget *parent = nullptr);
    void populate(pxr::UsdStageRefPtr stage);
    QString getSelectedPrimPath() const;

signals:
    void primSelected(const pxr::UsdPrim &prim);

private:
    void addPrimToTree(const pxr::UsdPrim &prim, QTreeWidgetItem *parentItem = nullptr);
    pxr::UsdStageRefPtr currentStage;
};
