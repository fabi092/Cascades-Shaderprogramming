#pragma once
#include <GL/glew.h>
#include "Shader.h"
#include <glm/detail/type_vec2.hpp>
#include <glm/detail/type_vec3.hpp>
#include "Enums.h"
#include "Texture.h"
#include "BaseObject.h"

struct RenderInfo;
class Camera;

struct Particle
{
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec2 seed;
	float lifeTime;
	ParticleType type;
};

class ParticleSystem : public BaseObject
{
public:
	ParticleSystem(const Camera& camera, const Texture& densityTex, const Texture& normalTexture);
	~ParticleSystem();

	void Update(GLfloat deltaTime);
	void UpdateUniformsU(GLfloat deltaTime);
	void Render(const RenderInfo& info);
	void UpdateUniformsR(const RenderInfo& info);

	void AddEmitter(glm::vec3 viewPos, glm::vec3 viewDir);

	void SetResolution(glm::ivec3 cubesPerDimension);
	void Reset();
protected:

	void SwapBuffer();

	Shader *m_updateShader, *m_renderShader;

	const int MAX_PARTICLES = 1000;
	const int PARTICLE_SIZE = 2 * sizeof(glm::vec3) + 2 * sizeof(float) + sizeof(int);

	GLuint readBuf, writeBuf;

	GLuint m_vaos[2];
	GLuint m_vbos[2];
	GLuint m_particleCount;

	glm::vec3 m_resolution;

	const Camera& m_camera;
	const Texture &m_densityTex, &m_normalTex;
};

