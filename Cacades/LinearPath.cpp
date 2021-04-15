#include "LinearPath.h"
#include <algorithm>

LinearPath::LinearPath(std::vector<ControlPoint>& controlPoints, GLuint duration) : LinearPath(controlPoints, duration)
{
}

LinearPath::LinearPath(std::vector<ControlPoint>& controlPoints, GLuint duration, bool evenOutSpeed) : BasePath(controlPoints, duration), m_forward(true)
{
	if (evenOutSpeed)
		CalculateTimeSteps();

	CalculateApprox();
}

LinearPath::~LinearPath()
{
}

void LinearPath::Update(GLfloat deltaTime)
{
	if (m_forward)
	{
		m_timer += deltaTime;

		if (m_timer >= m_duration)
		{
			m_timer = m_duration;
			m_forward = false;
		}
	}
	else
	{
		m_timer -= deltaTime;

		if (m_timer <= 0)
		{
			m_timer = 0;
			m_forward = true;
		}
	}

	m_position = GetPosByTime(m_timer);
	m_rotation = GetRotByTime(m_timer);
}

glm::vec3 LinearPath::CatmullRomSpline(const std::vector<ControlPoint>& cp, float t) const
{
	// indices of the relevant control points
	int i0 = (int)std::max(0.0f, t - 1);
	int i1 = t;
	int i2 = (int)std::min(m_controlCount - 1.0f, t + 1);
	int i3 = (int)std::min(m_controlCount - 1.0f, t + 2);

	// parameter on the local curve interval
	float local_t = glm::fract(t);

	return BasePath::CatmullRomSpline(cp[i0], cp[i1], cp[i2], cp[i3], local_t);
}

glm::quat LinearPath::Squad(const std::vector<ControlPoint>& cp, float t) const
{
	// indices of the relevant control points
	int i0 = (int)(t - 1 + m_controlCount) % m_controlCount;
	int i1 = t;
	int i2 = (int)(t + 1) % m_controlCount;
	int i3 = (int)(t + 2) % m_controlCount;

	// parameter on the local curve interval
	float local_t = glm::fract(t);

	return BasePath::Squad(cp[i0], cp[i1], cp[i2], cp[i3], local_t);
}

void LinearPath::CalculateTimeSteps()
{
	GLfloat pathLength = 0;
	std::vector<glm::vec3> vectors = std::vector<glm::vec3>();
	for (int i = 0; i < m_controlCount - 1; i++)
	{
		// V0 = P1 - P0
		vectors.push_back(m_controlPoints[i + 1].Position - m_controlPoints[i].Position);
		pathLength += glm::length(vectors[i]);
	}

	GLfloat velocity = pathLength / m_duration;

	m_controlPoints[0].TimeStamp = 0.0f;
	for (int i = 1; i < m_controlCount - 1; i++)
	{
		// t1 = t0 + (s0 / v)
		m_controlPoints[i].TimeStamp = m_controlPoints[i - 1].TimeStamp + (glm::length(vectors[i - 1]) / velocity);
	}
	m_controlPoints[m_controlCount - 1].TimeStamp = m_duration;
}
