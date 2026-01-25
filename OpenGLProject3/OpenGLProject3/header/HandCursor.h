#ifndef HANDCURSOR_H
#define HANDCURSOR_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <Shader.h>
#include <Camera.h>

#include <thread>
#include <atomic>
#include <cstdio>

class HandCursor {
private:
	std::vector<float> vertices;
	int segments;
	float radius = 1.0f;
	int vertexCount;

	unsigned int circleVAO, circleVBO;

	Shader* circleShader;
	float screenWidth;
	float screenHeight;

public:
	int x;
	int y;
	float l;
	float prevl;
	int prevX;
	int prevY;
	bool handExist;

	// For reading hand control data
	std::atomic<int> inputTypeCode; // Store input types as integer code
	std::atomic<int> handX;
	std::atomic<int> handY;
	std::atomic<float> length;
	std::atomic<bool> handDataReady;

	// Constructor
	HandCursor(Shader* shader, float screenW, float screenH, int seg = 30)
		: circleShader(shader),
		screenWidth(screenW),
		screenHeight(screenH),
		segments(seg),
		radius(0.1f),
		vertexCount(0),
		prevX(screenW / 2),
		prevY(screenH / 2),
		prevl(0),
		handExist(false),
		inputTypeCode(0),
		handX(0),
		handY(0),
		length(0),
		handDataReady(false) {
		
		// Create unit circle at (0,0)
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);

		for (int i = 0; i <= segments; i++) {
			float angle = 2.0f * 3.14159f * i / segments;
			vertices.push_back(radius * cos(angle));
			vertices.push_back(radius * sin(angle));
		}

		vertexCount = segments + 2;

		// Set up VAO VBO
		glGenVertexArrays(1, &circleVAO);
		glGenBuffers(1, &circleVBO);
		glBindVertexArray(circleVAO);
		glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Start python thread
		std::thread handThread(&HandCursor::readHandData, this);
		handThread.detach();
	}

	
	void readHandData() {
		FILE* pipe = _popen("python C:\\Users\\namfa\\OneDrive\\Desktop\\Coding\\OpenGL01\\OpenGLProject3\\HandDetection1\\HandDetection1\\HandController.py", "r");

		if (!pipe) {
			std::cerr << "Failed to start Python script!" << std::endl;
			return;
		}

		char buffer[256];
		while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
			int code, x, y;
			float l;
			if (sscanf_s(buffer, "%d %d %d %f", &code, &x, &y, &l) == 4) {
				l = roundf(l * 10.0f) / 10.0f;

				inputTypeCode.store(code);
				handX.store(x);
				handY.store(y);
				length.store(l);
				handDataReady.store(true);
			}
		}
		_pclose(pipe);
		
	}

	void updatePosition() {
		if (handDataReady.load()) {
			handExist = true;
			int method = inputTypeCode.load();
			x = handX.load();
			y = handY.load();
			l = length.load();

			const char* methodName = "unknown";
			if (method == 1) methodName = "position";
			else if (method == 2) methodName = "startHold";
			else if (method == 3) methodName = "holding";
			else if (method == 4) methodName = "endHold";
			else if (method == 5) methodName = "clicking";

			// For debugging
			std::cout << "Hand: " << methodName << ", " << x << ", " << y << ", " << l << std::endl;

			draw(x, y);

			prevX = x;
			prevY = y;

			handDataReady.store(false);
		}
		else {
			draw(prevX, prevY);
		}
	}

	void draw(float x, float y, float pixelRadius = 25.0f, glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f)) {
		circleShader->use();

		glUniform2f(glGetUniformLocation(circleShader->ID, "position"), float(x), float(y));
		glUniform2f(glGetUniformLocation(circleShader->ID, "screenSize"), 1280.0f, 720.0f);
		glUniform3f(glGetUniformLocation(circleShader->ID, "color"), 1.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(circleShader->ID, "radius"), 50.0f);

		glBindVertexArray(circleVAO);
		glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
		glBindVertexArray(0);
	}

	~HandCursor() {
		// Cleanup
		glDeleteVertexArrays(1, &circleVAO);
		glDeleteBuffers(1, &circleVBO);
	}
	

	void processInput(GLFWwindow* window, float deltaTime, Camera* camera) {
		float cameraSpeed = 15.0f * deltaTime;

		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && l != prevl) {
			float lengthDiff = l - prevl;
			std::cout << "Zoom: lengthDiff = " << lengthDiff << std::endl;  // ✅ Debug

			// ✅ Fixed: positive = spreading = zoom out, negative = pinching = zoom in
			if (lengthDiff > 0.5f) {
				camera->cameraPos -= cameraSpeed * camera->cameraFront * 3.0f;  // Zoom out
				prevl = l;
			}
			else if (lengthDiff < -0.5f) {
				camera->cameraPos += cameraSpeed * camera->cameraFront * 3.0f;  // Zoom in
				prevl = l;
			}
			
		}
			
	}

};


#endif
