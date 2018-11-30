#ifndef MESH_H
#define MESH_H


#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "shader.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 tex_coords;
};

struct Texture
{
	unsigned int ID;
	std::string type;
	std::string path;
};

class Mesh {

	public:
		
		/*  Mesh Data  */
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
		
		/*  Functions  */
		Mesh(std::vector<Vertex> vertices, 
			 std::vector<unsigned int> indices, 
			 std::vector<Texture> textures);

		void draw(Shader & shader);

		void get_extreme_points(glm::vec3 & max_coords, glm::vec3 & min_coords);

	private:

		/*  Render data  */
		unsigned int VAO, VBO, EBO;

		/*  Functions    */
		void setup_mesh();
};

#endif // !MESH_H