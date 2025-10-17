#include "MeshTriangulator.h"
#include <pxr/usd/usdGeom/primvarsAPI.h>
#include <pxr/usd/usdGeom/tokens.h>
#include <pxr/base/tf/token.h>
#include <iostream>
#include <algorithm>

PXR_NAMESPACE_USING_DIRECTIVE

namespace workbench
{
    namespace optimizer
    {

        MeshTriangulator::MeshTriangulator(const TriangulationOptions &options)
            : m_options(options)
        {
        }

        bool MeshTriangulator::triangulateStage(UsdStagePtr stage)
        {
            if (!stage)
            {
                std::cerr << "Error: Invalid stage provided to triangulateStage" << std::endl;
                return false;
            }

            resetStats();
            logVerbose("Starting triangulation of USD stage");

            // Traverse all mesh primitives in the stage
            UsdPrimRange range = stage->Traverse();
            bool success = true;

            for (UsdPrimRange::iterator it = range.begin(); it != range.end(); ++it)
            {
                UsdPrim prim = *it;

                // Check if this primitive is a mesh
                if (prim.IsA<UsdGeomMesh>())
                {
                    UsdGeomMesh mesh(prim);
                    logVerbose("Processing mesh: " + prim.GetPath().GetString());

                    if (!triangulateMesh(mesh))
                    {
                        std::cerr << "Warning: Failed to triangulate mesh: "
                                  << prim.GetPath().GetString() << std::endl;
                        success = false;
                    }
                    else
                    {
                        m_stats.meshesProcessed++;
                    }
                }
            }

            logVerbose("Triangulation complete. Processed " +
                       std::to_string(m_stats.meshesProcessed) + " meshes");

            return success;
        }

        bool MeshTriangulator::triangulateMesh(UsdGeomMesh &mesh)
        {
            return triangulateMesh(mesh, UsdTimeCode::Default());
        }

        bool MeshTriangulator::triangulateMesh(UsdGeomMesh &mesh, UsdTimeCode timeCode)
        {
            if (!mesh)
            {
                std::cerr << "Error: Invalid mesh provided to triangulateMesh" << std::endl;
                return false;
            }

            // Get face vertex counts and indices
            VtIntArray faceVertexCounts;
            VtIntArray faceVertexIndices;

            UsdAttribute faceCountsAttr = mesh.GetFaceVertexCountsAttr();
            UsdAttribute faceIndicesAttr = mesh.GetFaceVertexIndicesAttr();

            if (!faceCountsAttr.Get(&faceVertexCounts, timeCode))
            {
                std::cerr << "Error: Failed to get face vertex counts" << std::endl;
                return false;
            }

            if (!faceIndicesAttr.Get(&faceVertexIndices, timeCode))
            {
                std::cerr << "Error: Failed to get face vertex indices" << std::endl;
                return false;
            }

            // Check if triangulation is needed
            bool needsTriangulation = false;
            for (int count : faceVertexCounts)
            {
                if (count > 3)
                {
                    needsTriangulation = true;
                    break;
                }
            }

            if (!needsTriangulation)
            {
                logVerbose("Mesh is already triangulated, skipping");
                return true;
            }

            // Store original statistics
            m_stats.originalFaceCount += faceVertexCounts.size();

            // Triangulate the faces
            VtIntArray triangulatedCounts;
            VtIntArray triangulatedIndices;

            if (!triangulateFaces(faceVertexCounts, faceVertexIndices,
                                  triangulatedCounts, triangulatedIndices))
            {
                std::cerr << "Error: Failed to triangulate faces" << std::endl;
                return false;
            }

            // Update statistics
            m_stats.finalFaceCount += triangulatedCounts.size();

            // Count how many faces were actually triangulated
            for (int count : faceVertexCounts)
            {
                if (count > 3)
                {
                    m_stats.facesTriangulated++;
                }
            }

            // Triangulate primvars if requested
            if (m_options.preserveOriginalPrimvars && !triangulatePrimvars(mesh, faceVertexCounts, timeCode))
            {
                std::cerr << "Warning: Failed to triangulate primvars" << std::endl;
                // Don't fail the entire operation for primvar issues
            }

            // Set the new face data
            faceCountsAttr.Set(triangulatedCounts, timeCode);
            faceIndicesAttr.Set(triangulatedIndices, timeCode);

            logVerbose("Successfully triangulated mesh with " +
                       std::to_string(faceVertexCounts.size()) + " original faces to " +
                       std::to_string(triangulatedCounts.size()) + " triangular faces");

            return true;
        }

        bool MeshTriangulator::triangulateFaces(const VtIntArray &faceVertexCounts, const VtIntArray &faceVertexIndices, VtIntArray &triangulatedCounts, VtIntArray &triangulatedIndices)
        {

            triangulatedCounts.clear();
            triangulatedIndices.clear();

            int indexOffset = 0;

            for (int faceVertexCount : faceVertexCounts)
            {
                if (faceVertexCount < 3)
                {
                    std::cerr << "Warning: Skipping degenerate face with " << faceVertexCount << " vertices" << std::endl;
                }
                else if (faceVertexCount == 3)
                {
                    // Already a triangle, copy as-is
                    triangulatedCounts.push_back(3);
                    for (int i = 0; i < 3; ++i)
                    {
                        triangulatedIndices.push_back(faceVertexIndices[indexOffset + i]);
                    }
                }
                else
                {
                    // Triangulate using fan triangulation
                    triangulateFace(faceVertexCount, indexOffset, faceVertexIndices, triangulatedIndices);

                    // Add triangle counts (each n-gon becomes n-2 triangles)
                    int numTriangles = faceVertexCount - 2;
                    for (int i = 0; i < numTriangles; ++i)
                    {
                        triangulatedCounts.push_back(3);
                    }
                }

                indexOffset += faceVertexCount;
            }

            return true;
        }

        void MeshTriangulator::triangulateFace(int faceVertexCount, int startIndex, const VtIntArray &faceVertexIndices, VtIntArray &triangulatedIndices)
        {

            // Use fan triangulation: connect all vertices to the first vertex
            // For a face with vertices [v0, v1, v2, v3, v4], create triangles:
            // [v0, v1, v2], [v0, v2, v3], [v0, v3, v4]

            int firstVertex = faceVertexIndices[startIndex];

            for (int i = 1; i < faceVertexCount - 1; ++i)
            {
                triangulatedIndices.push_back(firstVertex);
                triangulatedIndices.push_back(faceVertexIndices[startIndex + i]);
                triangulatedIndices.push_back(faceVertexIndices[startIndex + i + 1]);
            }
        }

        bool MeshTriangulator::triangulatePrimvars(UsdGeomMesh &mesh, const VtIntArray &originalFaceCounts, UsdTimeCode timeCode)
        {

            UsdGeomPrimvarsAPI primvarsAPI(mesh.GetPrim());
            std::vector<UsdGeomPrimvar> primvars = primvarsAPI.GetPrimvars();

            bool success = true;

            for (UsdGeomPrimvar &primvar : primvars)
            {
                // Only triangulate face-varying primvars
                if (primvar.GetInterpolation() != UsdGeomTokens->faceVarying)
                {
                    continue;
                }

                VtValue value;
                if (!primvar.Get(&value, timeCode))
                {
                    std::cerr << "Warning: Failed to get primvar value for "
                              << primvar.GetPrimvarName() << std::endl;
                    success = false;
                    continue;
                }

                // Get indices if they exist
                VtIntArray indices;
                bool hasIndices = primvar.GetIndices(&indices, timeCode);

                // TODO: For now, we'll use a simple approach: duplicate face-varying data
                // for triangulated faces. A more sophisticated approach would
                // interpolate values at triangle vertices.

                if (hasIndices)
                {
                    VtIntArray newIndices;
                    int indexOffset = 0;

                    for (int faceVertexCount : originalFaceCounts)
                    {
                        if (faceVertexCount < 3)
                        {
                            std::cerr << "Warning: Skipping degenerate face with " << faceVertexCount << " vertices in primvar triangulation" << std::endl;
                        }
                        else if (faceVertexCount == 3)
                        {
                            // Copy triangle indices as-is
                            for (int i = 0; i < 3; ++i)
                            {
                                newIndices.push_back(indices[indexOffset + i]);
                            }
                        }
                        else
                        {
                            // Fan triangulation for indices
                            int firstIndex = indices[indexOffset];
                            for (int i = 1; i < faceVertexCount - 1; ++i)
                            {
                                newIndices.push_back(firstIndex);
                                newIndices.push_back(indices[indexOffset + i]);
                                newIndices.push_back(indices[indexOffset + i + 1]);
                            }
                        }
                        indexOffset += faceVertexCount;
                    }

                    primvar.SetIndices(newIndices, timeCode);
                }

                // TODO: If no indices, the primvar data itself might need expansion
                // This is a complex case that would require type-specific handling
            }

            return success;
        }

        void MeshTriangulator::logVerbose(const std::string &message) const
        {
            if (m_options.verbose)
            {
                std::cout << "[MeshTriangulator] " << message << std::endl;
            }
        }

    } // namespace optimizer
} // namespace workbench