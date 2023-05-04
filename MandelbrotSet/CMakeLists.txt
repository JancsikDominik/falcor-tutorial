add_falcor_executable(MandelbrotSet)

target_sources(MandelbrotSet PUBLIC
    MandelbrotRenderer.cpp
    MandelbrotRenderer.h
	Mandelbrot.vs.slang
	Mandelbrot.ps.slang
)

target_copy_shaders(MandelbrotSet Samples/MandelbrotSet)

target_source_group(MandelbrotSet "Samples")