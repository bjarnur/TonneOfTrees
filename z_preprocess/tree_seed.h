#ifndef TREE_SEED_H
#define TREE_SEED_H

#include <glm/glm.hpp>

#include "shader.h"

//Hardcoded plane used for camera facing proxy
static const GLfloat quad_vertex_buffer_data[] = 
{
	//Positions			//Texture coords
	-1.3f, -1.0f, 0.0f,	0.0f, 0.0f,
	1.3f, -1.0f, 0.0f,	1.0f, 0.0f,
	-1.3f, 1.6f, 0.0f,	0.0f, 1.0f,

	1.3f, 1.6f, 0.0f,	1.0f, 1.0f,
	1.3f, -1.0f, 0.0f,	1.0f, 0.0f,
	-1.3f, 1.6f, 0.0f,	0.0f, 1.0f
};

struct Sample
{
	//Texture id
	GLuint id;
	
	//Defines sample plane (centered at origin)
	glm::vec3 normal;
	glm::vec3 up;

	//Angular distance from current view
	float distance;


	Sample() : distance(FLT_MAX) {}
};

class Seed
{
	public:
		//Functions
		Seed(glm::vec3 position, glm::vec3 rotation);
		
		void Seed::draw(Shader shader, Sample * neighborSamples, const glm::mat4 & view_mtx, const glm::mat4 & proj_mtx);

		//Variables
		glm::vec3 position;
		glm::vec3 rotation;

	private:

		//Functions
		void setup_mesh();
		glm::mat4 Seed::get_model_mtx();
		void normalize_weights(Sample * neighborSamples);

		//Variables
		GLuint VAO;
		GLuint VBO;
};

#endif // !TREE_SEED_H



