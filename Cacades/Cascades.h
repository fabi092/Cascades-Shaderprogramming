#pragma once
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
#include <stdexcept>
#include "ProcedualGenerator.h"
#include "RenderInfo.h"
#include "ParticleSystem.h"
#include "Camera.h"

class Light;
class Camera;
class Shader;

class Cascades
{
public:
	
	static Cascades* Instance()
	{
		static CGuard g;
		if (!m_instance)
			throw std::logic_error("Call Cascades::Init beforehand!");
		return m_instance;
	}
	static void Init(char* windowTitle);
	static void Init(char* windowTitle, GLuint width, GLuint height);
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
	static void CursorPosCallback(GLFWwindow* window, double x, double y);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	void Update(GLfloat deltaTime);
	void RenderScene();
	void Start();
	void AddLight(Light& light);
protected:
	void RenderLights();
	void UpdateUniforms(const Shader& shader) const;
	void MoveActiveObject();
	void m_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
	void m_CursorPosCallback(GLFWwindow* window, double x, double y);
	void m_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	void m_ResizeCallback(GLFWwindow* window, int width, int height);
	void Loop();

	static GLFWwindow* InitWindow(const char* windowTitle, bool fullscreen);

	GLFWwindow& m_window;
	Shader* m_geometryShader;
	Shader* m_oreShader;
	Shader* m_floorShader;
	Shader* m_debugShader;
	Camera m_camera;
	std::vector<Light*> m_lights;

	ProcedualGenerator m_generator;
	ParticleSystem m_particleSystem;
	int m_activeObject;

	Model* m_floor;
	TriplanarMesh* m_mesh;
		
	const GLuint MaxTexturesPerModel = 10;

	RenderInfo m_renderInfo;
	int fps;
	float heightScale;

	static Cascades* m_instance;
	explicit Cascades(GLFWwindow& window);
	Cascades(const Cascades&);
	~Cascades();
	class CGuard
	{
	public:
		~CGuard()
		{
			if (nullptr != m_instance)
			{
				delete m_instance;
				m_instance = nullptr;
			}
		}
	};
};

