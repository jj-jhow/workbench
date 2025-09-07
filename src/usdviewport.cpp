#include "usdviewport.h"
#include <QOpenGLFunctions>
#include <QPainter>

UsdViewport::UsdViewport(QWidget* parent) : QOpenGLWidget(parent) {}

void UsdViewport::setStage(pxr::UsdStageRefPtr stage) {
    currentStage = stage;
    update();
}

void UsdViewport::initializeGL() {
    // Placeholder: setup OpenGL state if needed
    QOpenGLFunctions *f = context()->functions();
    f->glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void UsdViewport::paintGL() {
    QOpenGLFunctions *f = context()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Placeholder: draw USD stage using Hydra/GL or a simple message
    QPainter painter(this);
    painter.setPen(Qt::white);
    painter.drawText(rect(), Qt::AlignCenter, currentStage ? "USD Stage Loaded\n(Rendering not implemented)" : "No Stage Loaded");
}

void UsdViewport::resizeGL(int w, int h) {
    // Placeholder: handle viewport resize if needed
}
