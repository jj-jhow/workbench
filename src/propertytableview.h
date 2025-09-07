#pragma once
#include <QTableWidget>
#include <pxr/usd/usd/prim.h>

class PropertyTableView : public QTableWidget
{
    Q_OBJECT
public:
    explicit PropertyTableView(QWidget *parent = nullptr);
    void showProperties(const pxr::UsdPrim &prim);
};
