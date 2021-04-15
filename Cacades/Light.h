#pragma once
#include "BaseObject.h"
#include "Enums.h"

class Light : public BaseObject
{
public:
	Light(glm::vec3 position, glm::quat orientation, glm::vec3 color, Shader& shadowShader, int nearPlane, int farPlane);
	virtual ~Light();

	virtual void UpdateUniforms(const Shader& shader, int lightIndex, int textureIndex);

	virtual void PreRender() const = 0;
	virtual void PostRender() const;
	virtual void RenderDebug(Shader& shader) const = 0;
	
	Shader& GetShadowShader() const
	{
		return m_shadowShader;
	}

	void SetShadowMode(ShadowMode mode);
	ShadowMode GetShadowMode() const;

	void CastsShadows(bool value);
	bool CastsShadows() const;

		glm::vec3 GetColor() const;
protected:
	glm::vec3 m_color;
	GLuint shadowMap;
	GLuint shadowMapFBO;
	GLuint textureType;
	GLuint depthAttachment;
	const GLuint SHADOW_RES = 4;
	const GLuint SHADOW_WIDTH = 512 * SHADOW_RES, SHADOW_HEIGHT = 512 * SHADOW_RES;
	Shader& m_shadowShader;
	ShadowMode m_shadowMode;
	GLfloat m_farPlane;
	GLfloat m_nearPlane;
	GLboolean m_castShadow;

	BaseObject* m_debugCube;
};

