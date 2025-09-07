#include "primtreeview.h"
#include <QTreeWidgetItem>

PrimTreeView::PrimTreeView(QWidget *parent) : QTreeWidget(parent)
{
    setHeaderLabels({"Prim Name", "Type"});
    connect(this, &QTreeWidget::itemSelectionChanged, [this]()
            {
        if (!currentStage) return;
        auto items = selectedItems();
        if (items.isEmpty()) return;
        // Build prim path
        QTreeWidgetItem* item = items.first();
        QStringList pathParts;
        while (item && item->parent()) {
            pathParts.prepend(item->text(0));
            item = item->parent();
        }
        if (item && item->parent() == nullptr && item->text(0) != "")
            pathParts.prepend(item->text(0));
        QString usdPath = "/" + pathParts.join("/");
        usdPath.replace(QRegExp("/+"), "/");
        pxr::UsdPrim prim = currentStage->GetPrimAtPath(pxr::SdfPath(usdPath.toStdString()));
        if (prim)
            emit primSelected(prim); });
}

void PrimTreeView::populate(pxr::UsdStageRefPtr stage)
{
    clear();
    currentStage = stage;
    if (stage)
    {
        addPrimToTree(stage->GetPseudoRoot());
    }
}

void PrimTreeView::addPrimToTree(const pxr::UsdPrim &prim, QTreeWidgetItem *parentItem)
{
    QString name = QString::fromStdString(prim.GetName().GetString());
    QString type = QString::fromStdString(prim.GetTypeName().GetString());
    QTreeWidgetItem *item = new QTreeWidgetItem({name, type});
    if (parentItem)
        parentItem->addChild(item);
    else
        addTopLevelItem(item);
    for (auto child : prim.GetChildren())
    {
        addPrimToTree(child, item);
    }
}

QString PrimTreeView::getSelectedPrimPath() const
{
    auto items = selectedItems();
    if (items.isEmpty())
        return QString();
    QTreeWidgetItem *item = items.first();
    QStringList pathParts;
    while (item && item->parent())
    {
        pathParts.prepend(item->text(0));
        item = item->parent();
    }
    if (item && item->parent() == nullptr && item->text(0) != "")
        pathParts.prepend(item->text(0));
    QString usdPath = "/" + pathParts.join("/");
    usdPath.replace(QRegExp("/+"), "/");
    return usdPath;
}
