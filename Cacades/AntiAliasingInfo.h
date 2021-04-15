#pragma once
#include <sstream>
#include <GL/glew.h>

struct AntiAliasingInfo
{
	GLsizei Samples = 8;
	GLsizei ActiveSamples = 8;
	GLsizei MaxSamples;
	GLuint ColorBuffer;
	GLuint DepthBuffer;

	void Init()
	{
		glGenRenderbuffers(1, &ColorBuffer);
		glGenRenderbuffers(1, &DepthBuffer);


		if (GLEW_NV_framebuffer_multisample_coverage)
		{
			int maxModes;
			glGetIntegerv(GL_MAX_MULTISAMPLE_COVERAGE_MODES_NV, &maxModes);
			if (maxModes != 0)
			{
				int* modes = new int[2 * maxModes];
				glGetIntegerv(GL_MULTISAMPLE_COVERAGE_MODES_NV, modes);

				MaxSamples = modes[2 * (maxModes - 1)];
				return;
			}
		}

		MaxSamples = 16;
	}

	std::string ParseAAMode() const
	{
		if (Samples == 0)
			return "Deaktiviert";

		std::stringstream ss;
		ss << ActiveSamples << "x MSAA";
		return ss.str();
	}
};
