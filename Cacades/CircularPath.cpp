//#include "CircularPath.h"
//#include <vector>
//#include <glm/gtc/type_ptr.hpp>
//#include "Shader.h"
//#include "Box.h"
//
//CircularPath::CircularPath(std::vector<ControlPoint>& controlPoints, GLuint duration) : CircularPath(controlPoints, duration, false)
//{}
//
//CircularPath::CircularPath(std::vector<ControlPoint>& controlPoints, GLuint duration, bool evenOutSpeed) : BasePath(controlPoints, duration)
//{
//	if (evenOutSpeed)
//		CalculateTimeSteps();
//	Circularise();
//	CalculateApprox();
//}
//
//CircularPath::~CircularPath()
//{
//}
//
//glm::vec3 CircularPath::CatmullRomSpline(const std::vector<ControlPoint>& cp, float t) const
//{
//	// indices of the relevant control points
//	int i0 = (int)((t - 1) + m_controlCount) % m_controlCount;
//	int i1 = t;
//	int i2 = (int)(t + 1);
//	int i3 = (int)(t + 2);
//
//	// parameter on the local curve interval
//	float local_t = glm::fract(t);
//
//	return BasePath::CatmullRomSpline(cp[i0], cp[i1], cp[i2], cp[i3], local_t);
//}
//
//glm::quat CircularPath::Squad(const std::vector<ControlPoint>& cp, float t) const
//{
//	// indices of the relevant control points
//	int i0 = (int)(t - 1 + m_controlCount) % m_controlCount;
//	int i1 = t;
//	int i2 = (int)(t + 1) % m_controlCount;
//	int i3 = (int)(t + 2) % m_controlCount;
//
//	// parameter on the local curve interval
//	float local_t = glm::fract(t);
//
//	return BasePath::Squad(cp[i0], cp[i1], cp[i2], cp[i3], local_t);
//}
//
//void CircularPath::Circularise()
//{
//	ControlPoint p0 = m_controlPoints[0];
//	p0.TimeStamp += m_duration;
//	m_controlPoints.push_back(p0);
//
//	ControlPoint p1 = m_controlPoints[1];
//	p1.TimeStamp += m_duration;
//	m_controlPoints.push_back(p1);
//}
//
//void CircularPath::CalculateTimeSteps()
//{
//	GLfloat pathLength = 0;
//	std::vector<glm::vec3> vectors = std::vector<glm::vec3>();
//	for (int i = 0; i < m_controlCount - 1; i++)
//	{
//		// V0 = P1 - P0
//		vectors.push_back(m_controlPoints[i + 1].Position - m_controlPoints[i].Position);
//		pathLength += glm::length(vectors[i]);
//	}
//	vectors.push_back(m_controlPoints[0].Position - m_controlPoints[m_controlCount - 1].Position);
//	pathLength += glm::length(vectors[vectors.size() - 1]);
//
//	GLfloat velocity = pathLength / m_duration;
//
//	m_controlPoints[0].TimeStamp = 0.0f;
//	for (int i = 1; i < m_controlCount; i++)
//	{
//		// t1 = t0 + (s0 / v)
//		m_controlPoints[i].TimeStamp = m_controlPoints[i - 1].TimeStamp + (glm::length(vectors[i - 1]) / velocity);
//	}
//}
