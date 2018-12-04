#include <vector>

#include "mesh.h"


Mesh::Mesh(	std::vector<Vertex> vertices,
			std::vector<unsigned int> indices,
			std::vector<Texture> textures)
	: vertices(vertices), indices(indices), textures(textures) 
{
	setup_mesh();
}

void Mesh::get_extreme_points(glm::vec3 & max_coords, glm::vec3 & min_coords)
{	
	for (Vertex v : vertices)
	{	
		if (v.position.x < min_coords.x)
			min_coords.x = v.position.x;
		if (v.position.y < min_coords.y)
			min_coords.y = v.position.y;
		if (v.position.z < min_coords.z)
			min_coords.z = v.position.z;

		if (v.position.x > max_coords.x)
			max_coords.x = v.position.x;
		if (v.position.y > max_coords.y)
			max_coords.y = v.position.y;
		if (v.position.z > max_coords.z)
			max_coords.z = v.position.z;
	}
}

void Mesh::draw(Shader & shader)
{
	unsigned int diffuse_nr = 1;
	unsigned int specular_nr = 1;

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);

		std::string number;
		std::string name = textures[i].type;

		if (name == "texture_diffuse")
			number = std::to_string(diffuse_nr++);
		else if (name == "texture_specular")
			number = std::to_string(specular_nr++);

		//shader.setFloat(("material." + name + number).c_str(), i);
		shader.setFloat((name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].ID);			
	}
	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::setup_mesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));

	glBindVertexArray(0);
}