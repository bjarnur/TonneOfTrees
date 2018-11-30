#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"

class Model
{
	public:
		/*  Functions   */
		Model(char *path);
		void draw(Shader & shader);
		void get_bounding_sphere(glm::vec3 & center, float & radius);
	private:
		/*  Model Data  */
		std::string directory;
		std::vector<Mesh> meshes;		
		std::vector<Texture> textures_loaded;
		/*  Functions   */
		void load_model(std::string path);
		void process_node(aiNode *node, const aiScene *scene);
		Mesh process_mesh(aiMesh *mesh, const aiScene *scene);
		std::vector<Texture> load_material_textures(
			aiMaterial *mat, 
			aiTextureType type,
			std::string typeName);
		unsigned int texture_from_file(const char * name, std::string path);
};

#endif // !MODEL_H
