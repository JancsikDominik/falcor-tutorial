add_falcor_executable(ModelViewer)

target_sources(ModelViewer PRIVATE
    ModelViewer.cpp
    ModelViewer.h
    ModelViewer.ps.slang
    ModelViewer.vs.slang
)

target_copy_shaders(ModelViewer Samples/ModelViewer)

target_source_group(ModelViewer "Samples")