#include "SceneViewWidget.h"
#include <pxr/usd/usd/prim.h>

SceneViewWidget::SceneViewWidget(QWidget *parent)
    : QOpenGLWidget(parent), _renderer(nullptr)
{
}

SceneViewWidget::~SceneViewWidget()
{
    delete _renderer;
}

void SceneViewWidget::setStage(const pxr::UsdStageRefPtr &stage)
{
    _stage = stage;
    update();
}

void SceneViewWidget::initializeGL()
{
    initializeOpenGLFunctions();
    _renderer = new pxr::HydraRenderer();
    _renderer->Initialize();
}

void SceneViewWidget::resizeGL(int w, int h)
{
    if (_renderer) _renderer->SetRenderViewport(0, 0, w, h);
}

void SceneViewWidget::paintGL()
{
    if (_stage && _renderer)
    {
        pxr::UsdImagingGLRenderParams params;
        // Set any desired render params here (e.g., draw mode, complexity, etc.)
        _renderer->Render(_stage->GetPseudoRoot(), params);
    }
}
