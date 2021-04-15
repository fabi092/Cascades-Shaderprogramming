#pragma once
#include "Shader.h"
#include "Cascades.h"
#include "Camera.h"
#include "Global.h"
#include "DirectionalLight.h"

#define CPP true

void TestCSAA()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(10, 10, "Test", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	PrintCSAA();

	glfwSetWindowShouldClose(window, GL_TRUE);
	glfwTerminate();

	std::cin.ignore();
}

int main(int argc, char** argv)
{
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "--test") == 0)
		{
			TestCSAA();
			return 0;
		}
	}

	try {
		Cascades::Init("Camera", 1600, 800);

		Cascades* Cascades = Cascades::Instance();

		Shader* directionalLightShader = new Shader("./shaders/DirectionalLight.vert", nullptr, "./shaders/DirectionalLight.frag");
		directionalLightShader->Test("directionalLightShader");

		DirectionalLight* mainLight1 = new DirectionalLight(glm::vec3(10,10,10), glm::vec3(0.8f), *directionalLightShader, 50, -10);
		mainLight1->IsEnabled(true);
		mainLight1->CastsShadows(true);
		Cascades->AddLight(*mainLight1);

		Cascades->Start();

		return 0;
	}
	catch (std::exception& e)
	{
		std::cout << "ERROR::" << e.what() << " | " << __FILE__ << " (" << __LINE__ << ")" << std::endl;
		std::cin.ignore();
		return -1;
	}
}