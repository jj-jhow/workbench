#include "StageManager.h"
#include <pxr/usd/usd/stage.h>
#include <pxr/base/tf/stringUtils.h>
#include <iostream>

bool StageManager::LoadStage(const std::string &path)
{
    stage_ = pxr::UsdStage::Open(path);
    if (!stage_)
    {
        std::cerr << "Failed to open USD stage: " << path << std::endl;
        return false;
    }
    loadedPath_ = path;
    return true;
}

bool StageManager::SaveStage(const std::string &path)
{
    if (!stage_)
        return false;
    std::string savePath = path.empty() ? loadedPath_ : path;
    if (savePath.empty())
        return false;
    stage_->GetRootLayer()->Export(savePath);
    return true;
}

pxr::UsdStageRefPtr StageManager::GetStage() const
{
    return stage_;
}

bool StageManager::HasStage() const
{
    return static_cast<bool>(stage_);
}
