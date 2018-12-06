
#include "tree_seed.h"

#include <glm/gtc/matrix_transform.hpp>

Seed::Seed(glm::vec3 position, glm::vec3 rotation)
	: position(position), rotation(rotation)
{
	setup_mesh();
}

void Seed::setup_mesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertex_buffer_data), quad_vertex_buffer_data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);
}

glm::mat4 Seed::get_model_mtx()
{
	glm::mat4 model_mtx;


	model_mtx = glm::translate(model_mtx, position);
	//TODO rotate
	//TODO scale

	return model_mtx;
}

void Seed::draw(Shader shader, const glm::mat4 & view_mtx, const glm::mat4 & proj_mtx)
{
	shader.use();
	shader.setMat4("view", view_mtx);
	shader.setMat4("project", proj_mtx);
	shader.setMat4("model", get_model_mtx());

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}