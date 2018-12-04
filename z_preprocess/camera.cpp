#include <glm/glm.hpp>

#include "camera.h"

Camera::Camera(glm::vec3 camera_position, glm::vec3 camera_target, glm::vec3 world_up)
		:position(camera_position), target(camera_target), world_up(world_up), 
		front(glm::vec3(0.0, 0.0, -1.0f)) 
{
	glm::vec3 direction = glm::normalize(camera_target - camera_position);
	right = glm::normalize(glm::cross(world_up, direction));
	up = glm::cross(direction, right);
	front = direction;
}

void Camera::update(float pitch, float yaw)
{
	glm::vec3 front_temp;
	front_temp.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front_temp.y = sin(glm::radians(pitch));
	front_temp.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

	front = glm::normalize(front_temp);
	right = glm::normalize(glm::cross(front, world_up));
	up = glm::normalize(glm::cross(right, front));
}
