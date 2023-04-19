add_falcor_executable(ModelLoader)

target_sources(ModelLoader PRIVATE
    ModelLoader.cpp
    ModelLoader.h
    MeshLoader.cpp
	MeshLoader.h
    ModelLoader.vs.slang
    ModelLoader.ps.slang
)

target_copy_shaders(ModelLoader Samples/ModelLoader)

target_source_group(ModelLoader "Samples")