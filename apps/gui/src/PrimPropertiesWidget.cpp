#include "PrimPropertiesWidget.h"
#include <QVBoxLayout>
#include <QLabel>

PrimPropertiesWidget::PrimPropertiesWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Prim Properties", this));
    setLayout(layout);
}

