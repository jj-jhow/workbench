#pragma once
#include <QWidget>
#include <pxr/usd/usd/prim.h>

class QTableWidget;

class PrimPropertiesWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PrimPropertiesWidget(QWidget *parent = nullptr);

public slots:
    void setPrim(const pxr::UsdPrim &prim);

private:
    QTableWidget *tableWidget;
};
