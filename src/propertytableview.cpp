
#include "propertytableview.h"
#include <QHeaderView>
#include <pxr/usd/usd/property.h>
#include <pxr/usd/usd/attribute.h>
#include <pxr/usd/usd/relationship.h>
#include <pxr/base/tf/stringUtils.h>

PropertyTableView::PropertyTableView(QWidget *parent) : QTableWidget(parent)
{
    setColumnCount(2);
    setHorizontalHeaderLabels({"Property", "Value"});
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void PropertyTableView::showProperties(const pxr::UsdPrim &prim)
{
    clearContents();
    setRowCount(0);
    if (!prim)
        return;
    int row = 0;
    for (const auto &prop : prim.GetProperties())
    {
        std::string propName = prop.GetName().GetString();
        std::string valueStr;
        if (prop.Is<pxr::UsdAttribute>())
        {
            pxr::UsdAttribute attr = prop.As<pxr::UsdAttribute>();
            pxr::VtValue val;
            if (attr.Get(&val))
            {
                valueStr = pxr::TfStringify(val);
            }
            else
            {
                valueStr = "<unavailable>";
            }
        }
        else if (prop.Is<pxr::UsdRelationship>())
        {
            valueStr = "<relationship>";
        }
        else
        {
            valueStr = "<unknown>";
        }
        insertRow(row);
        setItem(row, 0, new QTableWidgetItem(QString::fromStdString(propName)));
        setItem(row, 1, new QTableWidgetItem(QString::fromStdString(valueStr)));
        ++row;
    }
}
