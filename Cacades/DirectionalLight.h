#pragma once
#include "Light.h"
class DirectionalLight : public Light
{
public:
	DirectionalLight(glm::vec3 position, glm::vec3 color, Shader& shadowShader, GLfloat farPlane, GLfloat nearPlane = 0.1f);
	virtual ~DirectionalLight();

	void UpdateUniforms(const Shader& shader, int lightIndex, int textureIndex) override;

	void PreRender() const override;
	void RenderDebug(Shader& shader) const override;

protected:
	DirectionalLight(glm::vec3 position, glm::quat orientation, glm::vec3 color, Shader& shadowShader, GLfloat farPlane, GLfloat nearPlane = 0.1f);
	virtual glm::mat4 GetProjection() const;
	virtual glm::mat4 GetView() const;
	glm::mat4 GetShadowMatrix() const;
};

