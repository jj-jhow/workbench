#include "SceneTreeWidget.h"
#include <QVBoxLayout>
#include <QLabel>

SceneTreeWidget::SceneTreeWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Scene Tree", this));
    setLayout(layout);
}

