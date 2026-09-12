#pragma once

enum MeshType 
{
	M_TRIANGLE = 1,
	M_QUAD = 2,
	M_CUBE = 3,
	M_SPHERE = 4
};

enum ShaderType
{
	S_COLOR = 1,
	S_TEXTURE = 2,
	S_COLOR_LIT = 3,
	S_TEXTURE_LIT = 4
};

enum LightType
{
	L_DIRECTIONAL = 0,
	L_POINT = 1,
	L_SPOT = 2
};

// Maximum number of simultaneous lights supported by the per-frame uniform
// buffer (see GraphicsEngine/LightData.h). This is the single source of
// truth for that limit: ShaderLoader automatically injects
// "#define MAX_LIGHTS <value>" into every shader at compile time, so this
// value never needs to be hand-copied into any .vs/.fs file.
constexpr int MAX_LIGHTS = 16;
