#include "Transform.h"

using namespace dae;

Transform Transform::operator+(const Transform& other)
{
	Transform Temp;
	Temp.SetPosition(this->m_position.x + other.m_position.x, this->m_position.y + other.m_position.y, this->m_position.z + other.m_position.z);

	return Temp;
}

Transform Transform::operator-(const Transform& other)
{
	Transform Temp;
	Temp.SetPosition(this->m_position.x - other.m_position.x, this->m_position.y - other.m_position.y, this->m_position.z - other.m_position.z);

	return Temp;
}

void Transform::SetPosition(const float x, const float y, const float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
}
