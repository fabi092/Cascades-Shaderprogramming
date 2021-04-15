#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include "Shlwapi.h"
#include "Pathcch.h"
#include "Global.h"


Shader::Shader(const GLchar* computePath)
{
	if (computePath != nullptr)
		m_sourceFiles.push_back(SourceFile{ computePath, GL_COMPUTE_SHADER });

	m_watcher = new FileWatcher(computePath, Delegate(&Shader::SetDirty, this));
	Load();
}

Shader::Shader(const GLchar* vertexPath, const GLchar* geometryPath, const GLchar* fragmentPath) : Program(0), m_transformFeedbackVariables(nullptr), m_isTempValid(false), m_isValid(false), m_isDirty(true)
{
	if (vertexPath != nullptr)
		m_sourceFiles.push_back(SourceFile{ vertexPath, GL_VERTEX_SHADER });
	if (geometryPath != nullptr)
		m_sourceFiles.push_back(SourceFile{ geometryPath, GL_GEOMETRY_SHADER });
	if (fragmentPath != nullptr)
		m_sourceFiles.push_back(SourceFile{ fragmentPath, GL_FRAGMENT_SHADER });

	const GLchar* sourceFiles[] = { vertexPath , geometryPath, fragmentPath };
	m_watcher = new FileWatcher(sourceFiles, 3, Delegate(&Shader::SetDirty, this));
	Load();
}

Shader::Shader(const GLchar* vertexPath, const GLchar* geometryPath, const GLchar* fragmentPath, const GLchar** transformFeedbackVariables, int variablesCount) : Program(0), m_transformFeedbackVariables(transformFeedbackVariables), m_transformFeedbackVariablesCount(variablesCount), m_isTempValid(false), m_isValid(false), m_isDirty(true)
{
	if (vertexPath != nullptr)
		m_sourceFiles.push_back(SourceFile{ vertexPath, GL_VERTEX_SHADER });
	if (geometryPath != nullptr)
		m_sourceFiles.push_back(SourceFile{ geometryPath, GL_GEOMETRY_SHADER });
	if (fragmentPath != nullptr)
		m_sourceFiles.push_back(SourceFile{ fragmentPath, GL_FRAGMENT_SHADER });

	const GLchar* sourceFiles[] = { vertexPath , geometryPath, fragmentPath };
	m_watcher = new FileWatcher(sourceFiles, 3, Delegate(&Shader::SetDirty, this));
	Load();
}

Shader::Shader(const GLchar* vertexPath, const GLchar* tessCtrlPath, const GLchar* tessEvalPath, const GLchar* geometryPath, const GLchar* fragmentPath) : Program(0), m_transformFeedbackVariables(nullptr), m_isTempValid(false), m_isValid(false), m_isDirty(true)
{
	if (vertexPath != nullptr)
		m_sourceFiles.push_back(SourceFile{ vertexPath, GL_VERTEX_SHADER });
	if (tessCtrlPath != nullptr)
		m_sourceFiles.push_back(SourceFile{ tessCtrlPath, GL_TESS_CONTROL_SHADER });
	if (tessEvalPath != nullptr)
		m_sourceFiles.push_back(SourceFile{ tessEvalPath, GL_TESS_EVALUATION_SHADER });
	if (geometryPath != nullptr)
		m_sourceFiles.push_back(SourceFile{ geometryPath, GL_GEOMETRY_SHADER });
	if (fragmentPath != nullptr)
		m_sourceFiles.push_back(SourceFile{ fragmentPath, GL_FRAGMENT_SHADER });

	const GLchar* sourceFiles[] = { vertexPath, tessCtrlPath, tessEvalPath , geometryPath, fragmentPath };
	m_watcher = new FileWatcher(sourceFiles, 5, Delegate(&Shader::SetDirty, this));
		Load();
}

void Shader::Load()
{
	GLuint tempProgram = glCreateProgram();
	GLint success;

	m_isTempValid = true;
	m_isDirty = false;

	GLuint* shaders = new GLuint[m_sourceFiles.size()] {0};
	
	for (int i = 0; i < m_sourceFiles.size(); ++i)
	{
		shaders[i] = LoadShader(m_sourceFiles[i].path, m_sourceFiles[i].type, m_isTempValid);
		glAttachShader(tempProgram, shaders[i]);
	}

	if (m_transformFeedbackVariables != nullptr)
	{
		glTransformFeedbackVaryings(tempProgram, m_transformFeedbackVariablesCount, m_transformFeedbackVariables, GL_INTERLEAVED_ATTRIBS);
	}

	glLinkProgram(tempProgram);
	glGetProgramiv(tempProgram, GL_LINK_STATUS, &success);
	glCheckError();

	if (m_isTempValid)
	{
		if (success)
		{
			glDeleteProgram(Program);
			Program = tempProgram;
			m_isValid = true;
		}
		else
		{
			m_isTempValid = false;
			// Print linking errors if any
			GLint errorLength;
			glGetProgramiv(tempProgram, GL_INFO_LOG_LENGTH, &errorLength);
			glCheckError();
			char* infoLog = new GLchar[errorLength];
			glGetProgramInfoLog(tempProgram, errorLength, nullptr, infoLog);
			glCheckError();
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
			delete[] infoLog;
			glDeleteProgram(tempProgram);
		}
	}
	else
		glDeleteProgram(tempProgram);


	// Delete the shaders as they're linked into our program now and no longer necessery
	for (int i=0; i < m_sourceFiles.size(); ++i)
	{
		glDeleteShader(shaders[i]);
	}
	glCheckError();
	delete[] shaders;
}

std::string Shader::ReadFile(const GLchar* shaderPath)
{
	std::string shaderCode;
	std::ifstream shaderFile;
	// ensures ifstream objects can throw exceptions:
	shaderFile.exceptions(std::ifstream::badbit);
	try
	{
		// Open files
		shaderFile.open(shaderPath);

		if (!shaderFile.good())
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n" << shaderPath << std::endl;
			return "COULD NOT READ";
		}

		std::stringstream shaderStream;
		// Read file's buffer contents into streams
		shaderStream << shaderFile.rdbuf();
		// close file handlers
		shaderFile.close();
		// Convert stream into string
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n" << shaderPath << std::endl;
	}

	return shaderCode;
}

GLuint Shader::LoadShader(const GLchar* shaderPath, GLenum shaderType, bool& isValid)
{
	std::string shaderCode = ReadFile(shaderPath);
#if DEBUG_SHADER
	size_t newLine = shaderCode.find('\n');
	shaderCode.insert(newLine + 1, "#pragma optimize(off)\n#pragma debug(on)\n");
#endif
	HandleIncludes(shaderCode, shaderPath);

	const GLchar* vShaderCode = shaderCode.c_str();
	// 2. Compile shader
	GLuint shader;
	GLint success;

	shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &vShaderCode, nullptr);
	glCompileShader(shader);
	// Print compile errors if any
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		// Print linking errors if any
		GLint errorLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errorLength);
		char* infoLog = new GLchar[errorLength];
		glGetShaderInfoLog(shader, errorLength, nullptr, infoLog);
		std::cout << "ERROR::SHADER::" << GetShaderName(shaderType) << "::COMPILATION_FAILED\n" << infoLog << std::endl;
		delete[] infoLog;
		isValid = false;
	}

	return shader;
}

void Shader::HandleIncludes(std::string& shaderCode, const GLchar* shaderPath)
{
	GLchar parentPath[MAX_PATH];
	_splitpath_s(shaderPath, nullptr, 0, parentPath, MAX_PATH, nullptr, 0, nullptr, 0);

	char includeText[] = "#pragma include \"";
	int textLength = sizeof(includeText) - 1;

	for (size_t includeStart = shaderCode.find(includeText, 0); includeStart != std::string::npos; includeStart = shaderCode.find(includeText, 0))
	{
		size_t includeEnd = shaderCode.find("\"", includeStart + textLength);
		std::string filePath(parentPath);
		std::string fileName = shaderCode.substr(includeStart + textLength, (includeEnd)-(includeStart + textLength));

		filePath.append(fileName);
		std::string includeCode = ReadFile(filePath.c_str());
		shaderCode.replace(includeStart, (includeEnd + 1) - includeStart, includeCode);
	}
}

GLchar* Shader::GetShaderName(GLenum shaderType)
{
	switch (shaderType)
	{
	case GL_VERTEX_SHADER:
		return "VERTEX";
	case GL_TESS_CONTROL_SHADER:
		return "TESS_CONTROL";
	case GL_TESS_EVALUATION_SHADER:
		return "TESS_EVALUATION";
	case GL_GEOMETRY_SHADER:
		return "GEOMETRY";
	case GL_FRAGMENT_SHADER:
		return "FRAGMENT";
	default:
		return "UNKNOWN";
	}
}

void Shader::Use()
{
	if (m_isDirty)
	{
		std::cout << "INFO::SHADER::HOT_RELOAD" << std::endl;
		Load();
		if (!m_isTempValid)
		{
			std::cout << "INFO::SHADER::HOT_RELOAD_ABORTED" << std::endl;
		}
	}

	glUseProgram(Program);
}

bool Shader::IsValid() const
{
	return  m_isValid;
}

bool Shader::IsDirty() const
{
	return m_isDirty;
}

void Shader::IsDirty(bool dirty)
{
	m_isDirty = dirty;
}

void Shader::SetDirty()
{
	m_isDirty = true;
}

void Shader::Test(const char* debugMessage)
{
		while (!IsValid())
		{
			std::cout << "ERROR::SHADER " << debugMessage << " invalid! Press Enter to reload: ";
			std::cin.ignore();
			Use();
		}
}
