#include "SceneViewWidget.h"
#include <QVBoxLayout>
#include <QLabel>

SceneViewWidget::SceneViewWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Scene View", this));
    setLayout(layout);
}

