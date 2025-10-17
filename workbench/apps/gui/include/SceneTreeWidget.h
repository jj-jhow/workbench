#pragma once
#include <QWidget>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>

class QTreeWidget;
class QTreeWidgetItem;
class QLineEdit;

class SceneTreeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SceneTreeWidget(QWidget *parent = nullptr);
    void setStage(const pxr::UsdStageRefPtr &stage);
    void clear();

signals:
    void primSelected(const pxr::UsdPrim &prim);

private slots:
    void onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void onFilterChanged(const QString &text);

private:
    void populateTree(const pxr::UsdPrim &prim, QTreeWidgetItem *parentItem = nullptr);

    pxr::UsdStageRefPtr currentStage;
    QLineEdit *searchBar;
    QTreeWidget *treeWidget;
};
