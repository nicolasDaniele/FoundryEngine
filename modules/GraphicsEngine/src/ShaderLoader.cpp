#include <glad/glad.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "GraphicsEngine/ShaderLoader.h"
#include "EngineInterfaces/GraphicsTypes.h"

GLuint ShaderLoader::CreateProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename)
{
	std::string vertex_shader_code = ReadShader(vertexShaderFilename);
	std::string fragment_shader_code = ReadShader(fragmentShaderFilename);

	GLuint vertex_shader = CreateShader(GL_VERTEX_SHADER, vertex_shader_code, "vertex shader");
	GLuint fragment_shader = CreateShader(GL_FRAGMENT_SHADER, fragment_shader_code, "fragment shader");
	
	int link_result = 0;
	GLuint program = glCreateProgram();

	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_result);

	if (link_result == GL_FALSE) {
		int info_log_length = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
		std::vector<char> program_log(info_log_length);
		glGetProgramInfoLog(program, info_log_length, NULL, &program_log[0]);

		std::cout << "Shader Loader : LINK ERROR" << std::endl << &program_log[0] << std::endl;

		return 0;
	}

	return program;
}

std::string ShaderLoader::ReadShader(const char* filename)
{
	std::string shaderCode;
	std::ifstream file(filename, std::ios::in);

	if (!file.good()) {
		std::cout << "Can't read file " << filename << std::endl;
		std::terminate();
	}

	file.seekg(0, std::ios::end);
	shaderCode.resize((unsigned int)file.tellg());
	file.seekg(0, std::ios::beg);
	file.read(&shaderCode[0], shaderCode.size());
	file.close();

	return shaderCode;
}

std::string ShaderLoader::InjectSharedDefines(std::string source)
{
	// Keeps MAX_LIGHTS in sync with EngineInterfaces/GraphicsTypes.h
	// automatically: every shader gets "#define MAX_LIGHTS <value>" injected
	// right after its #version line, so lit shaders never need a hand-copied
	// #define that could drift out of sync with the engine's actual constant.
	std::string defineLine = "#define MAX_LIGHTS " + std::to_string(MAX_LIGHTS) + "\n";

	size_t versionLineEnd = source.find('\n');
	if (versionLineEnd == std::string::npos)
		return defineLine + source;

	return source.substr(0, versionLineEnd + 1) + defineLine + source.substr(versionLineEnd + 1);
}

GLuint ShaderLoader::CreateShader(GLenum shaderType, std::string source, const char* shaderName)
{
	source = InjectSharedDefines(source);

	int compile_result = 0;
	GLuint shader = glCreateShader(shaderType);

	const char* shader_code_ptr = source.c_str();
	const int shader_code_size = (int)source.size();

	glShaderSource(shader, 1, &shader_code_ptr,	&shader_code_size);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_result);

	if (compile_result == GL_FALSE)
	{
		int info_log_length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
		std::vector<char> shader_log(info_log_length);
		glGetShaderInfoLog(shader, info_log_length, NULL, &shader_log[0]);

		std::cout << "ERROR compiling shader: " << shaderName << std::endl << &shader_log[0] << std::endl;
		
		return 0;
	}

	return shader;
}
