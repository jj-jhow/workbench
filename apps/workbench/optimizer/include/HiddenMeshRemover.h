#pragma once

#include <pxr/pxr.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/camera.h>
#include <pxr/usd/usdGeom/boundable.h>
#include <pxr/base/vt/array.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/gf/vec3d.h>
#include <pxr/base/gf/matrix4d.h>
#include <pxr/base/gf/bbox3d.h>
#include <pxr/base/gf/ray.h>
#include <pxr/base/gf/range3d.h>
#include <vector>
#include <unordered_set>

PXR_NAMESPACE_USING_DIRECTIVE

namespace workbench
{
    namespace optimizer
    {

        /**
         * @brief A utility class for removing hidden meshes from USD stages
         *
         * This class provides functionality to identify and remove mesh primitives
         * that are not visible from any reasonable viewpoint, helping to reduce
         * file size and improve performance.
         */
        class HiddenMeshRemover
        {
        public:
            /**
             * @brief Options for controlling hidden mesh removal behavior
             */
            struct RemovalOptions
            {
                bool useExistingCameras = true;      ///< Use cameras defined in the scene
                bool generateViewpoints = true;      ///< Generate additional viewpoints around the scene
                float viewpointDensity = 8.0f;       ///< Number of viewpoints per axis when generating
                bool conservativeRemoval = true;     ///< Be conservative - only remove obviously hidden meshes
                bool considerTransparency = true;    ///< Consider transparent materials when determining visibility
                bool preserveInstancedMeshes = true; ///< Don't remove meshes that are instanced multiple times
                bool verbose = false;                ///< Enable verbose logging
                float occlusionThreshold = 0.95f;    ///< Fraction of mesh that must be occluded to consider it hidden

                RemovalOptions() = default;
            };

            /**
             * @brief Statistics about the hidden mesh removal process
             */
            struct RemovalStats
            {
                size_t totalMeshes = 0;
                size_t hiddenMeshes = 0;
                size_t removedMeshes = 0;
                size_t preservedMeshes = 0;
                size_t viewpointsGenerated = 0;
                size_t viewpointsUsed = 0;
                float spaceSavedPercent = 0.0f;

                void reset()
                {
                    totalMeshes = 0;
                    hiddenMeshes = 0;
                    removedMeshes = 0;
                    preservedMeshes = 0;
                    viewpointsGenerated = 0;
                    viewpointsUsed = 0;
                    spaceSavedPercent = 0.0f;
                }
            };

            /**
             * @brief Represents a viewpoint for visibility testing
             */
            struct Viewpoint
            {
                GfVec3d position;
                GfVec3d direction;
                float fov = 60.0f; ///< Field of view in degrees

                Viewpoint(const GfVec3d &pos, const GfVec3d &dir, float fieldOfView = 60.0f)
                    : position(pos), direction(dir), fov(fieldOfView) {}
            };

            /**
             * @brief Default constructor
             */
            HiddenMeshRemover() = default;

            /**
             * @brief Constructor with options
             * @param options Hidden mesh removal options
             */
            explicit HiddenMeshRemover(const RemovalOptions &options);

            /**
             * @brief Remove hidden meshes from a USD stage
             * @param stage The USD stage containing meshes to analyze
             * @return True if removal was successful, false otherwise
             */
            bool removeHiddenMeshes(UsdStagePtr stage);

            /**
             * @brief Analyze visibility without removing meshes (dry run)
             * @param stage The USD stage containing meshes to analyze
             * @return Vector of mesh paths that would be removed
             */
            std::vector<SdfPath> analyzeHiddenMeshes(UsdStagePtr stage);

            /**
             * @brief Get removal statistics
             * @return Reference to the current statistics
             */
            const RemovalStats &getStats() const { return m_stats; }

            /**
             * @brief Reset removal statistics
             */
            void resetStats() { m_stats.reset(); }

            /**
             * @brief Set removal options
             * @param options New options to use
             */
            void setOptions(const RemovalOptions &options) { m_options = options; }

            /**
             * @brief Get current removal options
             * @return Reference to current options
             */
            const RemovalOptions &getOptions() const { return m_options; }

        private:
            /**
             * @brief Generate viewpoints around the scene bounding box
             * @param sceneBounds The bounding box of the entire scene
             * @return Vector of generated viewpoints
             */
            std::vector<Viewpoint> generateViewpoints(const GfBBox3d &sceneBounds);

            /**
             * @brief Extract camera viewpoints from the stage
             * @param stage The USD stage to search for cameras
             * @return Vector of camera viewpoints
             */
            std::vector<Viewpoint> extractCameraViewpoints(UsdStagePtr stage);

            /**
             * @brief Test if a mesh is visible from any viewpoint
             * @param mesh The mesh to test
             * @param viewpoints The viewpoints to test from
             * @param allMeshes All meshes in the scene (for occlusion testing)
             * @return True if the mesh is visible from at least one viewpoint
             */
            bool isMeshVisible(const UsdGeomMesh &mesh,
                               const std::vector<Viewpoint> &viewpoints,
                               const std::vector<UsdGeomMesh> &allMeshes);

            /**
             * @brief Test if a mesh is visible from a specific viewpoint
             * @param mesh The mesh to test
             * @param viewpoint The viewpoint to test from
             * @param occludingMeshes Other meshes that might occlude this one
             * @return Fraction of mesh that is visible (0.0 = completely hidden, 1.0 = fully visible)
             */
            float testMeshVisibilityFromViewpoint(const UsdGeomMesh &mesh,
                                                  const Viewpoint &viewpoint,
                                                  const std::vector<UsdGeomMesh> &occludingMeshes);

            /**
             * @brief Check if a mesh is within the camera frustum
             * @param meshBounds The bounding box of the mesh
             * @param viewpoint The viewpoint to test from
             * @return True if the mesh is within the frustum
             */
            bool isMeshInFrustum(const GfBBox3d &meshBounds, const Viewpoint &viewpoint);

            /**
             * @brief Perform ray-mesh intersection test
             * @param ray The ray to test
             * @param mesh The mesh to test against
             * @return True if the ray intersects the mesh
             */
            bool rayMeshIntersection(const GfRay &ray, const UsdGeomMesh &mesh);

            /**
             * @brief Get the bounding box of a mesh
             * @param mesh The mesh to get bounds for
             * @return The bounding box of the mesh
             */
            GfBBox3d getMeshBounds(const UsdGeomMesh &mesh);

            /**
             * @brief Calculate the scene bounding box
             * @param stage The USD stage
             * @return The bounding box containing all geometry
             */
            GfBBox3d calculateSceneBounds(UsdStagePtr stage);

            /**
             * @brief Check if a mesh is instanced (referenced multiple times)
             * @param mesh The mesh to check
             * @param stage The USD stage
             * @return True if the mesh is instanced
             */
            bool isMeshInstanced(const UsdGeomMesh &mesh, UsdStagePtr stage);

            /**
             * @brief Log a message if verbose mode is enabled
             * @param message Message to log
             */
            void logVerbose(const std::string &message) const;

            /**
             * @brief Sample points on the mesh surface for visibility testing
             * @param mesh The mesh to sample
             * @param numSamples Number of sample points to generate
             * @return Vector of sample points on the mesh surface
             */
            std::vector<GfVec3d> sampleMeshSurface(const UsdGeomMesh &mesh, size_t numSamples = 16);

        private:
            RemovalOptions m_options;
            RemovalStats m_stats;
        };

    } // namespace optimizer
} // namespace workbench