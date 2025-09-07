
#include "mainwindow.h"
#include <QPushButton>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QWidget>
#include <QTreeWidget>
#include <QHeaderView>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    auto *button = new QPushButton("Open USD File", this);
    primTree = new QTreeWidget(this);
    primTree->setHeaderLabels({"Prim Name", "Type"});
    primTree->header()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(button);
    layout->addWidget(primTree, 1);
    setCentralWidget(central);
    connect(button, &QPushButton::clicked, this, [this]() {
        QString file = QFileDialog::getOpenFileName(this, "Select USD File", "", "USD Files (*.usd *.usda *.usdc *.usdz)");
        if (!file.isEmpty()) {
            pxr::UsdStageRefPtr stage = pxr::UsdStage::Open(file.toStdString());
            if (stage) {
                currentStage = stage;
                std::cout << "Loaded USD stage: " << file.toStdString() << std::endl;
                PopulatePrimTree();
            } else {
                std::cerr << "Failed to load USD stage: " << file.toStdString() << std::endl;
                primTree->clear();
            }
        }
    });
}

void MainWindow::PopulatePrimTree() {
    primTree->clear();
    if (!currentStage) return;
    AddPrimToTree(currentStage->GetPseudoRoot());
}

void MainWindow::AddPrimToTree(const pxr::UsdPrim& prim, QTreeWidgetItem* parentItem) {
    QString name = QString::fromStdString(prim.GetName().GetString());
    QString type = QString::fromStdString(prim.GetTypeName().GetString());
    QTreeWidgetItem* item = new QTreeWidgetItem({name, type});
    if (parentItem) {
        parentItem->addChild(item);
    } else {
        primTree->addTopLevelItem(item);
    }
    for (const auto& child : prim.GetChildren()) {
        AddPrimToTree(child, item);
    }
}
