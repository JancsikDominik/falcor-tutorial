add_falcor_executable(DirectXShaders)

target_sources(DirectXShaders PRIVATE
    DirectXShaders.cpp
    DirectXShaders.h
	Mandelbrot.vs.slang
	Mandelbrot.ps.slang
)

target_copy_shaders(DirectXShaders Samples/DirectXShaders)

target_source_group(DirectXShaders "Samples")
