
#pragma once
#include <QMainWindow>
#include <QTreeWidget>
#include <pxr/usd/usd/stage.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

private:
    QTreeWidget* primTree;
    pxr::UsdStageRefPtr currentStage;
    void PopulatePrimTree();
    void AddPrimToTree(const pxr::UsdPrim& prim, QTreeWidgetItem* parentItem = nullptr);
};
