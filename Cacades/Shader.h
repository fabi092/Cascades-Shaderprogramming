#pragma once

#include <GL/glew.h>
#include <chrono>
#include "FileWatcher.h"
typedef std::chrono::system_clock::time_point time_point;

#define DEBUG_SHADER true

class Shader
{
	struct SourceFile
	{
		const GLchar* path;
		GLuint type;
	};

public:
	GLuint Program;
	void Load();
	// Constructor generates the shader on the fly
	explicit Shader(const GLchar* computePath);
	Shader(const GLchar* vertexPath, const GLchar* geometryPath, const GLchar* fragmentPath);
	Shader(const GLchar* vertexPath, const GLchar* geometryPath, const GLchar* fragmentPath, const GLchar** transformFeedbackVariables, int variablesCount);
	Shader(const GLchar* vertexPath, const GLchar* tessCtrlPath, const GLchar* tessEvalPath, const GLchar* geometryPath, const GLchar* fragmentPath);
	
	void Use();
	bool IsValid() const;

	bool IsDirty() const;
	void IsDirty(bool dirty);
	void SetDirty();

	void Test(const char* debugMessage);

private:
	static void HandleIncludes(std::string& shaderCode, const GLchar* shaderPath);
	static std::string ReadFile(const GLchar* shaderPath);
	static GLuint LoadShader(const GLchar* shaderPath, GLenum shaderType, bool& isValid);
	static GLchar* GetShaderName(GLenum shaderType);

	std::vector<SourceFile> m_sourceFiles;
	FileWatcher* m_watcher;
	const GLchar** m_transformFeedbackVariables;
	GLuint m_transformFeedbackVariablesCount;
	bool m_isTempValid, m_isValid, m_isDirty;

};
