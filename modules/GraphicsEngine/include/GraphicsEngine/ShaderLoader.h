#pragma once

class ShaderLoader
{
public:
	GLuint CreateProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename);

private:
	std::string ReadShader(const char* filename);
	GLuint CreateShader(GLenum shaderType, std::string source, const char* shaderName);

	// Injects engine-wide #defines (currently MAX_LIGHTS) right after the
	// shader's #version line, so values like MAX_LIGHTS only need to be
	// declared once, in EngineInterfaces/GraphicsTypes.h.
	std::string InjectSharedDefines(std::string source);
};
