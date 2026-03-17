#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// --- โค้ดที่ยกมาจากไฟล์ Wave ---
struct WaveParams {
    glm::vec2 direction;
    float steepness;
    float wavelength;
    float speed;
};
std::vector<WaveParams> waves = {
    { { 1.0f,  0.1f },   0.35f,        20.0f,        0.80f },
    { { 0.5f,  1.0f },   0.30f,        15.0f,        1.0f },
    { {-0.3f,  0.8f },   0.25f,         8.0f,        1.20f },
    { { 0.8f, -0.4f },   0.20f,         4.0f,        1.50f }
};
// ------------------------


// Player state
glm::vec3 playerPos = glm::vec3(0.0f, 0.0f, 0.0f); // ตำแหน่งเริ่มต้นของตัวละคร
float playerYaw = 0.0f;                            // มุมการหันหน้าของตัวละคร
float playerSpeed = 5.0f;                          // ความเร็วในการเดิน
float playerRadius = 1.0f;                         // รัศมีการชนของตัวละคร
// -----------------------

// --- Rock Data ---
struct RockTransform {
    glm::vec3 position;
    float scale;
    float rotationAngle;
    float hitboxRadius; // เพิ่มความกว้างของวงกลมรับการชน
};
std::vector<RockTransform> rocks = {
    { glm::vec3(15.0f, -1.5f,  15.0f), 4.0f, -45.0f,  6.67f },
    { glm::vec3(10.0f, -1.5f, -15.0f), 3.0f,  90.0f,  5.0f },
    { glm::vec3(0.0f, -1.5f,  25.0f), 3.5f, 180.0f,  6.0f }
};
// -----------------

// --- Item Data ---
struct Item {
    glm::vec3 position;
    float hitboxRadius; // รัศมีการชนของไอเทม
    bool isCollected;   // สถานะว่าถูกเก็บไปหรือยัง
};

// กำหนดตำแหน่งของไอเทมในฉาก
std::vector<Item> items = {
    { glm::vec3(5.0f, 1.0f,  5.0f), 1.5f, false },
    { glm::vec3(-5.0f, 1.0f, 10.0f), 1.5f, false },
    { glm::vec3(12.0f, 1.0f, -5.0f), 1.5f, false }
};
// -----------------

// --- ฟังก์ชันคำนวณ Gerstner Wave ใน C++ (ถอดสูตรมาจาก Shader) ---
glm::vec3 CalculateGerstnerWave(glm::vec2 position, float time, const std::vector<WaveParams>& waveParams) {
    glm::vec3 displacement = glm::vec3(0.0f);
    const float PI = 3.14159265359f;
    for (const auto& wave : waveParams) {
        float k = 2.0f * PI / wave.wavelength;
        // สูตร f = k * (dot(d, p) - speed * t)
        float f = k * (glm::dot(wave.direction, position) - (wave.speed * time));
        float a = wave.steepness / k;
        // ใน Shader: offset.x += d.x * (a * cos(f))
        displacement.x += wave.direction.x * (a * cos(f));
        // ใน Shader: offset.y += a * sin(f)
        displacement.y += a * sin(f);
        // ใน Shader: offset.z += d.y * (a * cos(f))
        displacement.z += wave.direction.y * (a * cos(f));
    }
    return displacement;
}

// ฟังก์ชันเช็คการชนแบบ Bounding Sphere
bool CheckSphereCollision(glm::vec3 pos1, float radius1, glm::vec3 pos2, float radius2) {
    // หาระยะห่างระหว่างจุดศูนย์กลางทั้งสอง
    float distance = glm::length(pos1 - pos2);
    // ถ้าระยะห่างน้อยกว่ารัศมีรวมกัน แปลว่าชนกัน
    return distance < (radius1 + radius2);
}

// --- ฟังก์ชันเช็คการชนกับหิน (คืนค่า true ถ้าชน) ---
bool CheckRockCollision(glm::vec3 currentPos, float radius, const std::vector<RockTransform>& rockList) {
    for (const auto& rock : rockList) {
        if (CheckSphereCollision(currentPos, radius, rock.position, rock.hitboxRadius)) {
            return true; 
        }
    }
    return false;
}

// --- ฟังก์ชันเช็คและเก็บไอเทม ---
void CheckAndCollectItems(glm::vec3 currentPos, float radius, std::vector<Item>& itemList) {
    for (auto& item : itemList) {
        if (!item.isCollected && CheckSphereCollision(currentPos, radius, item.position, item.hitboxRadius)) {
            item.isCollected = true;
            std::cout << "Collected an item!" << std::endl;
        }
    }
}
// -------------------------------------------------------------

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
    //stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader modelShader("1.model_loading.vs", "1.model_loading.fs");
    Shader waveShader("7.4.camera.vs", "7.4.camera.fs"); // Shader ของคลื่น

    // load models
    // -----------
    Model ourModel(FileSystem::getPath("resources/objects/ship/Untitled.obj"));
    Model island(FileSystem::getPath("resources/objects/rock/rock.obj"));
    Model collectableItem(FileSystem::getPath("resources/objects/oil/Untitled.obj"));

    // --- Setup ข้อมูล VBO/VAO สำหรับคลื่น (ยกมาจากไฟล์ 1) ---
    unsigned int trianglesVAO, trianglesVBO;
    glGenVertexArrays(1, &trianglesVAO);
    glGenBuffers(1, &trianglesVBO);
    glBindVertexArray(trianglesVAO);
    glBindBuffer(GL_ARRAY_BUFFER, trianglesVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    std::vector<glm::vec3> cubePositions;
    float startX = -20.0f; 
    float startY = 0.0f;  
    float startZ = -20.0f;
    float offset = 0.50f;
    int gridSizeX = 120;
    int gridSizeY = 1;
    int gridSizeZ = 120;
    for (int x = 0; x < gridSizeX; ++x) {
        for (int y = 0; y < gridSizeY; ++y) {
            for (int z = 0; z < gridSizeZ; ++z) {
                cubePositions.push_back(glm::vec3(startX + x * offset, startY + y * offset, startZ + z * offset));
            }
        }
    }
    std::vector<glm::vec3> trianglesVertices = {};
    for (int x = 0; x < gridSizeX - 1; ++x) {
        for (int z = 0; z < gridSizeZ - 1; ++z) {
            int topLeft = x * gridSizeZ + z;
            int topRight = x * gridSizeZ + (z + 1);
            int bottomLeft = (x + 1) * gridSizeZ + z;
            int bottomRight = (x + 1) * gridSizeZ + (z + 1);
            trianglesVertices.push_back(cubePositions[topLeft]);
            trianglesVertices.push_back(cubePositions[topRight]);
            trianglesVertices.push_back(cubePositions[bottomLeft]);
            trianglesVertices.push_back(cubePositions[topRight]);
            trianglesVertices.push_back(cubePositions[bottomLeft]);
            trianglesVertices.push_back(cubePositions[bottomRight]);
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, trianglesVBO);
    glBufferData(GL_ARRAY_BUFFER, trianglesVertices.size() * sizeof(glm::vec3), trianglesVertices.data(), GL_STATIC_DRAW); // ใช้ GL_STATIC_DRAW ได้เลยเพราะจุดไม่เปลี่ยน
    // -----------------------------------------------------
    
 
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);


        // --- ทำให้ตัวละครลอยตามคลื่น ---
        // 1. หา Displacement ณ ตำแหน่ง XZ ปัจจุบันของนักเตะ
        glm::vec3 waveOffset = CalculateGerstnerWave(glm::vec2(playerPos.x, playerPos.z), currentFrame, waves);
        // 2. เก็บตำแหน่งเดิมไว้ก่อนถูกคลื่นพัด
        glm::vec3 oldPosBeforeWave = playerPos;
        // 3. อัปเดตตำแหน่งตามคลื่น
        float waterSurfaceY = 0.0f;
        playerPos.y = waterSurfaceY + waveOffset.y;
        playerPos.x += waveOffset.x * deltaTime;
        playerPos.z += waveOffset.z * deltaTime;
        // 4. เช็ค Collision อีกรอบ หลังจากโดนคลื่นพัด
        if (CheckRockCollision(playerPos, playerRadius, rocks)) {
            // ถ้าโดนพัดไปชนหิน ให้คืนค่าตำแหน่งเฉพาะแนวนอน (X, Z)
            playerPos.x = oldPosBeforeWave.x;
            playerPos.z = oldPosBeforeWave.z;
        }        
        // 5. เช็ค Collision อีกรอบ หลังจากโดนคลื่นพัด (Player-Items)
        CheckAndCollectItems(playerPos, playerRadius, items);

        // --- เพิ่มตรรกะให้กล้องตามตัวละคร (Camera Follow Logic) ---
        float cameraDistance = 16.0f;
        // จุดที่อยากให้กล้องโฟกัส (ชดเชยแกน Y ขึ้นมาประมาณ 1.5 หน่วย จะได้มองที่กลางลำตัว/หัว)
        glm::vec3 targetOffset = glm::vec3(0.0f, 1.5f, 0.0f);
        glm::vec3 cameraTarget = playerPos + targetOffset;
        // ตั้งตำแหน่งกล้องให้อยู่ข้างหลังเป้าหมาย (cameraTarget) ตามทิศทางที่กล้องกำลังมองอยู่
        camera.Position = cameraTarget - (camera.Front * cameraDistance);        
        // ---------------------------------------------------

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Matrix มุมมองที่ต้องใช้ร่วมกัน
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // -------------------------------------------------------
        // ส่วนที่ 1: วาด Map คลื่น
        // -------------------------------------------------------
        waveShader.use();
        waveShader.setMat4("projection", projection);
        waveShader.setMat4("view", view);
        waveShader.setMat4("model", glm::mat4(1.0f));
        waveShader.setFloat("time", currentFrame); 
        for (unsigned int i = 0; i < waves.size(); i++) {
            std::string name = "waves[" + std::to_string(i) + "].";
            waveShader.setVec2(name + "direction", waves[i].direction);
            waveShader.setFloat(name + "steepness", waves[i].steepness);
            waveShader.setFloat(name + "wavelength", waves[i].wavelength);
            waveShader.setFloat(name + "speed", waves[i].speed);
        }
        glBindVertexArray(trianglesVAO);

        waveShader.setBool("isWireframe", true); // วาดแบบลวดลายก่อน
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)trianglesVertices.size());

        waveShader.setBool("isWireframe", false); // วาดแบบทึบ
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)trianglesVertices.size());


        // -------------------------------------------------------
        // ส่วนที่ 2: วาดตัวละคร 
        // -------------------------------------------------------
        modelShader.use();
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, playerPos);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(playerYaw), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        modelShader.setMat4("model", model);
        ourModel.Draw(modelShader);

        // -------------------------------------------------------
        // ส่วนที่ 3: วาดหิน (Rock) หลายก้อน
        // -------------------------------------------------------
        // วนลูปวาดหินทุกก้อนตามข้อมูลใน vector 'rocks'
        for (const auto& rock : rocks)
        {
            glm::mat4 rockModel = glm::mat4(1.0f);
            // 1. เลื่อนตำแหน่ง (Translate)
            rockModel = glm::translate(rockModel, rock.position);
            // 2. หมุน (Rotate) รอบแกน Y
            rockModel = glm::rotate(rockModel, glm::radians(rock.rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            // 3. ปรับขนาด (Scale) ให้เท่ากันทุกแกน (X, Y, Z)
            rockModel = glm::scale(rockModel, glm::vec3(rock.scale));
            // ส่งค่า Matrix ไปที่ Shader และสั่งวาด
            modelShader.setMat4("model", rockModel);
            island.Draw(modelShader);
        }

        // -------------------------------------------------------
        // ส่วนที่ 4: วาดไอเทม (Items)
        // -------------------------------------------------------
        for (const auto& item : items) {
            // วาดเฉพาะชิ้นที่ยังไม่ถูกเก็บ
            if (!item.isCollected) {
                glm::mat4 modelItem = glm::mat4(1.0f);
                modelItem = glm::translate(modelItem, item.position);
                // ทำให้ไอเทมหมุนรอบตัวเองตลอดเวลา จะได้ดูเป็นของให้เก็บ
                modelItem = glm::rotate(modelItem, currentFrame * 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
                modelItem = glm::scale(modelItem, glm::vec3(0.25f)); // ปรับขนาดโมเดลไอเทมให้พอดี
                modelShader.setMat4("model", modelItem);
                collectableItem.Draw(modelShader);
            }
        }

        // swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &trianglesVAO);
    glDeleteBuffers(1, &trianglesVBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}



// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // 1. กำหนดความเร็วในการขยับและการเลี้ยว
    float moveSpeed = playerSpeed * deltaTime;
    float turnSpeed = 60.0f * deltaTime; // ความไวในการหมุนพวงมาลัย (องศาต่อวินาที)

    // 2.1 เช็คว่ากำลังเหยียบคันเร่ง (W) หรือถอยหลัง (S) อยู่หรือไม่
    bool movingForward = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    bool movingBackward = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;

    // 2.2 บังคับพวงมาลัย (จะเลี้ยวได้ก็ต่อเมื่อรถขยับเท่านั้น)
    if (movingForward) {
        // เดินหน้าปกติ: A เลี้ยวซ้าย, D เลี้ยวขวา
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) playerYaw += turnSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) playerYaw -= turnSpeed;
    }
    else if (movingBackward) {
        // ถอยหลัง: ทิศทางการกวาดของหน้ารถจะกลับทิศทางกันเพื่อความสมจริง
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) playerYaw -= turnSpeed; 
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) playerYaw += turnSpeed;
    }

    // 3. คำนวณเวกเตอร์ "หน้ารถ" (Forward Vector) จากมุม playerYaw
    // เราใช้ sin() สำหรับแกน X และ cos() สำหรับแกน Z เพื่อแปลงมุมกลับมาเป็นทิศทาง
    glm::vec3 carFront;
    carFront.x = sin(glm::radians(playerYaw));
    carFront.y = 0.0f;
    carFront.z = cos(glm::radians(playerYaw));
    carFront = glm::normalize(carFront);

    // 4. เก็บตำแหน่งก่อนเดินไว้สำหรับการคำนวณการชน
    glm::vec3 oldPos = playerPos;

    // 5. คันเร่ง และ เบรก/ถอยหลัง (W/S ขยับตามทิศทางหน้ารถเสมอ)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        playerPos += carFront * moveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        playerPos -= carFront * (moveSpeed * 0.5f);
    }

    // -------------------------------------------------------------
    // โค้ดส่วนการเช็คการชน (Collision)
    // -------------------------------------------------------------

    // 2. เช็คการชนกับฉาก (หิน) Player-Scene
    if (CheckRockCollision(playerPos, playerRadius, rocks)) {
        // ถ้าชน ให้เด้งกลับไปจุดเดิมก่อนเดิน (ไม่ให้เดินทะลุ)
        playerPos = oldPos;
    }

    // 3. เช็คการเก็บไอเทม (Player-Items)
    CheckAndCollectItems(playerPos, playerRadius, items);

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


