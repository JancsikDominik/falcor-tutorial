#include "Scene/TriangleMesh.h"

namespace Falcor::Tutorial
{
    class MeshLoader
    {
    public:
        MeshLoader() = delete;

        static TriangleMesh::SharedPtr loadMeshFromObjFile(const std::filesystem::path& path);

    private:
        struct ObjFileData
        {
            TriangleMesh::VertexList vertices;
            TriangleMesh::IndexList indices;

            size_t vertexCount = 0;
            size_t texCoordCount = 0;
            size_t normalCount = 0;
        };

        static void createFaces(ObjFileData& data);
        static void loadRawData(ObjFileData& data, std::ifstream& inFile);
    };
}
