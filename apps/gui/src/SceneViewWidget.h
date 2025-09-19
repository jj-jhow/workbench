#ifndef SCENEVIEWWIDGET_H
#define SCENEVIEWWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include "render/HydraRenderer.h"
#include <pxr/usd/usd/stage.h>

class SceneViewWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit SceneViewWidget(QWidget *parent = nullptr);
    ~SceneViewWidget() override;

    void setStage(const pxr::UsdStageRefPtr &stage);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    pxr::HydraRenderer* _renderer;
    pxr::UsdStageRefPtr _stage;
};

#endif // SCENEVIEWWIDGET_H
