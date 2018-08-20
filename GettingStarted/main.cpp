#include<iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"


/*************************\
	Shape Definitions
\*************************/

float triangle[] = {
	//Positions         //Colors
	0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
	-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
	0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
};

float triangle1[] = {
	-0.5f, -0.5f, 0.0f,
	0.f, -0.5f, 0.0f,
	-0.25f,  0.f, 0.0f
};

float triangle2[] = {
	0.f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.25f,  0.f, 0.0f
};

float vertices[] = {
	//Positions
	0.5f,  0.5f, 0.0f,  // top right
	0.5f, -0.5f, 0.0f,  // bottom right
	-0.5f, -0.5f, 0.0f,  // bottom left
	-0.5f,  0.5f, 0.0f   // top left 
};

unsigned int indices[] = {  // note that we start from 0!
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
};

/****************************\
	Function declarations
\****************************/


/**
Configures scene and initializes window */
GLFWwindow * initialize_window();

/**
*/
void process_input(GLFWwindow * window);


/**
Callback resizes viewport based on new window size */
void framebuffer_size_callback(GLFWwindow* window, int width, int height);


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
	
	//Configure shader program used by OpenGL
	Shader shader_program = Shader("vertex_shader.txt", "fragment_shader.txt");
	shader_program.use();

	//Vertex Array Object (persists subsequent Vertex Attribute)
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	//Vertex Buffer Object (where we leave our vertices so that the GPU can find them)
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	//Used when drawing based on indices
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	
	//Bind the VertexArrayObject, subsequent vertex buffers and vertex attribute
	//configureations will be bound with this VAO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
	
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Rendering loop
	while (!glfwWindowShouldClose(window))
	{
		process_input(window);

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		//Set color and shader program
		//float time_value = glfwGetTime();
		//float green_value = (sin(time_value) / 2.0f) + 0.5f;
		//int vertex_location = glGetUniformLocation(shader_program, "our_color");		
		//glUniform4f(vertex_location, 0.0f, green_value, 0.0f, 1.0f);		

		//Draw triangle
		glBindVertexArray(VAO);				
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}


GLFWwindow * initialize_window()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Establishing a window and setting current context
	GLFWwindow * window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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
	glViewport(0, 0, 800, 600);

	//Registering callback functions
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	return window;
}

void process_input(GLFWwindow * window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}