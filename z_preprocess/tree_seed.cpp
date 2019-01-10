
#include "tree_seed.h"

#include <glm/gtc/matrix_transform.hpp>

Seed::Seed(glm::vec3 position, glm::vec3 rotation)
	: position(position), rotation(rotation)
{
	setup_mesh();
}

/**
Draw a camera facing proxy based on nearest neighbor samples */
void Seed::draw(Shader shader, Sample * neighborSamples, const glm::mat4 & view_mtx, const glm::mat4 & proj_mtx)
{	
	normalize_weights(neighborSamples);

	shader.use();

	shader.setMat4("view", view_mtx);
	shader.setMat4("project", proj_mtx);
	shader.setMat4("model", get_model_mtx());

	shader.setVec3("weights", glm::vec3(neighborSamples[0].distance, 
										neighborSamples[1].distance, 
										neighborSamples[2].distance));

	shader.setVec3("up1", neighborSamples[0].up);
	shader.setVec3("up2", neighborSamples[1].up);
	shader.setVec3("up3", neighborSamples[2].up);

	shader.setVec3("normal1", neighborSamples[0].normal);
	shader.setVec3("normal2", neighborSamples[1].normal);
	shader.setVec3("normal3", neighborSamples[2].normal);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, neighborSamples[0].id);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, neighborSamples[1].id);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, neighborSamples[2].id);

	shader.setInt("screenTexture1", 0); 
	shader.setInt("screenTexture2", 1); 
	shader.setInt("screenTexture3", 2); 

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}


	/************************\
		Private functions
	\************************/

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
	return model_mtx;
}

/*
Converts neighbor distances to weights based on proximity */
void Seed::normalize_weights(Sample * neighborSamples)
{
	float sum = 0.0f;
	for (int i = 0; i < 3; i++)
	{
		sum += neighborSamples[i].distance;
	}
	for (int i = 0; i < 3; i++)
	{
		neighborSamples[i].distance = sum - neighborSamples[i].distance;
	}
	sum = 0.0f;
	for (int i = 0; i < 3; i++)
	{
		sum += neighborSamples[i].distance;
	}
	for (int i = 0; i < 3; i++)
	{
		neighborSamples[i].distance /= sum;
	}
}