#pragma once
#include <GL/glew.h>

class Shader;

struct CellTriangles {
	int tris[16];
};

struct MC_EdgeTable
{
	int edgeTable[256];
};

struct MC_TrisTable
{
	CellTriangles triTable[256];
};

class GpuLookupTable
{
public:
	GpuLookupTable();
	~GpuLookupTable();
	void WriteLookupTablesToGpu();
	void UpdateUniforms(Shader& shader) const;

protected:
	GLuint m_edgeIndex;
	GLuint m_trisIndex;

	static const MC_EdgeTable m_edgeTable;
	static const MC_TrisTable m_trisTable;
};

