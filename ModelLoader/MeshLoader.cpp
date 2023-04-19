#include "MeshLoader.h"

#include <fstream>
#include <sstream>

namespace Falcor::Tutorial
{

    /*
     * loading data from obj files manually:
     * .obj file structure:
     *   '#' is a comment, just like '//' in C++, so we can skip those lines
     *   'v' is a vertex
     *   'vt' is the texture coordinate of one vertex, we skip these lines for now, we aren't applying textures in this tutorial
     *   'vn' is the normal of one vertex, we skip these lines for now, we aren't applying textures in this tutorial
     *   'f' is a face
     *   for f a/b/c d/e/f g/h/i :
     *       a/b/c describes the first vertex of the triangle
     *       d/e/f describes the second vertex of the triangle
     *       g/h/i describes the third vertex of the triangle
     *   for the first vertex:
     *      'a' says which vertex to use.
     *      'b' says which texture coordinate to use.
     *      'c' says which normal to use.
     *      'a', 'b' and 'c' are indexed from 1 not from 0 like in c++
     */

    TriangleMesh::SharedPtr MeshLoader::loadMeshFromObjFile(const std::filesystem::path& path)
    {
        if (path.extension().string() != ".obj")
            return nullptr;

        std::ifstream inFile(path);

        if (!inFile)
            return nullptr;

        ObjFileData data;
        loadRawData(data, inFile);
        createFaces(data);

        return TriangleMesh::create(data.vertices, data.indices);
    }

    void MeshLoader::createFaces(ObjFileData& data)
    {
        std::vector<uint32_t> finalIndices;
        for (size_t i = 0; i < data.indices.size(); i += 3)
        {
            finalIndices.push_back(data.indices[i]);
            data.vertices[data.indices[i]].texCoord = data.vertices[data.indices[i + 1] + data.vertexCount].texCoord;
            data.vertices[data.indices[i]].normal = data.vertices[data.indices[i + 2] + data.texCoordCount + data.vertexCount].normal;
        }

        data.indices = finalIndices;
    }

    void MeshLoader::loadRawData(ObjFileData& data, std::ifstream& inFile)
    {
        std::string line;

        while (std::getline(inFile, line))
        {
            TriangleMesh::Vertex v;

            std::stringstream ss(line);
            std::string inType;

            ss >> inType;

            if (inType == "v")
            {
                ss >> v.position.x >> v.position.y >> v.position.z;
                data.vertices.push_back(v);
                data.vertexCount++;
            }
            else if (inType == "vt")
            {
                ss >> v.texCoord.x >> v.texCoord.y;
                data.vertices.push_back(v);
                data.texCoordCount++;
            }
            else if (inType == "vn")
            {
                ss >> v.normal.x >> v.normal.y >> v.normal.z;
                data.vertices.push_back(v);
                data.normalCount++;
            }
            else if (inType == "f")
            {
                std::string vertexIndices;

                while (std::getline(ss, vertexIndices, ' '))
                {
                    if (vertexIndices.empty())
                        continue;

                    std::istringstream indexSS(vertexIndices);
                    std::string token;

                    // position index
                    std::getline(indexSS, token, '/');
                    data.indices.push_back(std::stoi(token) - 1);

                    // texCoord index
                    std::getline(indexSS, token, '/');
                    data.indices.push_back(std::stoi(token) - 1);

                    // normal index
                    std::getline(indexSS, token, '/');
                    data.indices.push_back(std::stoi(token) - 1);
                }
            }
        }
    }
}

