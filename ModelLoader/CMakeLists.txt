add_falcor_executable(ModelLoader)

target_sources(ModelLoader PRIVATE
    ModelLoader.cpp
    ModelLoader.h
    ModelLoader.ps.slang
    ModelLoader.vs.slang
	Mandelbrot.ps.slang
	Mandelbrot.vs.slang
)

target_copy_shaders(ModelLoader Samples/ModelLoader)

target_source_group(ModelLoader "Samples")