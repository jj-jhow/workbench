#include "HiddenMeshRemover.h"
#include <pxr/usd/usdGeom/tokens.h>
#include <pxr/usd/usdGeom/xformable.h>
#include <pxr/usd/usdGeom/scope.h>
#include <pxr/usd/usdGeom/imageable.h>
#include <pxr/base/gf/frustum.h>
#include <pxr/base/gf/plane.h>
#include <iostream>
#include <algorithm>
#include <cmath>

PXR_NAMESPACE_USING_DIRECTIVE

namespace workbench
{
    namespace optimizer
    {

        HiddenMeshRemover::HiddenMeshRemover(const RemovalOptions &options)
            : m_options(options)
        {
        }

        bool HiddenMeshRemover::removeHiddenMeshes(UsdStagePtr stage)
        {
            if (!stage)
            {
                logVerbose("Invalid stage provided");
                return false;
            }

            m_stats.reset();
            logVerbose("Starting hidden mesh removal analysis...");

            // Calculate scene bounds
            GfBBox3d sceneBounds = calculateSceneBounds(stage);
            logVerbose("Scene bounds calculated: " +
                       std::to_string(sceneBounds.GetRange().GetSize().GetLength()) + " units");

            // Generate viewpoints
            std::vector<Viewpoint> viewpoints;

            if (m_options.useExistingCameras)
            {
                auto cameraViewpoints = extractCameraViewpoints(stage);
                viewpoints.insert(viewpoints.end(), cameraViewpoints.begin(), cameraViewpoints.end());
                logVerbose("Found " + std::to_string(cameraViewpoints.size()) + " camera viewpoints");
            }

            if (m_options.generateViewpoints)
            {
                auto generatedViewpoints = generateViewpoints(sceneBounds);
                viewpoints.insert(viewpoints.end(), generatedViewpoints.begin(), generatedViewpoints.end());
                m_stats.viewpointsGenerated = generatedViewpoints.size();
                logVerbose("Generated " + std::to_string(generatedViewpoints.size()) + " additional viewpoints");
            }

            m_stats.viewpointsUsed = viewpoints.size();

            if (viewpoints.empty())
            {
                logVerbose("No viewpoints available for analysis");
                return false;
            }

            // Collect all meshes in the stage
            std::vector<UsdGeomMesh> allMeshes;
            auto range = stage->Traverse();
            for (auto it = range.begin(); it != range.end(); ++it)
            {
                if (it->IsA<UsdGeomMesh>())
                {
                    UsdGeomMesh mesh(*it);
                    allMeshes.push_back(mesh);
                }
            }

            m_stats.totalMeshes = allMeshes.size();
            logVerbose("Found " + std::to_string(allMeshes.size()) + " meshes to analyze");

            // Analyze each mesh for visibility
            std::vector<SdfPath> meshesToRemove;
            for (const auto &mesh : allMeshes)
            {
                // Skip if this is an instanced mesh and we want to preserve them
                if (m_options.preserveInstancedMeshes && isMeshInstanced(mesh, stage))
                {
                    m_stats.preservedMeshes++;
                    logVerbose("Preserving instanced mesh: " + mesh.GetPath().GetString());
                    continue;
                }

                // Test visibility
                if (!isMeshVisible(mesh, viewpoints, allMeshes))
                {
                    meshesToRemove.push_back(mesh.GetPath());
                    m_stats.hiddenMeshes++;
                    logVerbose("Mesh marked for removal: " + mesh.GetPath().GetString());
                }
            }

            // Create visibility overrides for hidden meshes (non-destructive approach)
            for (const auto &path : meshesToRemove)
            {
                auto prim = stage->GetPrimAtPath(path);
                if (prim && prim.IsValid())
                {
                    // Set visibility to invisible using USD's proper visibility attribute
                    UsdGeomImageable imageable(prim);
                    if (imageable)
                    {
                        imageable.CreateVisibilityAttr().Set(UsdGeomTokens->invisible);
                        m_stats.removedMeshes++;
                        logVerbose("Set visibility=invisible for mesh: " + path.GetString());
                    }
                    else
                    {
                        logVerbose("Could not create imageable for mesh: " + path.GetString());
                    }
                }
            }

            // Calculate space saved percentage
            if (m_stats.totalMeshes > 0)
            {
                m_stats.spaceSavedPercent = (float(m_stats.removedMeshes) / float(m_stats.totalMeshes)) * 100.0f;
            }

            logVerbose("Hidden mesh removal completed. Set visibility=invisible for " +
                       std::to_string(m_stats.removedMeshes) + " of " +
                       std::to_string(m_stats.totalMeshes) + " meshes (" +
                       std::to_string(m_stats.spaceSavedPercent) + "% visibility reduction)");

            return true;
        }

        std::vector<SdfPath> HiddenMeshRemover::analyzeHiddenMeshes(UsdStagePtr stage)
        {
            std::vector<SdfPath> hiddenMeshes;

            if (!stage)
            {
                return hiddenMeshes;
            }

            m_stats.reset();

            // Calculate scene bounds and generate viewpoints
            GfBBox3d sceneBounds = calculateSceneBounds(stage);
            std::vector<Viewpoint> viewpoints;

            if (m_options.useExistingCameras)
            {
                auto cameraViewpoints = extractCameraViewpoints(stage);
                viewpoints.insert(viewpoints.end(), cameraViewpoints.begin(), cameraViewpoints.end());
            }

            if (m_options.generateViewpoints)
            {
                auto generatedViewpoints = generateViewpoints(sceneBounds);
                viewpoints.insert(viewpoints.end(), generatedViewpoints.begin(), generatedViewpoints.end());
            }

            if (viewpoints.empty())
            {
                return hiddenMeshes;
            }

            // Collect all meshes
            std::vector<UsdGeomMesh> allMeshes;
            auto range = stage->Traverse();
            for (auto it = range.begin(); it != range.end(); ++it)
            {
                if (it->IsA<UsdGeomMesh>())
                {
                    UsdGeomMesh mesh(*it);
                    allMeshes.push_back(mesh);
                }
            }

            // Analyze visibility
            for (const auto &mesh : allMeshes)
            {
                if (m_options.preserveInstancedMeshes && isMeshInstanced(mesh, stage))
                {
                    continue;
                }

                if (!isMeshVisible(mesh, viewpoints, allMeshes))
                {
                    hiddenMeshes.push_back(mesh.GetPath());
                }
            }

            return hiddenMeshes;
        }

        std::vector<HiddenMeshRemover::Viewpoint> HiddenMeshRemover::generateViewpoints(const GfBBox3d &sceneBounds)
        {
            std::vector<Viewpoint> viewpoints;

            GfVec3d center = sceneBounds.GetRange().GetMidpoint();
            GfVec3d size = sceneBounds.GetRange().GetSize();
            double maxDim = std::max({size[0], size[1], size[2]});
            double radius = maxDim * 1.5; // Distance from center to place viewpoints

            int numViewpoints = static_cast<int>(m_options.viewpointDensity);

            // Generate viewpoints in a sphere around the scene
            for (int i = 0; i < numViewpoints; ++i)
            {
                for (int j = 0; j < numViewpoints; ++j)
                {
                    double theta = (2.0 * M_PI * i) / numViewpoints; // Azimuth
                    double phi = (M_PI * j) / (numViewpoints - 1);   // Elevation

                    // Convert spherical to cartesian coordinates
                    GfVec3d position(
                        center[0] + radius * sin(phi) * cos(theta),
                        center[1] + radius * sin(phi) * sin(theta),
                        center[2] + radius * cos(phi));

                    // Direction points towards scene center
                    GfVec3d direction = (center - position).GetNormalized();

                    viewpoints.emplace_back(position, direction, 60.0f);
                }
            }

            // Add some additional viewpoints along the main axes
            double axisDistance = radius * 0.8;
            std::vector<GfVec3d> axisPositions = {
                center + GfVec3d(axisDistance, 0, 0),  // +X
                center + GfVec3d(-axisDistance, 0, 0), // -X
                center + GfVec3d(0, axisDistance, 0),  // +Y
                center + GfVec3d(0, -axisDistance, 0), // -Y
                center + GfVec3d(0, 0, axisDistance),  // +Z
                center + GfVec3d(0, 0, -axisDistance)  // -Z
            };

            for (const auto &pos : axisPositions)
            {
                GfVec3d direction = (center - pos).GetNormalized();
                viewpoints.emplace_back(pos, direction, 90.0f); // Wider FOV for axis views
            }

            return viewpoints;
        }

        std::vector<HiddenMeshRemover::Viewpoint> HiddenMeshRemover::extractCameraViewpoints(UsdStagePtr stage)
        {
            std::vector<Viewpoint> viewpoints;

            auto range = stage->Traverse();
            for (auto it = range.begin(); it != range.end(); ++it)
            {
                if (it->IsA<UsdGeomCamera>())
                {
                    UsdGeomCamera camera(*it);

                    // Get camera transform
                    UsdGeomXformable xformable(camera);
                    GfMatrix4d transform;
                    bool resetsXformStack;
                    xformable.GetLocalTransformation(&transform, &resetsXformStack);

                    // Extract position and direction from transform
                    GfVec3d position = transform.ExtractTranslation();

                    // Camera looks down -Z axis in its local space
                    GfVec3d localDirection(0, 0, -1);
                    GfVec3d worldDirection = transform.TransformDir(localDirection);

                    // Get field of view
                    float fov = 60.0f; // Default
                    float horizontalAperture, verticalAperture, focalLength;
                    if (camera.GetHorizontalApertureAttr().Get(&horizontalAperture) &&
                        camera.GetVerticalApertureAttr().Get(&verticalAperture) &&
                        camera.GetFocalLengthAttr().Get(&focalLength))
                    {

                        // Calculate FOV from aperture and focal length
                        fov = 2.0f * atan(horizontalAperture / (2.0f * focalLength)) * 180.0f / M_PI;
                    }

                    viewpoints.emplace_back(position, worldDirection, fov);
                }
            }

            return viewpoints;
        }

        bool HiddenMeshRemover::isMeshVisible(const UsdGeomMesh &mesh, const std::vector<Viewpoint> &viewpoints, const std::vector<UsdGeomMesh> &allMeshes)
        {
            // Create a list of potential occluding meshes (all meshes except this one)
            std::vector<UsdGeomMesh> occludingMeshes;
            for (const auto &otherMesh : allMeshes)
            {
                if (otherMesh.GetPath() != mesh.GetPath())
                {
                    occludingMeshes.push_back(otherMesh);
                }
            }

            // Test visibility from each viewpoint
            for (const auto &viewpoint : viewpoints)
            {
                float visibilityFraction = testMeshVisibilityFromViewpoint(mesh, viewpoint, occludingMeshes);

                // If the mesh is sufficiently visible from this viewpoint, consider it visible
                if (visibilityFraction > (1.0f - m_options.occlusionThreshold))
                {
                    return true;
                }
            }

            return false; // Not visible from any viewpoint
        }

        float HiddenMeshRemover::testMeshVisibilityFromViewpoint(const UsdGeomMesh &mesh, const Viewpoint &viewpoint, const std::vector<UsdGeomMesh> &occludingMeshes)
        {
            // Get mesh bounds
            GfBBox3d meshBounds = getMeshBounds(mesh);

            // First check if mesh is within camera frustum
            if (!isMeshInFrustum(meshBounds, viewpoint))
            {
                return 0.0f; // Not in view
            }

            // Sample points on the mesh surface
            std::vector<GfVec3d> samplePoints = sampleMeshSurface(mesh, 16);

            int visibleSamples = 0;
            for (const auto &point : samplePoints)
            {
                // Create ray from viewpoint to sample point
                GfVec3d rayDirection = (point - viewpoint.position).GetNormalized();
                GfRay ray(viewpoint.position, rayDirection);

                bool occluded = false;

                // Test if ray is occluded by any other mesh
                for (const auto &occluder : occludingMeshes)
                {
                    if (rayMeshIntersection(ray, occluder))
                    {
                        // Check if intersection is closer than our target point
                        // This is a simplified check - in a full implementation you'd want
                        // to get the actual intersection distance
                        occluded = true;
                        break;
                    }
                }

                if (!occluded)
                {
                    visibleSamples++;
                }
            }

            return static_cast<float>(visibleSamples) / static_cast<float>(samplePoints.size());
        }

        bool HiddenMeshRemover::isMeshInFrustum(const GfBBox3d &meshBounds, const Viewpoint &viewpoint)
        {
            // This is a simplified frustum test
            // In a full implementation, you'd want to construct a proper view frustum

            GfVec3d meshCenter = meshBounds.GetRange().GetMidpoint();
            GfVec3d toMesh = (meshCenter - viewpoint.position).GetNormalized();

            // Check if mesh center is roughly in the viewing direction
            double dotProduct = GfDot(toMesh, viewpoint.direction);
            double fovRadians = viewpoint.fov * M_PI / 180.0;
            double cosHalfFov = cos(fovRadians * 0.5);

            return dotProduct > cosHalfFov;
        }

        bool HiddenMeshRemover::rayMeshIntersection(const GfRay &ray, const UsdGeomMesh &mesh)
        {
            // This is a simplified intersection test using bounding box
            // A full implementation would test against actual mesh triangles

            GfBBox3d meshBounds = getMeshBounds(mesh);
            GfRange3d range = meshBounds.GetRange();

            // Simple ray-box intersection test
            GfVec3d rayStart = ray.GetStartPoint();
            GfVec3d rayDir = ray.GetDirection();

            // Avoid division by zero
            GfVec3d invDir;
            invDir[0] = (std::abs(rayDir[0]) > 1e-9) ? 1.0 / rayDir[0] : 1e9;
            invDir[1] = (std::abs(rayDir[1]) > 1e-9) ? 1.0 / rayDir[1] : 1e9;
            invDir[2] = (std::abs(rayDir[2]) > 1e-9) ? 1.0 / rayDir[2] : 1e9;

            GfVec3d t1, t2;
            for (int i = 0; i < 3; ++i)
            {
                t1[i] = (range.GetMin()[i] - rayStart[i]) * invDir[i];
                t2[i] = (range.GetMax()[i] - rayStart[i]) * invDir[i];
            }

            GfVec3d tMin, tMax;
            for (int i = 0; i < 3; ++i)
            {
                tMin[i] = std::min(t1[i], t2[i]);
                tMax[i] = std::max(t1[i], t2[i]);
            }

            double tNear = std::max({tMin[0], tMin[1], tMin[2]});
            double tFar = std::min({tMax[0], tMax[1], tMax[2]});

            return tNear <= tFar && tFar > 0.0;
        }

        GfBBox3d HiddenMeshRemover::getMeshBounds(const UsdGeomMesh &mesh)
        {
            UsdGeomBoundable boundable(mesh);
            VtArray<GfVec3f> extent;

            if (boundable.GetExtentAttr().Get(&extent) && extent.size() == 2)
            {
                GfVec3d min_point(extent[0]);
                GfVec3d max_point(extent[1]);
                GfRange3d range(min_point, max_point);
                return GfBBox3d(range);
            }

            // Fallback: compute bounds from points
            VtArray<GfVec3f> points;
            if (mesh.GetPointsAttr().Get(&points))
            {
                GfRange3d range;
                for (const auto &point : points)
                {
                    range.UnionWith(GfVec3d(point));
                }
                return GfBBox3d(range);
            }

            // Last resort: return unit box
            return GfBBox3d(GfRange3d(GfVec3d(-1), GfVec3d(1)));
        }

        GfBBox3d HiddenMeshRemover::calculateSceneBounds(UsdStagePtr stage)
        {
            GfRange3d totalRange;

            auto range = stage->Traverse();
            for (auto it = range.begin(); it != range.end(); ++it)
            {
                if (it->IsA<UsdGeomBoundable>())
                {
                    UsdGeomBoundable boundable(*it);
                    GfBBox3d bounds = getMeshBounds(UsdGeomMesh(boundable));
                    totalRange.UnionWith(bounds.GetRange());
                }
            }

            if (totalRange.IsEmpty())
            {
                // Default scene bounds if no geometry found
                totalRange = GfRange3d(GfVec3d(-10), GfVec3d(10));
            }

            return GfBBox3d(totalRange);
        }

        bool HiddenMeshRemover::isMeshInstanced(const UsdGeomMesh &mesh, UsdStagePtr stage)
        {
            // This is a simplified check for instancing
            // In USD, instancing can be complex involving prototypes and instance proxies

            auto prim = mesh.GetPrim();

            // Check if this prim is an instance
            if (prim.IsInstance())
            {
                return true;
            }

            // Check if there are multiple references to the same mesh data
            // This would require analyzing the composition graph more thoroughly
            // For now, we'll use a simple heuristic based on the prim name/path

            return false; // Simplified implementation
        }

        std::vector<GfVec3d> HiddenMeshRemover::sampleMeshSurface(const UsdGeomMesh &mesh, size_t numSamples)
        {
            std::vector<GfVec3d> samples;

            VtArray<GfVec3f> points;
            if (!mesh.GetPointsAttr().Get(&points) || points.empty())
            {
                return samples;
            }

            VtArray<int> faceVertexCounts;
            VtArray<int> faceVertexIndices;

            if (!mesh.GetFaceVertexCountsAttr().Get(&faceVertexCounts) ||
                !mesh.GetFaceVertexIndicesAttr().Get(&faceVertexIndices))
            {
                return samples;
            }

            // Simple sampling: pick points from vertices
            // A more sophisticated approach would sample triangle surfaces
            size_t step = std::max(size_t(1), points.size() / numSamples);

            for (size_t i = 0; i < points.size(); i += step)
            {
                if (samples.size() >= numSamples)
                    break;
                samples.emplace_back(points[i]);
            }

            return samples;
        }

        void HiddenMeshRemover::logVerbose(const std::string &message) const
        {
            if (m_options.verbose)
            {
                std::cout << "[HiddenMeshRemover] " << message << std::endl;
            }
        }

    } // namespace optimizer
} // namespace workbench