#define STB_IMAGE_IMPLEMENTATION
#define _CRT_SECURE_NO_DEPRECATE

#define _USE_MATH_DEFINES

#include<iostream>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"

#include "camera.h"
#include "model.h"
#include "mouse_inputs.h"



/*************************\
	Global variables
\*************************/

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const unsigned int TEX_WIDTH = 256;
const unsigned int TEX_HEIGHT = 256;


float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
Mouse mouse(SCR_WIDTH/2, SCR_HEIGHT/2);

float config_swap_timer = 0.f;
bool draw_wireframe = false;
bool draw_sample_views = false;
bool draw_sample_rays = false;

/****************************\
	Function declarations
\****************************/


/**
Configures scene and initializes window */
GLFWwindow * initialize_window();

/**
Initializes and configures texture, returns texture handle */
unsigned int load_texture(const char * file_path, bool use_alpha);

/**
Handles input from user, should be called for each frame */
void process_input(GLFWwindow * window, Camera & camera);

/**
Callback resizes viewport based on new window size */
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

/**
Callback function for capturing input from mouse */
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

/**
Called by framework when mouse wheel is scrolled */
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

/**
Convertes texture to an image (NOTE: hardocded to GL_COLOR_ATTACHMENT0 at the moment) */
void texture_to_image(bool flip_image, int img_num, GLuint & fb);

/**
Set's up frame buffer for rendering to texture */
void get_texture_framebuffer(GLuint & texture_framebuffer, GLuint& tex_color_buffer);

/**
Prepare a screen-coverin quad */
void set_buffers_for_quad_render(GLuint & quadVAO, GLuint & quadVBO);

/**
Prepare uniformally distrubeted points on the models bounding sphere */
glm::vec3 * generate_viewpoints_on_sphere(Model model, int num_samples, GLuint & VAO, GLuint & VBO, GLuint & lineVAO, GLuint & lineVBO);

void rays_from_point_to_center(GLuint & VAO, GLuint & VBO, glm::vec3 * points, const glm::vec3 & center, int num_samples);

/**
*/
void setup_matrices(Camera camera, glm::mat4 & view, glm::mat4 & proj, glm::mat4 & model);

void draw_model(Model nano_model, Shader shader_program, const glm::mat4 & view, 
				const glm::mat4 & proj, const glm::mat4 & model);

void draw_sample_views_func(GLuint sphereVAO, Shader sphere_shader, const glm::mat4 & view, 
						const glm::mat4 & proj, const glm::mat4 & model);

void draw_sample_ray_func(GLuint VAO, Shader shader, const glm::mat4 & view_mtx,
						const glm::mat4 & proj_mtx, const glm::mat4 & model_mtx);

void sample_from_points(GLuint framebuffer, Shader shader, Model model, 
						glm::vec3 * points, glm::vec3 model_center, int num_samples);


/******************************\
	Function Definitions
\******************************/


int main()
{
	GLFWwindow * window = initialize_window();
	if (!window)
	{
		return -1;
	}

	//Setting up carmera
	Camera camera(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	
	//Configure shader program used by OpenGL
	Shader shader_program = Shader("shaders\\vertex_shader.txt", "shaders\\fragment_shader.txt");
	Shader screen_shader = Shader("shaders\\tex_vertex_shader.txt", "shaders\\tex_fragment_shader.txt");
	Shader sphere_shader = Shader("shaders\\sphere_vertex_shader.txt", "shaders\\sphere_fragment_shader.txt");

	//Configure mouse attributes
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	//Preparing model to render
	float model_radius;
	glm::vec3 model_center;	
	Model nano_model("D:\\Bjarni\\Projects\\3Dmodels\\nanosuit\\nanosuit.obj");
	nano_model.get_bounding_sphere(model_center, model_radius);
	
	//Preparing other geometry
	GLuint quadVAO, quadVBO, sphereVAO, sphereVBO, lineVAO, lineVBO;
	set_buffers_for_quad_render(quadVAO, quadVBO);
	glm::vec3 * points = generate_viewpoints_on_sphere(nano_model, 10, sphereVAO, sphereVBO, lineVAO, lineVBO);	

	//Preparing texture framebuffer
	GLuint texture_framebuffer, tex_color_buffer;
	glGenTextures(1, &tex_color_buffer);
	get_texture_framebuffer(texture_framebuffer, tex_color_buffer);

	//Generate sample images
	sample_from_points(texture_framebuffer, shader_program, nano_model, points, model_center, 10);

	//Rendering loop
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;		
		config_swap_timer += deltaTime;

		process_input(window, camera);
		camera.update(mouse.pitch, mouse.yaw);

		//Setup for render to window
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		glm::mat4 view;
		glm::mat4 proj;
		glm::mat4 model;		
		setup_matrices(camera, view, proj, model);

		draw_model(nano_model, shader_program, view, proj, model);

		if(draw_sample_views)
			draw_sample_views_func(sphereVAO, sphere_shader, view, proj, model);
		if (draw_sample_rays)
			draw_sample_ray_func(lineVAO, sphere_shader, view, proj, model);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void sample_from_points(GLuint framebuffer, Shader shader, Model model, 
						glm::vec3 * points, glm::vec3 model_center, int num_samples)
{	
	for (int i = 0; i < num_samples; i++)
	{		
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.5f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, TEX_WIDTH, TEX_HEIGHT);

		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::mat4 camera_transform_mtx;
		glm::mat4 view_mtx;
		glm::mat4 proj_mtx;
		glm::mat4 model_mtx;		
		
		//Transform camera locations 		
		camera_transform_mtx = glm::translate(model_mtx, glm::vec3(0.0f, -1.75f, 0.0f));
		camera_transform_mtx = glm::scale(model_mtx, glm::vec3(0.2f, 0.2f, 0.2f));
		glm::vec3 camera_pos = camera_transform_mtx * glm::vec4(points[i], 1.0);
		glm::vec3 camera_target = camera_transform_mtx * glm::vec4(model_center, 1.0);
		glm::vec3 camera_direction = glm::normalize(camera_pos - camera_target);
		
		//Render model to the framebuffer
		//Camera transformed_camera(camera_pos + camera_direction, camera_direction, up);		
		Camera transformed_camera(camera_pos, camera_direction, up);
		setup_matrices(transformed_camera, view_mtx, proj_mtx, model_mtx);		
		draw_model(model, shader, view_mtx, proj_mtx, model_mtx);

		texture_to_image(true, i, framebuffer);
	}
}

void draw_sample_views_func(GLuint VAO, Shader shader, const glm::mat4 & view_mtx,
						const glm::mat4 & proj_mtx, const glm::mat4 & model_mtx)
{
	shader.use();
	shader.setMat4("view", view_mtx);
	shader.setMat4("project", proj_mtx);
	shader.setMat4("model", model_mtx);

	glPointSize(5);
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, 10);
}

void draw_sample_ray_func(GLuint VAO, Shader shader, const glm::mat4 & view_mtx,
	const glm::mat4 & proj_mtx, const glm::mat4 & model_mtx)
{
	shader.use();
	shader.setMat4("view", view_mtx);
	shader.setMat4("project", proj_mtx);
	shader.setMat4("model", model_mtx);

	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, 2000);
}

void setup_matrices(Camera camera, glm::mat4 & view_mtx, glm::mat4 & proj_mtx, glm::mat4 & model_mtx)
{
	view_mtx = glm::lookAt(
		camera.position,
		camera.position + camera.front,
		camera.up);

	proj_mtx = glm::perspective(	
		(float)glm::radians(mouse.fov),
		(float)SCR_WIDTH / (float)SCR_HEIGHT,
		0.1f, 100.0f);

	//translate model down so it's at the center of the scene
	model_mtx = glm::translate(model_mtx, glm::vec3(0.0f, -1.75f, 0.0f)); 
	// it's a bit too big for our scene, so scale it down
	model_mtx = glm::scale(model_mtx, glm::vec3(0.2f, 0.2f, 0.2f));	
}

void draw_model(Model model, Shader shader, const glm::mat4 & view_mtx, 
				const glm::mat4 & proj_mtx, const glm::mat4 & model_mtx)
{
	shader.use();
	shader.setMat4("view", view_mtx);
	shader.setMat4("project", proj_mtx);
	shader.setMat4("model", model_mtx);

	model.draw(shader);
}

void texture_to_image(bool flip_image, int img_num, GLuint & dfb)
{
	FILE *output_image;
	std::vector<unsigned char>	pxls(TEX_WIDTH * TEX_HEIGHT * 3);	
	
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(0, 0, TEX_WIDTH, TEX_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, &pxls[0]);

	std::string file_name = "D:\\Bjarni\\Projects\\3Dmodels\\output " + std::to_string(img_num) + ".ppm";
	output_image = fopen(file_name.c_str(), "wt");
	fprintf(output_image, "P3\n");
	fprintf(output_image, "# Created by brgud\n");
	fprintf(output_image, "%d %d\n", TEX_WIDTH, TEX_HEIGHT);
	fprintf(output_image, "255\n");

	std::vector<unsigned char> flipped(TEX_WIDTH * TEX_HEIGHT * 3);
	for (int i = 0; i < TEX_HEIGHT; i++)
	{
		memcpy(&flipped[i * TEX_WIDTH * 3],						// address of destination
			&pxls[(TEX_HEIGHT - i - 1) * TEX_WIDTH * 3],		// address of source
			TEX_WIDTH * 3 * sizeof(unsigned char));				// number of bytes to copy
	}

	int k = 0;
	for (int i = 0; i<TEX_WIDTH; i++)
	{
		for (int j = 0; j<TEX_HEIGHT; j++)
		{
			if (flip_image)
			{
				fprintf(output_image, "%u %u %u ", 
						(unsigned int)flipped[k], 
						(unsigned int)flipped[k + 1],
						(unsigned int)flipped[k + 2]);
			}			
			else
			{
				fprintf(output_image, "%u %u %u ",
					(unsigned int)pxls[k],
					(unsigned int)pxls[k + 1],
					(unsigned int)pxls[k + 2]);
			}
			k = k + 3;
		}
		fprintf(output_image, "\n");
	}
	fclose(output_image);
}

void get_texture_framebuffer(GLuint & texture_framebuffer, GLuint& tex_color_buffer)
{
	glGenFramebuffers(1, &texture_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, texture_framebuffer);
	
	// create a color attachment texture
	glGenTextures(1, &tex_color_buffer);
	glBindTexture(GL_TEXTURE_2D, tex_color_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_color_buffer, 0);
	
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);	
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 256, 256); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void rays_from_point_to_center(GLuint & VAO, GLuint & VBO, glm::vec3 * points, const glm::vec3 & center, int num_samples)
{
	glm::vec3 * lines = new glm::vec3[num_samples * 2];
	for (int i = 0; i < num_samples; i++)
	{
		lines[(i * 2) + 0] = points[i];
		lines[(i * 2) + 1] = center;
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, num_samples * 2 * sizeof(glm::vec3), &(*lines), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glBindVertexArray(0);
}

glm::vec3 * generate_viewpoints_on_sphere(Model model, int num_samples, GLuint & VAO, GLuint & VBO, GLuint & lineVAO, GLuint & lineVBO)
{	
	float radius;
	glm::vec3 center;
	model.get_bounding_sphere(center, radius);

	float offset = 2. / num_samples;
	float increment = M_PI * (3. - glm::sqrt(5.));

	glm::vec3 * points = new glm::vec3[num_samples];
	for (int i = 0; i < num_samples; i++)
	{
		float y = ((i * offset) - 1) * (3. - sqrt(5.));
		float r = sqrt(1 - pow(y, 2));
		float phi = ((i + 1) % num_samples) * increment;

		float x = cos(phi) * r;
		float z = sin(phi) * r;
		points[i] = center + glm::vec3(x, y, z) * radius * 2.5f;
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, num_samples * sizeof(glm::vec3), &(*points), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	//Generate a visualization of the view-rays
	rays_from_point_to_center(lineVAO, lineVBO, points, center, num_samples);

	return points;
}

void set_buffers_for_quad_render(GLuint & quadVAO, GLuint & quadVBO)
{
	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
							 // positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);
}

GLFWwindow * initialize_window()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Establishing a window and setting current context
	GLFWwindow * window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	//Initializign GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return nullptr;
	}

	//Default size of rendering window
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	//Registering callback functions
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	
	// During init, enable debug output
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);


	return window;
}

unsigned int load_texture(const char * file_path, bool use_alpha)
{
	unsigned char * data;
	unsigned int texture;
	int width, height, nrChannels;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load and generate the texture
	data = stbi_load(file_path, &width, &height, &nrChannels, 0);
	if (data)
	{
		if(use_alpha)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture " << file_path << std::endl;
	}
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

void process_input(GLFWwindow * window, Camera & camera)
{
	float cameraSpeed = 2.5f * deltaTime; // adjust accordingly

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);	

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.position += cameraSpeed * camera.front;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.position -= cameraSpeed * camera.front;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.position += glm::normalize(glm::cross(camera.front, camera.up)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		draw_wireframe = true;
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE)
		draw_wireframe = false;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && config_swap_timer > 0.2f)
	{
		draw_sample_views = !draw_sample_views;
		config_swap_timer = 0.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && config_swap_timer > 0.f)
	{
		draw_sample_rays = !draw_sample_rays;
		config_swap_timer = 0.0f;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	mouse.update_mouse(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	mouse.update_scroll(yoffset);
}
