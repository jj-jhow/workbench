#include "SceneTreeWidget.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTreeWidget>
#include <pxr/usd/usd/prim.h>

#include <QLabel>

SceneTreeWidget::SceneTreeWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    auto *label = new QLabel("Scene Hierarchy", this);
    layout->addWidget(label);

    searchBar = new QLineEdit(this);
    searchBar->setPlaceholderText("Filter prims...");
    treeWidget = new QTreeWidget(this);
    treeWidget->setColumnCount(1);
    treeWidget->setHeaderHidden(true);

    layout->addWidget(searchBar);
    layout->addWidget(treeWidget);
    setLayout(layout);

    connect(treeWidget, &QTreeWidget::currentItemChanged, this, &SceneTreeWidget::onCurrentItemChanged);
    connect(searchBar, &QLineEdit::textChanged, this, &SceneTreeWidget::onFilterChanged);
}

void SceneTreeWidget::setStage(const pxr::UsdStageRefPtr &stage)
{
    clear();
    currentStage = stage;
    if (!stage)
        return;
    auto rootPrim = stage->GetPseudoRoot();
    populateTree(rootPrim);
}

void SceneTreeWidget::clear()
{
    treeWidget->clear();
}

void SceneTreeWidget::populateTree(const pxr::UsdPrim &prim, QTreeWidgetItem *parentItem)
{
    if (!prim)
        return;
    // Skip the pseudo-root itself, but process its children
    if (prim.IsPseudoRoot())
    {
        for (const auto &child : prim.GetChildren())
        {
            populateTree(child, nullptr);
        }
        return;
    }
    auto *item = new QTreeWidgetItem();
    item->setText(0, QString::fromStdString(prim.GetName().GetString()));
    item->setData(0, Qt::UserRole, QString::fromStdString(prim.GetPath().GetString()));
    if (parentItem)
    {
        parentItem->addChild(item);
    }
    else
    {
        treeWidget->addTopLevelItem(item);
    }
    for (const auto &child : prim.GetChildren())
    {
        populateTree(child, item);
    }
}

void SceneTreeWidget::onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if (!current || !currentStage)
        return;

    QString path = current->data(0, Qt::UserRole).toString();
    pxr::SdfPath primPath(path.toStdString());
    pxr::UsdPrim prim = currentStage->GetPrimAtPath(primPath);

    if (prim)
    {
        Q_EMIT primSelected(prim);
    }
}

void SceneTreeWidget::onFilterChanged(const QString &text)
{
    QTreeWidgetItemIterator it(treeWidget);
    while (*it)
    {
        bool match = (*it)->text(0).contains(text, Qt::CaseInsensitive);
        (*it)->setHidden(!match);
        if (match)
        {
            // Expand parents of matched items
            QTreeWidgetItem *parent = (*it)->parent();
            while (parent)
            {
                parent->setHidden(false);
                parent->setExpanded(true);
                parent = parent->parent();
            }
        }
        ++it;
    }
}
