#ifndef TREE_SEED_H
#define TREE_SEED_H

#include <glm/glm.hpp>

#include "shader.h"

//Hardcoded plane used for camera facing proxy
static const GLfloat quad_vertex_buffer_data[] = 
{
	-1.f, -2.0f, 0.0f,
	1.0f, -2.0f, 0.0f,
	-1.0f, 1.6f, 0.0f,

	1.0f, 1.6f, 0.0f,
	1.0f, -2.0f, 0.0f,
	-1.0f, 1.6f, 0.0f,
};

class Seed
{
	public:
		//Functions
		Seed(glm::vec3 position, glm::vec3 rotation);
		void draw(Shader shader, const glm::mat4 & view_mtx, const glm::mat4 & proj_mtx);

		//Variables

	private:

		//Functions
		void setup_mesh();
		glm::mat4 Seed::get_model_mtx();		

		//Variables
		GLuint VAO;
		GLuint VBO;
		glm::vec3 position;
		glm::vec3 rotation;
};

#endif // !TREE_SEED_H



