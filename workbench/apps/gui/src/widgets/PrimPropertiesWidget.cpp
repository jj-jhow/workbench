#include "PrimPropertiesWidget.h"

#include <pxr/usd/usd/attribute.h>
#include <pxr/usd/usd/property.h>
#include <pxr/base/tf/token.h>
#include <pxr/base/vt/value.h>
#include <string>

#include <QLabel>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>

PrimPropertiesWidget::PrimPropertiesWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    auto *label = new QLabel("Properties", this);
    layout->addWidget(label);

    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Name" << "Value");
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(tableWidget);
    setLayout(layout);
}

void PrimPropertiesWidget::setPrim(const pxr::UsdPrim &prim)
{
    tableWidget->clearContents();
    tableWidget->setRowCount(0);

    if (!prim)
        return;

    for (const pxr::UsdProperty &prop : prim.GetProperties())
    {
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(prop.GetName().GetString())));

        if (auto attr = prop.As<pxr::UsdAttribute>())
        {
            pxr::VtValue value;
            if (attr.Get(&value))
            {
                std::stringstream ss;
                ss << value;
                tableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(ss.str())));
            }
        }
    }
}
