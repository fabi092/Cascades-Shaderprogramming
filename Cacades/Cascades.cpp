#include "Cascades.h"
#include <GL/glew.h>
#include "Shader.h"
#include "Camera.h"
#include "Global.h"
#include <sstream>
#include <glm/gtc/type_ptr.hpp>
#include "Light.h"
#include "Plane.h"

Cascades::Cascades(GLFWwindow& window)
	: m_window(window), m_camera(), m_generator(),
	m_particleSystem(m_camera, m_generator.GetDensityTexture(), m_generator.GetNormalTexture()),
	m_activeObject(-1), m_mesh(nullptr)
{
	m_geometryShader = new Shader("./shaders/TriPlanar.vert", "./shaders/TriPlanar.tesc", "./shaders/TriPlanar.tese", "./shaders/TriPlanar.geom", "./shaders/TriPlanar.frag");
	m_geometryShader->Test("TriPlanar");

	m_oreShader = new Shader("./shaders/Tessellation.vert", "./shaders/Tessellation.tesc", "./shaders/Tessellation.tese", "./shaders/Tessellation.geom", "./shaders/Tessellation.frag");
	m_oreShader->Test("Tesselation");

	m_floorShader = new Shader("./shaders/default.vert", nullptr, "./shaders/default.frag");
	m_floorShader->Test("Floor");

	m_debugShader = new Shader("./shaders/Simple.vert", nullptr, "./shaders/Simple.frag");
	m_debugShader->Test("Debug");

	Texture* floorTex = new Texture("textures/bricks2_COLOR.jpg");
	Texture* floorNormal = new Texture("textures/bricks2_normal.jpg");
	Texture* floorHeight = new Texture("textures/bricks2_disp.jpg");
	m_floor = new Model(glm::vec3(-7, -10, -7), MakeQuat(0.0f, 0.0f, 0.0f), Plane::GetTris(glm::vec2(20, 20), glm::vec2(10, 10)), 2, glm::vec3(0.3f, 0.3f, 0.3f), TextureOnly, floorTex, BumpMapOnly, floorNormal, DepthDisplacement, floorHeight);
}

Cascades::~Cascades()
{
	glfwSetWindowShouldClose(&m_window, GL_TRUE);
}

GLFWwindow* Cascades::InitWindow(const char* windowTitle, bool fullscreen)
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	GLFWmonitor* monitor = nullptr;

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, windowTitle, monitor, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, CursorPosCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	glGetError(); // Call it once to catch glewInit()

	// OpenGL configuration
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);

	return window;
}

void Cascades::Init(char* windowTitle)
{
	m_instance = new Cascades(*InitWindow(windowTitle, true));
}

void Cascades::Init(char* windowTitle, GLuint width, GLuint height)
{
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	m_instance = new Cascades(*InitWindow(windowTitle, false));
}

void Cascades::Start()
{
	m_renderInfo.Seed = -7;
	m_renderInfo.Resolution = glm::vec3(m_generator.WIDTH, m_generator.LAYERS, m_generator.DEPTH);
	m_renderInfo.StartLayer = 0;
	m_renderInfo.NoiseScale = 0.6f;
	m_renderInfo.IsoLevel = 0;
	m_renderInfo.GeometryScale = glm::vec3(5, 10, 5);
	m_renderInfo.ShadowMode = HardShadows;
	m_renderInfo.WireFrameMode = false;


	m_generator.SetRandomSeed(m_renderInfo.Seed);
	m_generator.SetResolution(m_renderInfo.Resolution);
	m_generator.SetStartLayer(m_renderInfo.StartLayer);
	m_generator.SetNoiseScale(m_renderInfo.NoiseScale);
	m_generator.SetIsoLevel(m_renderInfo.IsoLevel);
	m_generator.SetGeometryScale(m_renderInfo.GeometryScale);

	m_particleSystem.SetScale(m_renderInfo.GeometryScale);
	m_particleSystem.SetResolution(m_renderInfo.Resolution);

	m_generator.GenerateMcVbo();
	m_generator.Generate3dTexture();
	m_mesh = m_generator.GenerateMesh();

	Loop();
}

void Cascades::AddLight(Light& light)
{
	m_lights.push_back(&light);
}

void Cascades::Update(GLfloat deltaTime)
{
	MoveActiveObject();

	m_particleSystem.Update(deltaTime);
}

void Cascades::RenderScene()
{
	if (m_renderInfo.WireFrameMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	m_geometryShader->Use();
	UpdateUniforms(*m_geometryShader);
	m_mesh->Render(*m_geometryShader, true);
	glCheckError();

	m_oreShader->Use();
	UpdateUniforms(*m_oreShader);
	glCheckError();

	m_floorShader->Use();
	UpdateUniforms(*m_floorShader);
	m_floor->Render(*m_floorShader);
	glCheckError();

	m_debugShader->Use();
	UpdateUniforms(*m_debugShader);
	glCheckError();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (m_renderInfo.DrawLightPosition)
		for (std::vector<Light*>::const_iterator it = m_lights.begin(); it != m_lights.end(); ++it)
			if ((*it)->IsEnabled())
				(*it)->RenderDebug(*m_debugShader);

	glCheckError();
}

void Cascades::Loop()
{
	// DeltaTime variables
	GLfloat deltaTime = 0.0f;
	GLfloat lastFrame = 0.0f;
	GLfloat second = 0.0f;
	int frame = 0;

	// Rendering Loop
	while (!glfwWindowShouldClose(&m_window))
	{
		// Calculate delta time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;

		second += deltaTime;
		lastFrame = currentFrame;
		glfwPollEvents();

		Update(deltaTime);

		RenderLights();

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClearColor(.1f, .1f, .1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderScene();

		m_particleSystem.Render(m_renderInfo);

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		glCheckError();


		glfwSwapBuffers(&m_window);
		glCheckError();

		++frame;

		if (second > 1)
		{
			fps = frame;
			frame = 0;
			second -= 1;
			std::cout << fps<<std::endl;
		}
	}
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
}

void Cascades::RenderLights()
{
	glCullFace(GL_FRONT);
	for (std::vector<Light*>::const_iterator it = m_lights.begin(); it != m_lights.end(); ++it)
	{
		if (!(*it)->CastsShadows() || !(*it)->IsEnabled())
			continue;

		(*it)->PreRender();
		glEnable(GL_CULL_FACE);
		m_mesh->Render((*it)->GetShadowShader(), false);
		glDisable(GL_CULL_FACE);
		m_floor->Render((*it)->GetShadowShader());
		(*it)->PostRender();
	}
	glCullFace(GL_BACK);
}

void Cascades::UpdateUniforms(const Shader& shader) const
{
	for (int i = 0; i < m_lights.size(); ++i)
	{
		m_lights[i]->UpdateUniforms(shader, i, MaxTexturesPerModel + i);
	}

	glm::vec3 viewPos = m_camera.GetPosition();
	GLuint viewPosLocation = glGetUniformLocation(shader.Program, "viewPos");
	glUniform3fv(viewPosLocation, 1, glm::value_ptr(viewPos));
	glCheckError();

	glm::mat4 view = m_camera.GetViewMatrix();
	GLuint viewLocation = glGetUniformLocation(shader.Program, "view");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
	glCheckError();

	glm::mat4 proj = m_camera.GetProjectionMatrix();
	GLuint projLocation = glGetUniformLocation(shader.Program, "projection");
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(proj));
	glCheckError();

	GLuint bumbinessLocation = glGetUniformLocation(shader.Program, "bumbiness");
	glUniform1f(bumbinessLocation, m_renderInfo.NormalMapFactor);
	glCheckError();

	GLuint initialStepsLocation = glGetUniformLocation(shader.Program, "displacement_initialSteps");
	glUniform1i(initialStepsLocation, m_renderInfo.DisplacementInitialSteps);
	glCheckError();

	GLuint refinementStepsLocation = glGetUniformLocation(shader.Program, "displacement_refinementSteps");
	glUniform1i(refinementStepsLocation, m_renderInfo.DisplacementRefinementSteps);
	glCheckError();

	GLuint scaleLocation = glGetUniformLocation(shader.Program, "heightScale");
	glUniform1f(scaleLocation, m_renderInfo.heightScale);
	glCheckError();
}

void Cascades::MoveActiveObject()
{
	if (m_activeObject == -1)
		m_camera.ProcessInput(m_window);

	else
		m_lights[m_activeObject]->ProcessInput(m_window);
}

void Cascades::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	Instance()->m_KeyCallback(window, key, scancode, action, mode);
}

// Is called whenever a key is pressed/released via GLFW
void Cascades::m_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS || action == GLFW_REPEAT)
		switch (key)
		{
		case GLFW_KEY_X:
		{
			m_particleSystem.Reset();
		} break;

		case GLFW_KEY_C:
		{
			m_renderInfo.WireFrameMode = !m_renderInfo.WireFrameMode;
		} break;
		case GLFW_KEY_Q:
		{
			m_renderInfo.ShadowMode = static_cast<ShadowMode>(m_renderInfo.ShadowMode + 1);
			if (m_renderInfo.ShadowMode > VsmShadows)
				m_renderInfo.ShadowMode = HardShadows;

			for (std::vector<Light*>::const_iterator it = m_lights.begin(); it != m_lights.end(); ++it)
				(*it)->SetShadowMode(m_renderInfo.ShadowMode);
		} break;

		case GLFW_KEY_T:
			m_renderInfo.StartLayer -= 5;
			m_generator.SetStartLayer(m_renderInfo.StartLayer);
			m_generator.Generate3dTexture();
			m_mesh = m_generator.GenerateMesh();
			break;
		case GLFW_KEY_G:
			m_renderInfo.StartLayer += 5;
			m_generator.SetStartLayer(m_renderInfo.StartLayer);
			m_generator.Generate3dTexture();
			m_mesh = m_generator.GenerateMesh();
			break;

		case GLFW_KEY_Y:
			m_renderInfo.NoiseScale = std::min(4.0f, m_renderInfo.NoiseScale + 0.2f);
			m_generator.SetNoiseScale(m_renderInfo.NoiseScale);
			m_mesh = m_generator.GenerateMesh();
			break;
		case GLFW_KEY_H:
			m_renderInfo.NoiseScale = std::max(0.2f, m_renderInfo.NoiseScale - 0.2f);
			m_generator.SetNoiseScale(m_renderInfo.NoiseScale);
			m_mesh = m_generator.GenerateMesh();
			break;

		case GLFW_KEY_U:
			++m_renderInfo.Seed;
			m_generator.SetRandomSeed(m_renderInfo.Seed);
			m_generator.Generate3dTexture();
			m_mesh = m_generator.GenerateMesh();
			break;
		case GLFW_KEY_J:
			--m_renderInfo.Seed;
			m_generator.SetRandomSeed(m_renderInfo.Seed);
			m_generator.Generate3dTexture();
			m_mesh = m_generator.GenerateMesh();
			break;

		case GLFW_KEY_I:
			m_renderInfo.heightScale += 0.0025;
			break;
		case GLFW_KEY_K:
			m_renderInfo.heightScale -= 0.0025;
			break;

		case GLFW_KEY_O:
			++m_renderInfo.DisplacementRefinementSteps;
			break;
		case GLFW_KEY_L:
			if (m_renderInfo.DisplacementRefinementSteps > 1)
				--m_renderInfo.DisplacementRefinementSteps;
			break;
		}
}

void Cascades::CursorPosCallback(GLFWwindow* window, double x, double y)
{
	Instance()->m_CursorPosCallback(window, x, y);
}

void Cascades::m_CursorPosCallback(GLFWwindow* window, double x, double y)
{
	m_camera.CursorPosCallback(x, y);
}

void Cascades::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	Instance()->m_MouseButtonCallback(window, button, action, mods);
}

void Cascades::m_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
	{
		m_particleSystem.AddEmitter(m_camera.GetPosition(), -(glm::vec3(0, 0, 1) * m_camera.GetOrientation()));
	}
}

void Cascades::m_ResizeCallback(GLFWwindow* window, int width, int height)
{
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

Cascades* Cascades::m_instance = nullptr;