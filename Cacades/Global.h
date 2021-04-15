#pragma once
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <iostream>
#include <iomanip>

const int PATH_APPROXIMATION = 100;
const int VS_IN_POSITION = 0;
const int VS_IN_NORMAL = 1;
const int VS_IN_UV = 2;
const int VS_IN_TANGENT = 3;
const int KEY_COUNT = 512;
const float KEY_SENSITIVITY = 0.05f;
const float MOUSE_SENSITIVITY = 0.005f;

static GLuint SCREEN_WIDTH = 1280;
static GLuint SCREEN_HEIGHT = 720;

#define CHECK_GL_ERROR
#ifdef CHECK_GL_ERROR
#define glCheckError() glCheckError_(__FUNCTION__, __LINE__) 
#else
#define glCheckError() 
#endif

#ifdef CHECK_GL_ERROR
#define glCheckFrameBuffer() glCheckFrameBuffer_(__FUNCTION__, __LINE__) 
#else
#define glCheckFBO() 
#endif

inline glm::quat MakeQuat(GLfloat pitch, GLfloat yaw, GLfloat roll)
{
	return glm::quat(glm::vec3(glm::radians(pitch), glm::radians(yaw), glm::radians(roll)));
}

inline glm::quat MakeQuat(glm::vec3 v)
{
	return MakeQuat(v.x, v.y, v.z);
}

inline void PrintCSAA()
{
	if (GLEW_NV_framebuffer_multisample_coverage)
	{
		int maxModes;
		glGetIntegerv(GL_MAX_MULTISAMPLE_COVERAGE_MODES_NV, &maxModes);
		if (maxModes != 0)
		{
			int* modes = new int[2 * maxModes];
			glGetIntegerv(GL_MULTISAMPLE_COVERAGE_MODES_NV, modes);

			std::cout << "Coverage|Color" << std::endl;
			std::setfill(' ');
			for (int i = 0; i < maxModes; ++i)
			{
				std::cout << std::setw(8) << modes[2 * i] << '|' << modes[2 * i + 1] << std::endl;
			}
		}
		else
			std::cout << "No CSAA modes found";
	}
	else
		std::cout << "CSAA-Extension not available";
}

GLenum static glCheckFrameBuffer_(const char *function, int line)
{
	GLuint errorCode = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	std::string error;
	switch (errorCode)
	{
		case GL_FRAMEBUFFER_COMPLETE:						return errorCode;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:			error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:	error = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:			error = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:			error = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"; break;
	}
	printf("%s | %s (%i)\n", error.c_str(), function, line);
	//std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	std::cin.ignore();

	return errorCode;
}

GLenum static glCheckError_(const char *function, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
			case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
			case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
			case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
			case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
			case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
			case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		printf("%s | %s (%i)\n", error.c_str(), function, line);
		//std::cout << error << " | " << file << " (" << line << ")" << std::endl;
		std::cin.ignore();
	}
	return errorCode;
}

inline float ClampAngles(const float orientation)
{
	if (orientation > glm::pi<float>())
		return orientation - glm::two_pi<float>();

	if (orientation < -glm::pi<float>())
		return  orientation + glm::two_pi<float>();

	return orientation;
}

inline glm::vec3 ClampAngles(const glm::vec3& v)
{
	return glm::vec3(ClampAngles(v.x), ClampAngles(v.y), ClampAngles(v.z));
}