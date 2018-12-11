
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
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

void Seed::draw(Shader shader, GLuint * textures, float * distances, const glm::mat4 & view_mtx, const glm::mat4 & proj_mtx)
{
	//Normalize weights
	std::cout << "w1: " << distances[0] << " , w2: " << distances[1] << ", w3: " << distances[2] << std::endl;
	float sum = 0.0f;
	for (int i = 0; i < 3; i++)
	{
		sum += distances[i];
	}
	for (int i = 0; i < 3; i++)
	{
		distances[i] = sum - distances[i];
	}
	sum = 0.0f;
	for (int i = 0; i < 3; i++)
	{
		sum += distances[i];
	}
	for (int i = 0; i < 3; i++)
	{
		distances[i] /= sum;
	}

	shader.use();
	shader.setMat4("view", view_mtx);
	shader.setMat4("project", proj_mtx);
	shader.setMat4("model", get_model_mtx());
	shader.setVec3("weights", glm::vec3(distances[0], distances[1], distances[2]));
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textures[2]);

	shader.setInt("screenTexture1", 0); // or with shader class
	shader.setInt("screenTexture2", 1); // or with shader class
	shader.setInt("screenTexture3", 2); // or with shader class

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}