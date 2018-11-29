#define STB_IMAGE_IMPLEMENTATION
#define _CRT_SECURE_NO_DEPRECATE

#include<iostream>

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

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
Mouse mouse(SCR_WIDTH/2, SCR_HEIGHT/2);

bool draw_wireframe = false;

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
*/
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

/**
Convertes texture to an image (NOTE: hardocded to GL_COLOR_ATTACHMENT0 at the moment) */
void texture_to_image(bool flip_image);

/**
Set's up frame buffer for rendering to texture */
GLuint get_texture_framebuffer(GLuint& tex_color_buffer);

/**
*/
void set_buffers_for_quad_render(GLuint & quadVAO, GLuint & quadVBO);

/******************************\
	Function Definitions
\******************************/

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
	screen_shader.use();
	screen_shader.setInt("screenTexture", 0);

	//Configure mouse attributes
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	//Preparing models to render
	GLuint quadVAO, quadVBO;
	set_buffers_for_quad_render(quadVAO, quadVBO);
	Model nano_model("D:\\Bjarni\\Projects\\3Dmodels\\nanosuit\\nanosuit.obj");	

	//Preparing texture
	GLuint tex_color_buffer;
	glGenTextures(1, &tex_color_buffer);
	GLuint texture_framebuffer = get_texture_framebuffer(tex_color_buffer);

	//Rendering loop
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;		

		process_input(window, camera);
		camera.update(mouse.pitch, mouse.yaw);

		//Setup for render to texture
		glBindFramebuffer(GL_FRAMEBUFFER, texture_framebuffer);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader_program.use();

		//Setting up transformation matrices
		glm::mat4 view = glm::lookAt(
			camera.position,
			camera.position + camera.front,
			camera.up);
		shader_program.setMat4("view", view);

		glm::mat4 project = glm::perspective(
			(float) glm::radians(mouse.fov),
			(float)SCR_WIDTH / (float)SCR_HEIGHT,
			0.1f, 100.0f);
		shader_program.setMat4("project", project);

		// render the loaded model
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
		shader_program.setMat4("model", model);
		
		nano_model.draw(shader_program);
		
		//Setup for render to window
		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		screen_shader.use();

		//Render a single quad
		if(draw_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glBindVertexArray(quadVAO);		
		glBindTexture(GL_TEXTURE_2D, tex_color_buffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//Save current state of the frame buffer texture
	texture_to_image(true);

	glfwTerminate();
	return 0;
}

void texture_to_image(bool flip_image)
{
	FILE *output_image;
	int output_width = SCR_WIDTH;
	int output_height = SCR_HEIGHT;
	std::vector<unsigned char>	pxls(output_width * output_height * 3);

	glReadBuffer(GL_COLOR_ATTACHMENT0);	
	//glReadBuffer(GL_DEPTH_ATTACHMENT);
	glReadPixels(0, 0, output_width, output_height, GL_RGB, GL_UNSIGNED_BYTE, &pxls[0]);

	output_image = fopen("D:\\Bjarni\\Projects\\3Dmodels\\output.ppm", "wt");
	fprintf(output_image, "P3\n");
	fprintf(output_image, "# Created by brgud\n");
	fprintf(output_image, "%d %d\n", output_width, output_height);
	fprintf(output_image, "255\n");

	std::vector<unsigned char> flipped(output_width * output_height * 3);
	for (int i = 0; i < output_height; i++)
	{
		memcpy(&flipped[i * output_width * 3],						// address of destination
			&pxls[(output_height - i - 1) * output_width * 3],		// address of source
			output_width * 3 * sizeof(unsigned char));				// number of bytes to copy
	}

	int k = 0;
	for (int i = 0; i<output_width; i++)
	{
		for (int j = 0; j<output_height; j++)
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

GLuint get_texture_framebuffer(GLuint& tex_color_buffer)
{
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	
	// create a color attachment texture
	glGenTextures(1, &tex_color_buffer);
	glBindTexture(GL_TEXTURE_2D, tex_color_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_color_buffer, 0);
	
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);	
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	
	//TODO: attach an additional buffer here, will keep Z z^, alpha, and ambient occlusion
	//The depth values can be altered in the vertex buffer, unclear about the others
	//to begin with we can change the color one to include alpha, then compute alpha in vertex and save it as depth in fragment

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	return framebuffer;
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
