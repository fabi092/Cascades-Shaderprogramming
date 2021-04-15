#pragma once
#include <GL/glew.h>
#include <glm/detail/type_vec2.hpp>
#include <glm/detail/type_vec3.hpp>
#include <random>
#include "NoiseTexture.h"
#include <glm/mat4x4.hpp>
#include <glm/gtx/quaternion.hpp>
#include "TriplanarMesh.h"
#include "GpuLookupTable.h"

class Shader;

struct Randoms
{
	float offset;
	int frequenceSign;
	float frequence;
};

namespace glm
{
	inline vec2 rotate(vec2 v, float a)
	{
		float s = sin(a);
		float c = cos(a);
		mat2 m = mat2(c, -s, s, c);
		return m * v;
	}
}

class ProcedualGenerator
{
	struct Noise
	{
		Noise(const glm::quat& rot, const NoiseTexture& texture) : rotation(glm::toMat4(rot)), texture(texture) {}
		glm::mat4 rotation;
		NoiseTexture texture;
	};

public:
	ProcedualGenerator();
	~ProcedualGenerator();
	void Generate3dTexture();

	void GenerateMcVbo();
	TriplanarMesh* GenerateMesh();

	GLuint GetVertexCountMc() const;
	GLuint GetVertexCountTf() const;

	const Texture& GetDensityTexture() const;
	const Texture& GetNormalTexture() const;

	void SetRandomSeed(int seed);
	void SetStartLayer(int layer);
	void SetResolution(glm::ivec3 cubesPerDimension);
	void SetNoiseScale(float scale);
	void SetIsoLevel(float isoLevel);

	void SetGeometryScale(glm::vec3 scale);
	const glm::vec3 GetGeometryScale() const;

	static const int WIDTH = 96, DEPTH = 96, LAYERS = 256;

protected:
	void SetupMC();
	void SetupDensity();

	void UpdateUniformsMc();
	void UpdateUniformsD();
	void UpdateUniformsN();

	static float NormalizeCoord(int coord, int dim);
	static int ToSignBit(int random);

	Randoms m_pillars[4];
	Randoms m_helix;
	Randoms m_shelf;

	Noise* m_noise;

	Texture m_densityTex, m_normalTex;
	GLuint m_vaoMc = 0, m_vboMc = 0, m_tboMc = 0, m_vaoD = 0, m_vboD = 0, m_fboD = 0, m_fboN;

	GLuint m_vertexCount = 0;

	glm::vec3 m_mcResolution;
	glm::vec3 m_geometryScale;
	glm::ivec3 m_cubesPerDimension;
	int m_layerCorrection;
	float m_noiseScale;
	float m_isoLevel;

	Shader* m_marchingCubeShader, *m_densityShader, *m_normalShader;
	GpuLookupTable m_lookupTable;

	TriplanarMesh m_mcMesh;

	std::default_random_engine m_random;
	std::uniform_int_distribution<int> m_randomAngle;
	std::uniform_real_distribution<float> m_randomRand;
	std::uniform_real_distribution<float> m_randomFloat;
};

