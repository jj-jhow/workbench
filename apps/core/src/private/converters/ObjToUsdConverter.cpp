#include "converters/ObjToUsdConverter.h"

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/metrics.h>
#include <pxr/base/tf/stringUtils.h>
#include <pxr/base/tf/token.h>
#include <pxr/base/vt/array.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/usd/sdf/attributeSpec.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>

#include <iostream>

namespace converters
{

    bool ObjToUsdConverter::Convert(const fs::path &inputPath, const fs::path &outputPath, const ConverterOptions &options) const
    {
        std::cout << "Converting OBJ to USD: " << inputPath << " -> " << outputPath << std::endl;

        try
        {
            // Extract and transform
            auto stage = Extract(inputPath, outputPath);
            if (!stage)
            {
                std::cerr << "Failed to create USD stage: " << outputPath << std::endl;
                return false;
            }

            Transform(stage, options);

            // Save the stage
            stage->Save();

            std::cout << "Successfully created USD stage " << std::endl;
            return true;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error during conversion: " << e.what() << std::endl;
            return false;
        }
    }

    pxr::UsdStageRefPtr ObjToUsdConverter::Extract(const fs::path &inputPath, const fs::path &outputPath) const
    {
        std::cout << "Extracting data from: " << inputPath << " to " << outputPath << std::endl;

        // Using Assimp to read the OBJ file and extract the scene data before converting to USD
        const unsigned int importFlags = 0;
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(inputPath.string(), importFlags);
        if (!scene)
        {
            std::cerr << "Failed to extract data from OBJ file: " << inputPath << std::endl;
            return nullptr;
        }

        // Create a new USD stage so we can populate it with the extracted data
        auto stage = pxr::UsdStage::CreateNew(outputPath);
        if (!stage)
        {
            std::cerr << "Failed to create USD stage: " << outputPath << std::endl;
            return nullptr;
        }

        // The actual data conversion from aiScene to USD
        aiMesh * const *meshes = scene->mMeshes;
        for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
        {
            aiMesh const *mesh = meshes[i];
            if (!mesh)
            {
                std::cerr << "Invalid mesh found in scene." << std::endl;
                continue;
            }

            // Convert each mesh to a USD mesh
            ConvertMeshToUsd(mesh, stage);
        }

        // NOTE: Do NOT delete scene - Assimp::Importer manages memory automatically
        return stage;
    }

    void ObjToUsdConverter::Transform(pxr::UsdStageRefPtr stage, const ConverterOptions &options) const
    {
        if (!stage)
        {
            std::cerr << "Invalid USD stage." << std::endl;
            return;
        }

        // Placeholder for transformation logic if needed
        std::cout << "Transforming stage with up axis: " << UpAxisParser::toString(options.upAxis) << std::endl;

        // Set up axis metadata
        pxr::TfToken upAxisToken = UpAxisParser::toToken(options.upAxis);
        pxr::UsdGeomSetStageUpAxis(stage, upAxisToken);
    }

    void ObjToUsdConverter::ConvertMeshToUsd(const ::aiMesh *mesh, pxr::UsdStageRefPtr stage) const
    {
        if (!mesh || !stage)
        {
            std::cerr << "Invalid mesh or stage for conversion." << std::endl;
            return;
        }

        std::string meshName = pxr::TfMakeValidIdentifier(mesh->mName.C_Str());

        // Create a USD mesh at the root level with a unique name
        pxr::UsdGeomMesh usdMesh = pxr::UsdGeomMesh::Define(stage, pxr::SdfPath("/" + meshName));

        // Convert vertex to points
        pxr::VtArray<pxr::GfVec3f> points;
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            const ::aiVector3D &v = mesh->mVertices[i];
            points.push_back(pxr::GfVec3f(v.x, v.y, v.z));
        }

        usdMesh.CreatePointsAttr().Set(points);

        // Convert faces to faceVertexIndices and faceVertexCounts
        pxr::VtArray<int> faceVertexIndices;
        pxr::VtArray<int> faceVertexCounts;

        for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
        {
            const ::aiFace &face = mesh->mFaces[i];
            faceVertexCounts.push_back(face.mNumIndices);
            for (unsigned int j = 0; j < face.mNumIndices; ++j)
            {
                faceVertexIndices.push_back(face.mIndices[j]);
            }
        }

        usdMesh.CreateFaceVertexCountsAttr().Set(faceVertexCounts);
        usdMesh.CreateFaceVertexIndicesAttr().Set(faceVertexIndices);

        // Convert texture coordinates to primvars:st

        // Note: OBJ format does not support normal maps.
        usdMesh.CreateSubdivisionSchemeAttr().Set(pxr::UsdGeomTokens->none);

        // Convert vertex normals to primvars:normals
        if (mesh->mNormals)
        {
            pxr::VtArray<pxr::GfVec3f> normals;
            for (unsigned int i = 0; i < mesh->mNumVertices; i++)
            {
                const ::aiVector3D &n = mesh->mNormals[i];
                normals.push_back(pxr::GfVec3f(n.x, n.y, n.z));
            }

            usdMesh.CreateNormalsAttr().Set(normals);
        }

        std::cout << "Converted mesh: " << meshName << " with " << mesh->mNumVertices << " vertices and " << mesh->mNumFaces << " faces." << std::endl;
    }
} // namespace converters
