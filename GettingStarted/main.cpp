#include<iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


/****************************\
	Shader Implementations
\****************************/

const char *vertex_shader_source = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"void main()\n"
	"{\n"
	"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\0";

const char * fragmet_shader_source = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
	"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\0";



/*************************\
	Shape Definitions
\*************************/

float triangle[] = {
	-0.5f, -0.5f, 0.0f,
	0.f, -0.5f, 0.0f,
	-0.25f,  0.f, 0.0f,

	0.f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.25f,  0.f, 0.0f
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
*/
unsigned int initialize_shader_program();

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
	unsigned int shader_program = initialize_shader_program();

	//Vertex Array Object (persists subsequent Vertex Attribute)
	unsigned int VAO;
	unsigned int VAO2;
	glGenVertexArrays(1, &VAO);
	glGenVertexArrays(1, &VAO2);

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle1), triangle1, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	//Now bind attributes for the second triangle
	glBindVertexArray(VAO2);
	unsigned int VBO2;
	glGenBuffers(1, &VBO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle2), triangle2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Used to draw triangles based on indices
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Rendering loop
	while (!glfwWindowShouldClose(window))
	{
		process_input(window);

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader_program);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//Draw triangle 1
		glBindVertexArray(VAO);				
		glDrawArrays(GL_TRIANGLES, 0, 3);
	
		//Draw triangle 2
		glBindVertexArray(VAO2);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//Draw triagnles based on indices
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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

unsigned int initialize_shader_program()
{
	int success;
	char infoLog[512];

	unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragmet_shader_source, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	return shader_program;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}