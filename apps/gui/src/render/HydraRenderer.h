#ifndef WORKBENCH_HYDRARENDERER_H
#define WORKBENCH_HYDRARENDERER_H

#include <pxr/usd/usd/stage.h>
#include <pxr/usdImaging/usdImagingGL/engine.h>
#include <pxr/imaging/hdx/taskController.h>
#include <pxr/imaging/glf/simpleLightingContext.h>
#include <pxr/base/gf/matrix4d.h>

PXR_NAMESPACE_OPEN_SCOPE

class HydraRenderer : public UsdImagingGLEngine
{
public:
    HydraRenderer();
    ~HydraRenderer();

    void Initialize();
    void Render(const UsdPrim &prim, const UsdImagingGLRenderParams &params); // Updated signature
    void SetCamera(const GfMatrix4d &viewMatrix, const GfMatrix4d &projectionMatrix);
    void SetRenderViewport(int x, int y, int width, int height);

private:
    HdxTaskController *_taskController;
    GlfSimpleLightingContextRefPtr _lightingContext;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif // WORKBENCH_HYDRARENDERER_H
