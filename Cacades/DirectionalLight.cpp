#include "DirectionalLight.h"
#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include "Box.h"


DirectionalLight::DirectionalLight(glm::vec3 position, glm::vec3 color, Shader& shadowShader, GLfloat farPlane, GLfloat nearPlane) : DirectionalLight(position, glm::quat(), color, shadowShader, farPlane, nearPlane)
{
}

DirectionalLight::DirectionalLight(glm::vec3 position, glm::quat orientation, glm::vec3 color, Shader& shadowShader, GLfloat farPlane, GLfloat nearPlane) : Light(position, orientation, color, shadowShader, nearPlane, farPlane)
{
	glGenFramebuffers(1, &shadowMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

	textureType = GL_TEXTURE_2D;

	glGenTextures(1, &shadowMap);
	glBindTexture(textureType, shadowMap);

	glTexImage2D(textureType, 0, GL_RG32F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RG, GL_FLOAT, nullptr);
	glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(textureType, GL_TEXTURE_BORDER_COLOR, borderColor);
	glCheckError();

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureType, shadowMap, 0);
	glBindTexture(textureType, 0);
	glCheckError();
	
	glGenTextures(1, &depthAttachment);
	glBindTexture(textureType, depthAttachment);

	glTexImage2D(textureType, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glTexParameterfv(textureType, GL_TEXTURE_BORDER_COLOR, borderColor);
	glCheckError();

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthAttachment, 0);
	glBindTexture(textureType, 0);
	glCheckError();

	glCheckFrameBuffer();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glCheckError();

	m_debugCube = new Model(m_position, glm::quat(), Box::GetTris(glm::vec3(.1f, .1f, 1)), 12, m_color, NoNormals);
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::UpdateUniforms(const Shader& shader, int lightIndex, int textureIndex)
{
	Light::UpdateUniforms(shader, lightIndex, textureIndex);

	GLint lightSpaceMatrixLoc = glGetUniformLocation(shader.Program, ("Lights[" + std::to_string(lightIndex) + "].lightSpaceMatrix").c_str());
	glUniformMatrix4fv(lightSpaceMatrixLoc, 1, GL_FALSE, glm::value_ptr(GetShadowMatrix()));
	glCheckError();

	glActiveTexture(GL_TEXTURE0 + textureIndex);
	glBindTexture(textureType, shadowMap);
	GLuint depthMapPos = glGetUniformLocation(shader.Program, ("Lights[" + std::to_string(lightIndex) + "].depthMap").c_str());
	glUniform1i(depthMapPos, textureIndex);
	glCheckError();
}

void DirectionalLight::PreRender() const
{
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_shadowShader.Use();

	GLint lightSpaceMatrixLoc = glGetUniformLocation(m_shadowShader.Program, "lightSpaceMatrix");
	glUniformMatrix4fv(lightSpaceMatrixLoc, 1, GL_FALSE, glm::value_ptr(GetShadowMatrix()));
	glCheckError();
}

void DirectionalLight::RenderDebug(Shader& shader) const
{
	m_debugCube->SetPosition(m_position);
	m_debugCube->SetOrientation(glm::conjugate(glm::toQuat(GetView())));
	m_debugCube->Render(shader);
}

glm::mat4 DirectionalLight::GetProjection() const
{
	return glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, m_nearPlane, m_farPlane);
}

glm::mat4 DirectionalLight::GetView() const
{
	return glm::lookAt(m_position, glm::vec3(0), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 DirectionalLight::GetShadowMatrix() const
{
	return GetProjection() * GetView();
}
