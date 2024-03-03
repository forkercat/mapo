//
// Created by Junhao Wang (@forkercat) on 3/2/24.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main()
{
	std::cout << "Hello, World!" << std::endl;

	GLFWwindow* window;

	if (!glfwInit())
	{
		return -1;
	}

	window = glfwCreateWindow(800, 600, "Hello World", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Cannot be null.");
	LOG("hello! %d", 123123213);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}
