
#include "mainwindow.h"
#include <QPushButton>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QWidget>
#include <QHeaderView>
#include <QSplitter>
#include "primtreeview.h"
#include "propertytableview.h"
#include "usdviewport.h"
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/property.h>
#include <pxr/usd/usd/attribute.h>
#include <pxr/usd/usd/relationship.h>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    auto *button = new QPushButton("Open USD File", this);


    auto *splitter = new QSplitter(Qt::Horizontal, this);
    primTree = new PrimTreeView(this);
    splitter->addWidget(primTree);
    propertyTable = new PropertyTableView(this);
    splitter->addWidget(propertyTable);
    usdViewport = new UsdViewport(this);
    splitter->addWidget(usdViewport);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 3);
    splitter->setStretchFactor(2, 5);

    layout->addWidget(button);
    layout->addWidget(splitter, 1);
    setCentralWidget(central);


    connect(button, &QPushButton::clicked, this, [this]() {
        QString file = QFileDialog::getOpenFileName(this, "Select USD File", "", "USD Files (*.usd *.usda *.usdc *.usdz)");
        if (!file.isEmpty()) {
            pxr::UsdStageRefPtr stage = pxr::UsdStage::Open(file.toStdString());
            if (stage) {
                currentStage = stage;
                std::cout << "Loaded USD stage: " << file.toStdString() << std::endl;
                primTree->populate(stage);
                propertyTable->clearContents();
                propertyTable->setRowCount(0);
                usdViewport->setStage(stage);
            } else {
                std::cerr << "Failed to load USD stage: " << file.toStdString() << std::endl;
                primTree->clear();
                propertyTable->clearContents();
                propertyTable->setRowCount(0);
                usdViewport->setStage(nullptr);
            }
        }
    });

    connect(primTree, &PrimTreeView::primSelected, this, [this](const pxr::UsdPrim &prim)
            { propertyTable->showProperties(prim); });
}
