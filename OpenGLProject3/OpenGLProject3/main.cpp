#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <Sphere.h>
#include <Shader.h>
#include <Camera.h>
#include <Planets.h>
#include <HandCursor.h>


// Read python output


// Register callback function to adjust viewport on window resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

// Speed
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame


int main() {
	// Configure GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFW window
	GLFWwindow* window = glfwCreateWindow(1280, 720, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Viewport
	glViewport(0, 0, 1280, 720);

	// Create camera
	Camera camera;
	// Store camera pointer for callback functions
	Camera* ptrCamera = &camera;
	glfwSetWindowUserPointer(window, ptrCamera);
	// Register callback functions
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, [](GLFWwindow* w, double x, double y) {
		static_cast<Camera*>(glfwGetWindowUserPointer(w))->mouse_callback(w, x, y);
		});

	glfwSetScrollCallback(window, [](GLFWwindow* w, double x, double y) {
		static_cast<Camera*>(glfwGetWindowUserPointer(w))->scroll_callback(w, x, y);
		});

	glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int btn, int act, int mods) {
		static_cast<Camera*>(glfwGetWindowUserPointer(w))->mouse_button_callback(w, btn, act, mods);
		});


	// Hide cursor
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	
	
	// Setting up sphere-----------------------------------------------------------------

	Planet sun("Sun");
	Planet mercury("Mercury");
	Planet venus("Venus");
	Planet earth("Earth");
	Planet mars("Mars");
	Planet jupiter("Jupiter");
	Planet saturn("Saturn");
	Planet uranus("Uranus");
	Planet neptune("Neptune");

	std::vector<Planet*> allPlanets;
	allPlanets.push_back(&sun);
	allPlanets.push_back(&mercury);
	allPlanets.push_back(&venus);
	allPlanets.push_back(&earth);
	allPlanets.push_back(&mars);
	allPlanets.push_back(&jupiter);
	allPlanets.push_back(&saturn);
	allPlanets.push_back(&uranus);
	allPlanets.push_back(&neptune);
	


	
	Shader ourShader("vertex.vert", "fragment.frag");
	Shader circleShader("vertCir.vert", "fragCir.frag");
	
	// Get uniform locations
	int modelLoc = glGetUniformLocation(ourShader.ID, "model");
	int viewLoc = glGetUniformLocation(ourShader.ID, "view");
	int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



	HandCursor handCursor(&circleShader, 1280.0f, 720.0f);



	lastFrame = glfwGetTime();  // Initialize lastFrame before loop starts
	// Render loop
	while (!glfwWindowShouldClose(window)) {

		// Get time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		


		// Input
		camera.processInput(window, deltaTime);

		// Rendering commands here
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Draw the cube
		ourShader.use();

		// Model matrix
		glm::mat4 model = glm::mat4(1.0f);
		
		// View matrix - update with camera position and orientation
		glm::mat4 view = glm::lookAt(camera.cameraPos, camera.cameraPos + camera.cameraFront, camera.cameraUp);
		// Projection matrix - update with fov from scroll
		//g;m::perspective(FOV, aspect ratio, near plane, far plane)
		glm::mat4 projection = glm::perspective(glm::radians(camera.fov), 1280.0f / 720.0f, 0.1f, 1000.0f);

		// Upload matrices to shader
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


		static int frameCount = 0;
		/*if (frameCount++ % 60 == 0) {  // Print every 60 frames
			glm::vec3 pos = earth.getPosition();
			std::cout << "Earth: (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
		}*/

		for (Planet* planet : allPlanets) {
			planet->draw(ourShader, allPlanets, deltaTime);
		}


		// For drawing overlay on camera view
		// Disable depth test
		glDisable(GL_DEPTH_TEST);

		circleShader.use();

		// Draw circle
		
		handCursor.updatePosition();
		handCursor.processInput(window, deltaTime, &camera);

		// Re-enable depth test
		glEnable(GL_DEPTH_TEST);


		// ...

		// Check for any events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();		
	}

	
	

	glfwTerminate();
	return 0;
}
