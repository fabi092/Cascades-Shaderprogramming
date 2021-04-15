#include "ProcedualGenerator.h"
#include <GL/glew.h>
#include <glm/gtx/optimum_pow.hpp>
#include "Global.h"
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include <string>

ProcedualGenerator::ProcedualGenerator() : m_noise(nullptr), m_random(0), m_randomAngle(0, 359), m_randomRand(-glm::pi<float>(), glm::pi<float>()), m_randomFloat(0.0f, 1000.0f)
{
	SetupDensity(); 
	SetupMC();
}

void ProcedualGenerator::SetupMC()
{
	GLchar** feedbackVaryings = new GLchar*[3]{ "gs_out.position", "gs_out.normal", "gs_out.uvw" };
	m_marchingCubeShader = new Shader("./shaders/MarchingCubes.vert", "./shaders/MarchingCubes.geom", nullptr, const_cast<const GLchar**>(feedbackVaryings), 3);
	m_marchingCubeShader->Test("MarchingCubes");

	m_lookupTable.WriteLookupTablesToGpu();

	glGenVertexArrays(1, &m_vaoMc);
	glGenBuffers(1, &m_vboMc);

	glBindVertexArray(m_vaoMc);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboMc);
	// Position attribute
	glEnableVertexAttribArray(VS_IN_POSITION);
	glVertexAttribPointer(VS_IN_POSITION, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenBuffers(1, &m_tboMc);
	glCheckError();
}

void ProcedualGenerator::SetupDensity()
{
	m_densityShader = new  Shader("./shaders/Density.vert", "./shaders/Density.geom", "./shaders/Density.frag");
	m_densityShader->Test("Density");

	m_normalShader = new  Shader("./shaders/Normal.vert", "./shaders/Normal.geom", "./shaders/Normal.frag");
	m_normalShader->Test("Normal");

	glBindTexture(GL_TEXTURE_3D, m_densityTex.GetId());
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, WIDTH, DEPTH, LAYERS, 0, GL_RED, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glCheckError();

	glGenFramebuffers(1, &m_fboD);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboD);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_densityTex.GetId(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glCheckError();

	glBindTexture(GL_TEXTURE_3D, 0);
	glCheckError();

	glBindTexture(GL_TEXTURE_3D, m_normalTex.GetId());
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB16F, WIDTH, DEPTH, LAYERS, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glCheckError();

	glGenFramebuffers(1, &m_fboN);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboN);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_normalTex.GetId(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glCheckError();

	glBindTexture(GL_TEXTURE_3D, 0);
	glCheckError();

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("ERROR::DENSITY::FRAMEBUFFER_NOT_COMPLETE");
		std::cin.ignore();
	}

	GLfloat vertices[6][2] = {
		{ -1,  1 },
		{ -1, -1 },
		{  1, -1 },

		{ -1,  1 },
		{  1, -1 },
		{  1,  1 }
	};

	glGenVertexArrays(1, &m_vaoD);
	glGenBuffers(1, &m_vboD);
	glBindVertexArray(m_vaoD);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboD);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VS_IN_POSITION, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(VS_IN_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glCheckError();
}


ProcedualGenerator::~ProcedualGenerator()
{
	glDeleteVertexArrays(1, &m_vaoMc);
	glDeleteBuffers(1, &m_vboMc);
	glDeleteVertexArrays(1, &m_vaoD);
	glDeleteBuffers(1, &m_vboD);
	glDeleteFramebuffers(1, &m_fboD);
	glDeleteBuffers(1, &m_tboMc);
}

void ProcedualGenerator::Generate3dTexture()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboD);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glReadBuffer(GL_NONE);

	glViewport(0, 0, WIDTH, DEPTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glCheckError();

	m_densityShader->Use();
	UpdateUniformsD();
	glBindVertexArray(m_vaoD);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, LAYERS);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_3D, 0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glCheckError();

	glBindFramebuffer(GL_FRAMEBUFFER, m_fboN);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glReadBuffer(GL_NONE);

	glViewport(0, 0, WIDTH, DEPTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glCheckError();

	m_normalShader->Use();
	UpdateUniformsN();
	glBindVertexArray(m_vaoD);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, LAYERS);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_3D, 0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glCheckError();
}

void ProcedualGenerator::GenerateMcVbo()
{
	glCheckError();

	m_vertexCount = m_cubesPerDimension.x * m_cubesPerDimension.z;
	glm::vec2* vertices = new glm::vec2[m_vertexCount];

	glm::vec2 pos = glm::vec2(-1);
	int index = 0;
	for (int x = 0; x < m_cubesPerDimension.x - 1; ++x)
	{
		pos.y = -1.0f;
		for (int z = 0; z < m_cubesPerDimension.z - 1; ++z)
		{
			vertices[index] = pos;
			++index;
			pos.y += m_mcResolution.z;
		}
		pos.x += m_mcResolution.x;
	}

	glBindVertexArray(m_vaoMc);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboMc);
			glBufferData(GL_ARRAY_BUFFER, m_vertexCount * sizeof(glm::vec2), vertices, GL_STATIC_DRAW);
			glCheckError();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glCheckError();

	delete[] vertices;

	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, m_tboMc);
		glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, (GetVertexCountTf() * 5 * (3 * sizeof(glm::vec3) * 3)) / m_mcMesh.GetVaoCount(), nullptr, GL_DYNAMIC_COPY);
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
	glCheckError();
}

TriplanarMesh* ProcedualGenerator::GenerateMesh()
{
	m_marchingCubeShader->Use();
	UpdateUniformsMc();

	// Create query object to collect info
	GLuint queryTF;
	glGenQueries(1, &queryTF);

	// Perform feedback transform
	glEnable(GL_RASTERIZER_DISCARD);

	int layerPerVao = m_cubesPerDimension.y / m_mcMesh.GetVaoCount();
	GLuint sumTriCount = 0;
	GLuint triCount;

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_tboMc);
	for (int vao = 0; vao < m_mcMesh.GetVaoCount(); ++vao)
	{
		GLuint layerLocation = glGetUniformLocation(m_marchingCubeShader->Program, "layerStart");
		glUniform1i(layerLocation, vao * layerPerVao);
		glCheckError();

		glBindVertexArray(m_vaoMc);
	
		glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, queryTF);
			glBeginTransformFeedback(GL_TRIANGLES);
				glDrawArraysInstanced(GL_POINTS, 0, GetVertexCountMc(), layerPerVao);
			glEndTransformFeedback();
		glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

		glBindVertexArray(0);

		glGetQueryObjectuiv(queryTF, GL_QUERY_RESULT, &triCount);
		sumTriCount += triCount;

		glBindVertexArray(m_mcMesh.GetVAO(vao));
			glBindBuffer(GL_ARRAY_BUFFER, m_mcMesh.GetVBO(vao));
				glBufferData(GL_ARRAY_BUFFER, triCount * 3 * (sizeof(glm::vec3) * 3), nullptr, GL_STATIC_COPY);
				glCheckError();
				glCopyBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, GL_ARRAY_BUFFER, 0, 0, triCount * 3 * (sizeof(glm::vec3) * 3));
			glCheckError();
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		m_mcMesh.UpdateVao(vao, triCount);
	}
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
	glDisable(GL_RASTERIZER_DISCARD);

	glFlush();

	glDeleteQueries(1, &queryTF);

	return &m_mcMesh;
}

GLuint ProcedualGenerator::GetVertexCountMc() const
{
	return m_vertexCount;
}

GLuint ProcedualGenerator::GetVertexCountTf() const
{
	return m_vertexCount * m_cubesPerDimension.y;
}

const Texture& ProcedualGenerator::GetDensityTexture() const
{
	return m_densityTex;
}

const Texture& ProcedualGenerator::GetNormalTexture() const
{
	return m_normalTex;
}

void ProcedualGenerator::SetRandomSeed(int seed)
{
	m_random.seed(seed);

	m_pillars[0] = Randoms{ m_randomRand(m_random), ToSignBit(m_random()), m_randomFloat(m_random) };
	m_pillars[1] = Randoms{ m_randomRand(m_random), ToSignBit(m_random()), m_randomFloat(m_random) };
	m_pillars[2] = Randoms{ m_randomRand(m_random), ToSignBit(m_random()), m_randomFloat(m_random) };
	m_pillars[3] = Randoms{ m_randomRand(m_random), ToSignBit(m_random()), m_randomFloat(m_random) };

	m_helix = Randoms{ m_randomRand(m_random), ToSignBit(m_random()), m_randomFloat(m_random) };

	m_shelf = Randoms{ m_randomRand(m_random), ToSignBit(m_random()), m_randomFloat(m_random) };

	m_noise = new Noise[4]
	{
		Noise(glm::toMat4(MakeQuat(m_randomAngle(m_random), m_randomAngle(m_random), m_randomAngle(m_random))), NoiseTexture(16, 16, 16, 1)),
		Noise(glm::toMat4(MakeQuat(m_randomAngle(m_random), m_randomAngle(m_random), m_randomAngle(m_random))), NoiseTexture(16, 16, 16, 2)),
		Noise(glm::toMat4(MakeQuat(m_randomAngle(m_random), m_randomAngle(m_random), m_randomAngle(m_random))), NoiseTexture(16, 16, 16, 3)),
		Noise(glm::toMat4(MakeQuat(m_randomAngle(m_random), m_randomAngle(m_random), m_randomAngle(m_random))), NoiseTexture(16, 16, 16, 4))
	};
}

void ProcedualGenerator::SetStartLayer(int layer)
{
	m_layerCorrection = layer;
}

void ProcedualGenerator::SetResolution(glm::ivec3 cubesPerDimension)
{
	m_cubesPerDimension = cubesPerDimension;
	m_mcResolution = 2.0f / glm::vec3(cubesPerDimension);
}

void ProcedualGenerator::SetNoiseScale(float scale)
{
	m_noiseScale = scale;
}

void ProcedualGenerator::SetGeometryScale(glm::vec3 scale)
{
	m_geometryScale = scale;
	m_mcMesh.SetScale(scale);
}

const glm::vec3 ProcedualGenerator::GetGeometryScale() const
{
	return m_geometryScale;
}

void ProcedualGenerator::SetIsoLevel(float isoLevel)
{
	m_isoLevel = isoLevel;
}

void ProcedualGenerator::UpdateUniformsMc()
{
	m_lookupTable.UpdateUniforms(*m_marchingCubeShader);

	GLint resolutioneLoc = glGetUniformLocation(m_marchingCubeShader->Program, "resolution");
	glUniform3fv(resolutioneLoc, 1, glm::value_ptr(m_mcResolution));
	glCheckError();

	GLuint layerLocation = glGetUniformLocation(m_marchingCubeShader->Program, "layerCorrection");
	glUniform1i(layerLocation, m_layerCorrection * m_cubesPerDimension.y / LAYERS);
	glCheckError();

	GLuint noiseScaleLocation = glGetUniformLocation(m_marchingCubeShader->Program, "noiseScale");
	glUniform1f(noiseScaleLocation, m_noiseScale);
	glCheckError();

	GLuint textureRepeatLocation = glGetUniformLocation(m_marchingCubeShader->Program, "textureRepeat");
	glUniform3fv(textureRepeatLocation, 1, glm::value_ptr(glm::vec3(WIDTH / 8, LAYERS / 8, DEPTH / 8)));
	glCheckError();

	for (int i = 0; i < 4; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		GLuint textureId = m_noise[i].texture.GetId();
		GLint textureLoc = glGetUniformLocation(m_marchingCubeShader->Program, ("noise[" + std::to_string(i) + "].tex").c_str());
		glUniform1i(textureLoc, i);
		glBindTexture(GL_TEXTURE_3D, textureId);
		glCheckError();

		glm::mat4 rot = m_noise[i].rotation;
		GLuint rotLocation = glGetUniformLocation(m_marchingCubeShader->Program, ("noise[" + std::to_string(i) + "].rotation").c_str());
		glUniformMatrix4fv(rotLocation, 1, GL_FALSE, glm::value_ptr(rot));
		glCheckError();
	}

	glActiveTexture(GL_TEXTURE4);
	GLint textureLoc = glGetUniformLocation(m_marchingCubeShader->Program, "densityTex");
	glUniform1i(textureLoc, 4);
	glBindTexture(GL_TEXTURE_3D, m_densityTex.GetId());
	glCheckError();

	GLint isoLevelLoc = glGetUniformLocation(m_marchingCubeShader->Program, "isoLevel");
	glUniform1i(isoLevelLoc, m_isoLevel);
	glCheckError();

	GLuint scaleLocation = glGetUniformLocation(m_marchingCubeShader->Program, "scale");
	glUniform3fv(scaleLocation, 1, glm::value_ptr(m_geometryScale));
	glCheckError();
}

void ProcedualGenerator::UpdateUniformsD()
{
	GLuint resLocation = glGetUniformLocation(m_densityShader->Program, "resolution");
	glUniform3f(resLocation, WIDTH, LAYERS, DEPTH);
	glCheckError();

	GLuint layerLocation = glGetUniformLocation(m_densityShader->Program, "startLayer");
	glUniform1i(layerLocation, m_layerCorrection);
	glCheckError();

	for (int i = 0; i < 4; ++i)
	{
		GLuint posLocation = glGetUniformLocation(m_densityShader->Program, ("pillars[" + std::to_string(i) + "].offset").c_str());
		glUniform1f(posLocation, m_pillars[i].offset);
		glCheckError();

		GLuint freqLocation = glGetUniformLocation(m_densityShader->Program, ("pillars[" + std::to_string(i) + "].frequence").c_str());
		glUniform1f(freqLocation, m_pillars[i].frequence);
		glCheckError();

		GLuint signLocation = glGetUniformLocation(m_densityShader->Program, ("pillars[" + std::to_string(i) + "].frequenceSign").c_str());
		glUniform1i(signLocation, m_pillars[i].frequenceSign);
		glCheckError();
	}

	{
		GLuint offsetLocation = glGetUniformLocation(m_densityShader->Program, "helix.offset");
		glUniform1f(offsetLocation, m_helix.offset);
		glCheckError();

		GLuint freqLocation = glGetUniformLocation(m_densityShader->Program, "helix.frequence");
		glUniform1f(freqLocation, m_helix.frequence);
		glCheckError();

		GLuint signLocation = glGetUniformLocation(m_densityShader->Program, "helix.frequenceSign");
		glUniform1i(signLocation, m_helix.frequenceSign);
		glCheckError();
	}

	{
		GLuint offsetLocation = glGetUniformLocation(m_densityShader->Program, "shelf.offset");
		glUniform1f(offsetLocation, m_shelf.offset);
		glCheckError();

		GLuint freqLocation = glGetUniformLocation(m_densityShader->Program, "shelf.frequence");
		glUniform1f(freqLocation, m_shelf.frequence);
		glCheckError();

		GLuint signLocation = glGetUniformLocation(m_densityShader->Program, "shelf.frequenceSign");
		glUniform1i(signLocation, m_shelf.frequenceSign);
		glCheckError();
	}
}

void ProcedualGenerator::UpdateUniformsN()
{
	GLuint resLocation = glGetUniformLocation(m_normalShader->Program, "resolution");
	glUniform3f(resLocation, 2.0f / WIDTH, 2.0f / LAYERS, 2.0f / DEPTH);
	glCheckError();

	glActiveTexture(GL_TEXTURE0);
	GLint textureLoc = glGetUniformLocation(m_normalShader->Program, "densityTex");
	glUniform1i(textureLoc, 0);
	glBindTexture(GL_TEXTURE_3D, m_densityTex.GetId());
	glCheckError();
}

float ProcedualGenerator::NormalizeCoord(int coord, int dim)
{
	return 2.0f * (coord * 1.0f / dim - 0.5f);
}

int ProcedualGenerator::ToSignBit(int random)
{
	return random & 1 ? 1 : -1;
}
