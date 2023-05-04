add_falcor_executable(ParametricSurfaces)

target_sources(ParametricSurfaces PRIVATE
    ParametricSurfaces.cpp
    ParametricSurfaces.h
    ParametricSurfaces.ps.slang
	ParametricSurfaces.vs.slang
	ParametricSurfaces.cs.slang
)

target_copy_shaders(ParametricSurfaces Samples/ParametricSurfaces)

target_source_group(ParametricSurfaces "Samples")
