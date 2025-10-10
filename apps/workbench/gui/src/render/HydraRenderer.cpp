#include "HydraRenderer.h"
#include <pxr/imaging/garch/glApi.h>

PXR_NAMESPACE_OPEN_SCOPE

HydraRenderer::HydraRenderer() : _taskController(nullptr)
{
}

HydraRenderer::~HydraRenderer()
{
    delete _taskController;
}

void HydraRenderer::Initialize()
{
    _taskController = new HdxTaskController(_GetRenderIndex(), SdfPath::AbsoluteRootPath());
    _lightingContext = GlfSimpleLightingContext::New();
    SetLightingState(_lightingContext);
}

void HydraRenderer::Render(const UsdPrim &prim, const UsdImagingGLRenderParams &params)
{
    // Set camera state using SetFreeCameraMatrices if needed (not GetCameraMatrices)
    // Camera matrices should be managed externally and set via SetCamera
    UsdImagingGLEngine::Render(prim, params);
}

void HydraRenderer::SetCamera(const GfMatrix4d &viewMatrix, const GfMatrix4d &projectionMatrix)
{
    if (_taskController) {
        _taskController->SetFreeCameraMatrices(viewMatrix, projectionMatrix);
    }
}

void HydraRenderer::SetRenderViewport(int x, int y, int width, int height)
{
    GfVec4d viewport(x, y, width, height);
    _taskController->SetRenderViewport(viewport);
}

PXR_NAMESPACE_CLOSE_SCOPE
