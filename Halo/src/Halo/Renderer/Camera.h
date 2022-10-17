#pragma once

#include <glm/glm.hpp>

namespace Halo {

	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projectionMatrix);

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }

	private:
		glm::quat GetOrientation();
		glm::mat4 m_ProjectionMatrix, m_ViewMatrix;
		glm::vec3 m_Position, m_Rotation, m_FocalPoint;

		float m_Pitch, m_Yaw;
	};
}