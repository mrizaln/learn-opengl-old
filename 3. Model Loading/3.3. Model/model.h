#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <string_view>
#include <iostream>
#include <cstring>                  // std::strcmp

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // output data structure
#include <assimp/postprocess.h>     // post processing flags
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include <shader_header/shader.h>
#include <mesh_header/mesh.h>       // Vertex, Texture, Mesh


unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma=false);

class Model
{
public:
    Model(const std::string& path)
    {
        loadModel(path);
    }

    void draw(Shader& shader)
    {
        for (auto& mesh : m_meshes)
            mesh.draw(shader);
    }

private:
    // model data
    std::vector<Texture> m_texturesLoaded{};    // stores all the textures loaded so far, optimization to make sure texture aren't loaded more than once.
    std::vector<Mesh>    m_meshes{};
    std::string          m_directory{};
    bool                 m_gammaCorrection{};

    void loadModel(const std::string& path)
    {
        Assimp::Importer importer{};
        const aiScene* scene{ importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs) };
        /**
         * the second argument of Assimp::Importer.ReadFile() function allows us to specify
         * several options that forces Assimp to do extra calculations/operations on the
         * imported data:
         * 
         *   aiProcess_Triangulate      : if the model does not (entirely) consists of triangles,
         *                                it should transform all the model's primitive shapes
         *                                to triangulate first.
         *   aiProcess_FlipUVs          : flips the texture coordinates on the y-axis where necessary
         *                                during processing.
         *   aiProcess_GenNormals       : creates normal vector for each vertex if the model doesn't
         *                                contain normal vectors.
         *   aiProcess_SplitLargeMeshes : splits large meshes into smaller sub-meshes.
         *   aiProcess_OptimizeMeshes   : does the reverse of aiProcess_SplitLargeMeshes.
         * 
         * full docs :  [http://assimp.sourceforge.net/lib_html/postprocess_8h.html]
         */

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }

        // retrieve the directory path of the filepath
        m_directory = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode* node, const aiScene* scene)
    {
        // process all the node's meshes (if any)
        for (unsigned int i{ 0 }; i < node->mNumMeshes; ++i)
        {
            aiMesh* mesh{ scene->mMeshes[node->mMeshes[i]] };
            m_meshes.push_back(processMesh(mesh, scene));
        }

        // then do the same for each of its children
        for (unsigned int i{ 0 }; i < node->mNumChildren; ++i)
            processNode(node->mChildren[i], scene);
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices{};
        std::vector<unsigned int> indices{};
        std::vector<Texture> textures{};

        for (unsigned int i{ 0 }; i < mesh->mNumVertices; ++i)
        {
            // process vertex positions, normals and texture coordinates

            Vertex vertex{
                // position (Assimp calls their vertex position array mVertices)
                { mesh->mVertices[i].x,
                  mesh->mVertices[i].y,
                  mesh->mVertices[i].z },
                // normals
                [&]() -> glm::vec3 {
                    if (mesh->HasNormals())
                        return { mesh->mNormals[i].x,
                                 mesh->mNormals[i].y,
                                 mesh->mNormals[i].z };
                    else
                        return {};
                }(),
                // textures (Assimp allows a model to have up to 8 different texture coordinates per vertex)
                // for now, we're going to use the first set of texture coordinates
                [&]() -> glm::vec2 {
                    if (mesh->mTextureCoords[0])
                        return { mesh->mTextureCoords[0][i].x,
                                 mesh->mTextureCoords[0][i].y };
                    else
                        return {};
                }(),
                // tangent
                [&]() -> glm::vec3 {
                    if (mesh->mTextureCoords[0])
                        return { mesh->mTangents[i].x,
                                 mesh->mTangents[i].y,
                                 mesh->mTangents[i].z };
                    else
                        return {};
                }(),
                // bitangent
                [&]() -> glm::vec3 {
                    if (mesh->mTextureCoords[0])
                        return { mesh->mBitangents[i].x,
                                 mesh->mBitangents[i].y,
                                 mesh->mBitangents[i].z };
                    else
                        return {};
                }()
            };
            vertices.push_back(vertex);
        }

        // process indices
        for (unsigned int i{ 0 }; i < mesh->mNumFaces; ++i)
        {
            aiFace face{ mesh->mFaces[i] };
            for (unsigned int j{ 0 }; j < face.mNumIndices; ++j)
                indices.push_back(face.mIndices[j]);
        }

        // process material
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material{ scene->mMaterials[mesh->mMaterialIndex] };
            
            auto diffuseMaps{
                loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse")
            };
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            auto specularMaps{
                loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular")
            };
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

            auto normalMaps{
                loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal")
            };
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

            auto heightMaps{
                loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height")
            };
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }

        return { vertices, indices, textures };
    }

    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName)
    {
        std::vector<Texture> texes{};
        for (unsigned int i{ 0 }; i < mat->GetTextureCount(type); ++i)
        {
            aiString str;
            mat->GetTexture(type, i, &str);

            bool skip{ false };
            // check if texture has been loaded already
            for (auto& tex : m_texturesLoaded)
            {
                // if (std::strcmp(tex.m_path.data(), str.C_Str()) == 0)
                if (std::strcmp(tex.m_path.c_str(), str.C_Str()) == 0)
                {
                    texes.push_back(tex);
                    skip = true;
                    break;
                }
            }

            // if texture hasn't been loaded already, load it
            if (!skip)
            {
                Texture tex{
                    TextureFromFile(str.C_Str(), m_directory),  // id
                    typeName,                                   // type
                    str.C_Str()                                 // path
                };
                texes.push_back(tex);
                m_texturesLoaded.push_back(tex);
            }
        }

        return texes;
    }
};


unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma=false)
{
    std::string fileName{ directory + "/" + path };
    unsigned int textureID{};
    
    glGenTextures(1, &textureID);

    int width{};
    int height{};
    int nrComponents{};

    unsigned char* data{ stbi_load(fileName.c_str(), &width, &height, &nrComponents, 0) };
    if (data)
    {
        GLenum format{};
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    
    return textureID;
}

#endif