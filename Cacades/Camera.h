#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "BaseObject.h"

class Shader;

enum CameraMode
{
	Overview,
	Manual,
	Automatic
};

enum CameraType
{
	FPS,
	Free
};

class Camera : public BaseObject
{
public:
	Camera();
	~Camera();

	void Update(GLfloat deltaTime) override;

	void Move(glm::vec3 direction, GLfloat speed) override;

	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const;

	void SetRenderPath(bool value);
	bool GetRenderPath() const;

	CameraMode GetMode() const
	{
		return m_mode;
	}

	void SetMode(CameraMode mode)
	{
		m_mode = mode;
	}

	bool CursorPosCallback(float x, float y) override;
protected:
	CameraMode m_mode;
	CameraType m_type;
	bool m_renderPath;
	glm::vec2 m_mousePos;
};

