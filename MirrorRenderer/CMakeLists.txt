add_falcor_executable(MirrorRenderer)

target_sources(MirrorRenderer PRIVATE
    MirrorRenderer.cpp
    MirrorRenderer.h
	MirrorRenderer.vs.slang
    MirrorRenderer.ps.slang
	Mirror.cpp
	Mirror.h
	Observer.cpp
	Observer.h
	Object.h
	Object.cpp
)

target_copy_shaders(MirrorRenderer Samples/MirrorRenderer)

target_source_group(MirrorRenderer "Samples")