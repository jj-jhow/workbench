#pragma once
#include <QOpenGLWidget>
#include <pxr/usd/usd/stage.h>

class UsdViewport : public QOpenGLWidget {
    Q_OBJECT
public:
    explicit UsdViewport(QWidget* parent = nullptr);
    void setStage(pxr::UsdStageRefPtr stage);
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
private:
    pxr::UsdStageRefPtr currentStage;
};
