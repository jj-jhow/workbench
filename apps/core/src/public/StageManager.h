#pragma once
#include <pxr/usd/usd/stage.h>
#include <memory>
#include <string>

class StageManager
{
public:
    // Loads a USD stage from file. Returns true on success.
    bool LoadStage(const std::string &path);

    // Saves the current stage to file. Returns true on success.
    bool SaveStage(const std::string &path = "") const;

    // Returns a shared pointer to the current stage.
    pxr::UsdStageRefPtr GetStage() const;

    // Returns true if a stage is loaded.
    bool HasStage() const;

    // Clears the current stage.
    void ClearStage();

private:
    pxr::UsdStageRefPtr stage_;
    std::string loadedPath_;
};
