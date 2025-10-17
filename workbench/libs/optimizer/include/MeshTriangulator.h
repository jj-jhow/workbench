#pragma once

#include <pxr/pxr.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/base/vt/array.h>
#include <pxr/base/gf/vec3f.h>
#include <vector>

PXR_NAMESPACE_USING_DIRECTIVE

namespace workbench
{
    namespace optimizer
    {

        /**
         * @brief A utility class for triangulating meshes in USD stages
         *
         * This class provides functionality to convert n-gon faces in USD mesh
         * primitives to triangular faces, which is useful for rendering engines
         * that require triangulated geometry.
         */
        class MeshTriangulator
        {
        public:
            /**
             * @brief Options for controlling triangulation behavior
             */
            struct TriangulationOptions
            {
                bool preserveOriginalPrimvars = true; ///< Whether to preserve original primvar data
                bool inPlace = false;                 ///< Whether to modify meshes in-place or create new ones
                bool verbose = false;                 ///< Enable verbose logging

                TriangulationOptions() = default;
            };

            /**
             * @brief Statistics about the triangulation process
             */
            struct TriangulationStats
            {
                size_t meshesProcessed = 0;
                size_t facesTriangulated = 0;
                size_t originalFaceCount = 0;
                size_t finalFaceCount = 0;

                void reset()
                {
                    meshesProcessed = 0;
                    facesTriangulated = 0;
                    originalFaceCount = 0;
                    finalFaceCount = 0;
                }
            };

            /**
             * @brief Default constructor
             */
            MeshTriangulator() = default;

            /**
             * @brief Constructor with options
             * @param options Triangulation options
             */
            explicit MeshTriangulator(const TriangulationOptions &options);

            /**
             * @brief Triangulate all meshes in a USD stage
             * @param stage The USD stage containing meshes to triangulate
             * @return True if triangulation was successful, false otherwise
             */
            bool triangulateStage(UsdStagePtr stage);

            /**
             * @brief Triangulate a specific mesh primitive
             * @param mesh The USD mesh primitive to triangulate
             * @return True if triangulation was successful, false otherwise
             */
            bool triangulateMesh(UsdGeomMesh &mesh);

            /**
             * @brief Triangulate a mesh at a specific time sample
             * @param mesh The USD mesh primitive to triangulate
             * @param timeCode The time code for the sample
             * @return True if triangulation was successful, false otherwise
             */
            bool triangulateMesh(UsdGeomMesh &mesh, UsdTimeCode timeCode);

            /**
             * @brief Get triangulation statistics
             * @return Reference to the current statistics
             */
            const TriangulationStats &getStats() const { return m_stats; }

            /**
             * @brief Reset triangulation statistics
             */
            void resetStats() { m_stats.reset(); }

            /**
             * @brief Set triangulation options
             * @param options New options to use
             */
            void setOptions(const TriangulationOptions &options) { m_options = options; }

            /**
             * @brief Get current triangulation options
             * @return Reference to current options
             */
            const TriangulationOptions &getOptions() const { return m_options; }

        private:
            /**
             * @brief Triangulate face vertex counts and indices
             * @param faceVertexCounts Input face vertex counts
             * @param faceVertexIndices Input face vertex indices
             * @param triangulatedCounts Output triangulated face counts (all 3s)
             * @param triangulatedIndices Output triangulated face indices
             * @return True if triangulation was successful
             */
            bool triangulateFaces(const VtIntArray &faceVertexCounts, const VtIntArray &faceVertexIndices, VtIntArray &triangulatedCounts, VtIntArray &triangulatedIndices);

            /**
             * @brief Triangulate primvar data to match new face topology
             * @param mesh The mesh being triangulated
             * @param originalFaceCounts Original face vertex counts
             * @param timeCode Time code for the sample
             * @return True if primvar triangulation was successful
             */
            bool triangulatePrimvars(UsdGeomMesh &mesh, const VtIntArray &originalFaceCounts, UsdTimeCode timeCode);

            /**
             * @brief Triangulate a single face using fan triangulation
             * @param faceVertexCount Number of vertices in the face
             * @param startIndex Starting index in the vertex index array
             * @param faceVertexIndices Input face vertex indices
             * @param triangulatedIndices Output triangulated indices (appended to)
             */
            void triangulateFace(int faceVertexCount, int startIndex, const VtIntArray &faceVertexIndices, VtIntArray &triangulatedIndices);

            /**
             * @brief Log a message if verbose mode is enabled
             * @param message Message to log
             */
            void logVerbose(const std::string &message) const;

        private:
            TriangulationOptions m_options;
            TriangulationStats m_stats;
        };

    } // namespace optimizer
} // namespace workbench