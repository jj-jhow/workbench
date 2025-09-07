
#pragma once
#include <QMainWindow>
#include "primtreeview.h"
#include "propertytableview.h"
#include <pxr/usd/usd/stage.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

private:
    PrimTreeView *primTree;
    PropertyTableView *propertyTable;
    class UsdViewport *usdViewport;
    pxr::UsdStageRefPtr currentStage;
};
