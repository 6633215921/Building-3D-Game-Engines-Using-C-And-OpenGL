#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include "Icosphere.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(10.0f, 10.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// 1. นิยามโครงสร้างคลื่น (วางไว้นอก loop หรือบนสุดของ main ก็ได้)
struct WaveParams {
    glm::vec2 direction; // ทิศทางที่คลื่นวิ่งไป
    float steepness;     // ความแหลมของยอดคลื่น (0.0 - 1.0)
    float wavelength;    // ความยาวคลื่น
    float speed;         // ความเร็ว
};

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

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("7.4.camera.vs", "7.4.camera.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    // Create Icosphere:
    Icosphere sphere(0.25f, 8, true);

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Upload Interleaved Data (Pos, Normal, TexCoord)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sphere.getInterleavedVertexSize(), sphere.getInterleavedVertices(), GL_STATIC_DRAW);

    // Upload Indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.getIndexSize(), sphere.getIndices(), GL_STATIC_DRAW);

    // Attributes (Stride is usually 32 bytes: 3+3+2 floats)
    int stride = 8 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // --- SETUP LINE RENDERING ---
    unsigned int lineVAO, lineVBO;
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);

    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    // เราจะส่งข้อมูลแค่ Position (vec3) เข้าไป
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // world space positions of our cubes
    std::vector<glm::vec3> cubePositions;
    float startX = -1.0f;
    float startY = -1.0f;
    float startZ = -1.0f;
    float offset = 1.0f;

    for (int x = 0; x < 20; ++x)
    {
        for (int y = 0; y < 1; ++y)
        {
            for (int z = 0; z < 20; ++z)
            {
                cubePositions.push_back(glm::vec3(startX + x * offset, startY + y * offset, startZ + z * offset));
            }
        }
    }



    // สร้างชุดคลื่นสัก 3-4 ลูก เพื่อทำ Superposition (การซ้อนทับ)
    std::vector<WaveParams> waves = {
        { { 1.0f,  0.1f },   0.35f,       20.0f,       0.80f },
        { { 0.5f,  1.0f },   0.30f,       15.0f,       1.0f },
        { {-0.3f,  0.8f },   0.25f,        8.0f,       1.20f }, 
        { { 0.8f, -0.4f },   0.20f,        4.0f,       1.50f }
    };

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float time = static_cast<float>(glfwGetTime());
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 


        // activate shader
        ourShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        // -------------------------------------------------------
        // STEP 1: คำนวณตำแหน่งคลื่นทั้งหมดก่อน (ยังไม่วาด)
        // -------------------------------------------------------
        std::vector<glm::vec3> currentFramePos; // เก็บตำแหน่งของเฟรมนี้
        currentFramePos.reserve(cubePositions.size());
        for (unsigned int i = 0; i < cubePositions.size(); i++)
        {
            glm::vec3 basePos = cubePositions[i];
            glm::vec3 offset(0.0f);
            // Gerstner Wave Calculation
            for (const auto& w : waves) {
                float k = 2.0f * 3.14159f / w.wavelength;
                float c = sqrt(9.8f / k) * w.speed;
                glm::vec2 d = glm::normalize(w.direction);
                float f = k * (glm::dot(d, glm::vec2(basePos.x, basePos.z)) - c * time);
                float a = w.steepness / k;
                offset.x += d.x * (a * cos(f));
                offset.y += a * sin(f);
                offset.z += d.y * (a * cos(f));
            }
            currentFramePos.push_back(basePos + offset);
        }

        // -------------------------------------------------------
        // STEP 2: วาด Sphere (ใช้ตำแหน่งที่เพิ่งคำนวณ)
        // -------------------------------------------------------
        glBindVertexArray(VAO);

        for (const auto& pos : currentFramePos)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            ourShader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, sphere.getIndexCount(), GL_UNSIGNED_INT, 0);
        }

        // -------------------------------------------------------
        // STEP 3: วาดเส้นเชื่อม (Lines)
        // -------------------------------------------------------
        std::vector<glm::vec3> lineVertices;
        int rows = 20; // ตาม loop x ของคุณ
        int cols = 20; // ตาม loop z ของคุณ (y มีแค่ 1 ชั้น)
        for (int x = 0; x < rows; ++x) {
            for (int z = 0; z < cols; ++z) {
                int index = x * cols + z; // สูตรแปลง 2D Grid เป็น 1D Array
                if (index >= currentFramePos.size()) continue;
                glm::vec3 p1 = currentFramePos[index];
                // 1. เชื่อมไปทางขวา (ถ้ายังไม่ตกขอบ)
                if (z < cols - 1) {
                    glm::vec3 p2 = currentFramePos[index + 1];
                    lineVertices.push_back(p1);
                    lineVertices.push_back(p2);
                }
                // 2. เชื่อมไปข้างล่าง (ถ้ายังไม่ตกขอบล่าง)
                if (x < rows - 1) {
                    glm::vec3 p3 = currentFramePos[index + cols];
                    lineVertices.push_back(p1);
                    lineVertices.push_back(p3);
                }
            }
        }

        // อัปเดตข้อมูลเส้นเข้า GPU
        glBindVertexArray(lineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
        glBufferData(GL_ARRAY_BUFFER, lineVertices.size() * sizeof(glm::vec3), lineVertices.data(), GL_DYNAMIC_DRAW);
        
        // ตั้งค่า Model Matrix ของเส้นให้เป็น Identity (เพราะพิกัดคำนวณมาเป็น World Space แล้ว)
        ourShader.setMat4("model", glm::mat4(1.0f));

        glDrawArrays(GL_LINES, 0, lineVertices.size());
        

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
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
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
