#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/animator.h>
#include <learnopengl/model_animation.h>
#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 2.0f, 8.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

glm::vec3 cameraOffset = glm::vec3(0.0f, 3.0f, 6.0f); // Height of 3, Distance of 6

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// [NEW] Character State Variables
glm::vec3 characterPos = glm::vec3(0.0f, 0.0f, 0.0f);
float characterRotation = 0.0f;
float moveSpeed = 3.0f;
bool isMoving = false;

// [NEW] Simple Floor Data
float floorVertices[] = {
	// positions          // normals           // texture coords
	 10.0f, -0.4f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 0.0f,
	-10.0f, -0.4f,  10.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
	-10.0f, -0.4f, -10.0f,  0.0f, 1.0f, 0.0f,  0.0f, 10.0f,

	 10.0f, -0.4f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 0.0f,
	-10.0f, -0.4f, -10.0f,  0.0f, 1.0f, 0.0f,  0.0f, 10.0f,
	 10.0f, -0.4f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
};

Animation* currentAnimation = nullptr;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader ourShader("anim_model.vs", "anim_model.fs");
	Shader floorShader("floor.vs", "floor.fs"); // Create a simple shader for the floor
	// Floor VAO Setup
	unsigned int floorVAO, floorVBO;
	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &floorVBO);
	glBindVertexArray(floorVAO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), &floorVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0); // pos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1); // normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2); // tex
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	// load models
	// -----------
	Model ourModel(FileSystem::getPath("resources/objects/animation/Ch03_nonPBR.dae"));
	Animation moveAnimation(FileSystem::getPath("resources/objects/animation/walk.dae"), &ourModel);
	Animation standingAnimation(FileSystem::getPath("resources/objects/animation/standing.dae"), &ourModel);
	Animator animator(&moveAnimation);

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// select animation based on movement state
		Animation* targetAnimation = isMoving ? &moveAnimation : &standingAnimation;

		// Only trigger PlayAnimation if we are switching to a DIFFERENT animation
		if (currentAnimation != targetAnimation) {
			currentAnimation = targetAnimation;
			animator.PlayAnimation(currentAnimation);
		}

		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);
		animator.UpdateAnimation(deltaTime);


		camera.Position = characterPos - (camera.Front * 6.0f) + glm::vec3(0.0f, 2.5f, 0.0f);

		// Update the View Matrix
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		// 2. Make the camera look at the character's torso/head
		glm::mat4 view = glm::lookAt(camera.Position, characterPos + glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		// render
		// ------
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. Setup Lighting Uniforms (Since your floor.fs requires them)
		glm::vec3 lightPos(2.0f, 4.0f, 2.0f);

		// 2. Render Floor
		floorShader.use();
		floorShader.setMat4("projection", projection);
		floorShader.setMat4("view", view);
		floorShader.setMat4("model", glm::mat4(1.0f));
		floorShader.setVec3("lightPos", lightPos);
		floorShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		glBindVertexArray(floorVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// don't forget to enable shader before setting uniforms
		ourShader.use();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);
		auto transforms = animator.GetFinalBoneMatrices();
		for (int i = 0; i < transforms.size(); ++i)
			ourShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, characterPos); // Move based on input
		model = glm::rotate(model, glm::radians(characterRotation), glm::vec3(0, 1, 0));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		ourShader.setMat4("model", model);
		ourModel.Draw(ourShader);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float speed = moveSpeed * deltaTime;

	// Get the camera's forward and right vectors (flattened to the XZ plane)
	glm::vec3 forward = camera.Front;
	forward.y = 0.0f;
	forward = glm::normalize(forward);

	glm::vec3 right = camera.Right;
	right.y = 0.0f;
	right = glm::normalize(right);

	glm::vec3 direction(0.0f);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) direction += forward;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) direction -= forward;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) direction -= right;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) direction += right;

	if (glm::length(direction) > 0.0f) {
		direction = glm::normalize(direction);
		characterPos += direction * speed;

		float targetAngle = glm::degrees(atan2(direction.x, direction.z));
		characterRotation = targetAngle;
		isMoving = true;
	}
	else {
		isMoving = false;
	}
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; 

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
