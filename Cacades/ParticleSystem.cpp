// float4 pos; float4 vel; float2 seed; float time; int type
#include "ParticleSystem.h"
#include "Global.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "RenderInfo.h"
#include "Camera.h"

ParticleSystem::ParticleSystem(const Camera& camera, const Texture& densityTex, const Texture& normalTexture)
	: BaseObject(glm::vec3(0)), readBuf(0), writeBuf(1), m_particleCount(0), m_camera(camera), m_densityTex(densityTex), m_normalTex(normalTexture)
{
	GLchar** feedbackVaryings = new GLchar*[5]{ "gs_out.position", "gs_out.velocity", "gs_out.lifeTime", "gs_out.seed", "gs_out.type" };
	m_updateShader = new Shader("./shaders/ParticleUpdate.vert", "./shaders/ParticleUpdate.geom", nullptr, const_cast<const GLchar**>(feedbackVaryings), 5);
	m_updateShader->Test("ParticleUpdateShader");

	m_renderShader = new Shader("./shaders/ParticleRender.vert", "./shaders/ParticleRender.geom", "./shaders/ParticleRender.frag");
	m_renderShader->Test("ParticleRenderShader");

	glGenVertexArrays(2, m_vaos);
	glGenBuffers(2, m_vbos);

	for (int i = 0; i < 2; ++i)
	{
		glBindVertexArray(m_vaos[i]);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbos[i]);
		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// Velocity attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)sizeof(glm::vec3));
		glEnableVertexAttribArray(1);
		// seed attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)(2 * sizeof(glm::vec3)));
		glEnableVertexAttribArray(2);
		// lifeTime attribute
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
		glEnableVertexAttribArray(3);
		// type attribute
		glVertexAttribIPointer(4, 1, GL_INT, sizeof(Particle), (GLvoid*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2) + sizeof(float)));
		glEnableVertexAttribArray(4);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glCheckError();

	for (int i = 0; i < 2; ++i)
	{
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, m_vbos[i]);
		glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, MAX_PARTICLES * sizeof(Particle), nullptr, GL_STREAM_COPY);
	}
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
	glCheckError();
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Update(GLfloat deltaTime)
{
	if (m_particleCount == 0)
		return;

	m_updateShader->Use();
	UpdateUniformsU(deltaTime);

	GLuint queryTF;
	glGenQueries(1, &queryTF);

	glEnable(GL_RASTERIZER_DISCARD);

	glBindVertexArray(m_vaos[readBuf]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_vbos[writeBuf]);

	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, queryTF);
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, m_particleCount);
	glEndTransformFeedback();
	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

	glBindVertexArray(0);
	glCheckError();

	// Fetch result
	Particle* particlesBefore = new Particle[m_particleCount];
	glBindBuffer(GL_ARRAY_BUFFER, m_vbos[readBuf]);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, m_particleCount * sizeof(Particle), particlesBefore);
	glCheckError();

	glGetQueryObjectuiv(queryTF, GL_QUERY_RESULT, &m_particleCount);

	glFlush();

	if (m_particleCount > 0)
	{
		Particle* particlesAfter = new Particle[m_particleCount];
		glBindBuffer(GL_ARRAY_BUFFER, m_vbos[writeBuf]);
		glGetBufferSubData(GL_ARRAY_BUFFER, 0, m_particleCount * sizeof(Particle), particlesAfter);
		glCheckError();
		delete[] particlesAfter;
	}
	delete[] particlesBefore;


	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);

	glFlush();

	glDisable(GL_RASTERIZER_DISCARD);
	glDeleteQueries(1, &queryTF);

	SwapBuffer();
	glCheckError();
}

void ParticleSystem::UpdateUniformsU(GLfloat deltaTime)
{
	GLint deltaTimeLoc = glGetUniformLocation(m_updateShader->Program, "deltaTime");
	glUniform1f(deltaTimeLoc, deltaTime);
	glCheckError();

	int densityLoc = glGetUniformLocation(m_updateShader->Program, "densityTex");
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, m_densityTex.GetId());
	glUniform1i(densityLoc, 1);
	glCheckError();

	int normalLoc = glGetUniformLocation(m_updateShader->Program, "normalTex");
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, m_normalTex.GetId());
	glUniform1i(normalLoc, 2);
	glCheckError();
}

void ParticleSystem::Render(const RenderInfo& info)
{
	if (m_particleCount == 0)
		return;

	m_renderShader->Use();
	UpdateUniformsR(info);

	glBindVertexArray(m_vaos[readBuf]);
	glDrawArrays(GL_POINTS, 0, m_particleCount);
	glBindVertexArray(0);
	glCheckError();
}

void ParticleSystem::UpdateUniformsR(const RenderInfo& info)
{
	GLint resolutionLoc = glGetUniformLocation(m_renderShader->Program, "resolution");
	glUniform3fv(resolutionLoc, 1, glm::value_ptr(m_resolution));
	glCheckError();

	int normalLoc = glGetUniformLocation(m_renderShader->Program, "normalTex");
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, m_normalTex.GetId());
	glUniform1i(normalLoc, 2);
	glCheckError();

	GLint modelLoc = glGetUniformLocation(m_renderShader->Program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(GetMatrix()));
	glCheckError();

	glm::mat4 view = m_camera.GetViewMatrix();
	GLuint viewLocation = glGetUniformLocation(m_renderShader->Program, "view");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
	glCheckError();

	glm::mat4 proj = m_camera.GetProjectionMatrix();
	GLuint projLocation = glGetUniformLocation(m_renderShader->Program, "projection");
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(proj));
	glCheckError();
}

void ParticleSystem::AddEmitter(glm::vec3 viewPos, glm::vec3 viewDir)
{
	if (m_particleCount == MAX_PARTICLES)
		return;

	Particle rayParticle;
	glm::mat4 antiModel = glm::inverse(GetMatrix());
	rayParticle.position = glm::vec3(antiModel * glm::vec4(viewPos, 1.0f));
	rayParticle.velocity = glm::normalize(glm::vec3(antiModel * glm::vec4(viewDir, 1.0f)));
	rayParticle.lifeTime = 0;
	rayParticle.seed = glm::vec2(rand(), rand());
	rayParticle.type = EmitterRay;

	glBindBuffer(GL_ARRAY_BUFFER, m_vbos[readBuf]);
	glBufferSubData(GL_ARRAY_BUFFER, m_particleCount * sizeof(Particle), sizeof(Particle), &rayParticle);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	++m_particleCount;

	glCheckError();
}

void ParticleSystem::SetResolution(glm::ivec3 cubesPerDimension)
{
	m_resolution = 2.0f / glm::vec3(cubesPerDimension);
}

void ParticleSystem::Reset()
{
	m_particleCount = 0;
}

void ParticleSystem::SwapBuffer()
{
	readBuf = 1 - readBuf;
	writeBuf = 1 - writeBuf;
}
