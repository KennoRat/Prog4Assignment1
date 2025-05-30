#pragma once
#include <glm.hpp>

namespace dae
{
	class Transform final
	{
	public:

		Transform operator+(const Transform& other);
		Transform operator-(const Transform& other);

		const glm::vec3& GetPosition() const { return m_position; }
		void SetPosition(float x, float y, float z);
	private:
		glm::vec3 m_position;
	};
}
