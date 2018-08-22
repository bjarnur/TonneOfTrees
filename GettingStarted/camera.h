#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
	public:
		
		Camera(glm::vec3 camera_position, glm::vec3 camera_target, glm::vec3 world_up);
		
		/**
		Update cameras coordinate system */
		void update(float pitch, float yaw);

		glm::vec3 up;
		glm::vec3 position;
		glm::vec3 front;
		glm::vec3 world_up;
		glm::vec3 target;
		glm::vec3 right;
};
