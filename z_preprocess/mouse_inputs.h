#ifndef MOUSE_H
#define MOUSE_H

class Mouse
{
	public:

		Mouse(float center_x, float center_y) : 
			first_mouse(true), lastX(center_x), lastY(center_y),
			pitch(0.0), yaw(-90.f), fov(45.0) {}


		void update_mouse(double xpos, double ypos)
		{
			if (first_mouse)
			{
				lastX = xpos;
				lastY = ypos;
				first_mouse = false;
			}

			float x_offset = xpos - lastX;
			float y_offset = lastY - ypos;
			lastX = xpos;
			lastY = ypos;

			float sensitivity = 0.05f;
			x_offset *= sensitivity;
			y_offset *= sensitivity;

			yaw += x_offset;
			pitch += y_offset;

			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;
		}

		void update_scroll(double yoffset)
		{
			if (fov >= 1.0f && fov <= 45.0f)
				fov -= yoffset;
			if (fov <= 1.0f)
				fov = 1.0f;
			if (fov >= 45.0f)
				fov = 45.0f;
		}

		bool first_mouse;
		float lastX;
		float lastY;
		float pitch;
		float yaw;
		double fov;
};

#endif // !MOUSE_H