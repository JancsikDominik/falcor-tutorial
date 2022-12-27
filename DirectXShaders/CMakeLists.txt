add_falcor_executable(DirectXShaders)

target_sources(DirectXShaders PRIVATE
    DirectXShaders.cpp
    DirectXShaders.h
	ShaderBasics.ps.slang
	ShaderBasics.3d.slang
)

target_copy_shaders(DirectXShaders Samples/DirectXShaders)

target_source_group(DirectXShaders "Samples")
