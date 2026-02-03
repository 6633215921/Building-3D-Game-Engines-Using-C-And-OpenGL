#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <learnopengl/filesystem.h>
#include <learnopengl/shader_s.h>
#include <iostream>
#include <vector>
#include <cmath> 

struct Mesh {
    unsigned int vao, vbo, ebo;
    int indexCount;
};

Mesh createCircle(int segmentCount, float radius) {
    const float PI = 3.14159265359f;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    // --- 1. Generate Vertices [Pos(3), Color(3), Tex(2)] ---
    // 1.1 Center Point
    vertices.insert(vertices.end(), { 0.0f, 0.0f, 0.0f }); // Pos
    vertices.insert(vertices.end(), { 1.0f, 1.0f, 1.0f }); // Color (White)
    vertices.insert(vertices.end(), { 0.5f, 0.5f });       // UV
    // 1.2 Perimeter Points
    for (int i = 0; i <= segmentCount; i++) {
        float angle = i * (2.0f * PI / segmentCount);
        float x = cos(angle) * radius;
        float y = sin(angle) * radius;
        // Position
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f);
        // Color (Don't Use)
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(0);
        // Texture Coordinates (Remap -1:1 to 0:1)
        vertices.push_back((cos(angle) * 0.5f) + 0.5f);
        vertices.push_back((sin(angle) * 0.5f) + 0.5f);
    }
    // --- 2. Generate Indices ---
    for (int i = 1; i <= segmentCount; i++) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
    // --- 3. OpenGL Buffers ---
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    // Attribute Pointers (Stride: 8 * float)
    GLsizei stride = 8 * sizeof(float);
    // Position (Location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    // Color (Location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Texture Coord (Location 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    return { VAO, VBO, EBO, (int)indices.size() };
}
Mesh createDashedCircle(int segmentCount, float radius) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    const float PI = 3.14159265359f;

    // 1. Generate Vertices (เหมือนวงกลมปกติ แต่เราต้องการแค่ขอบ)
    for (int i = 0; i < segmentCount; i++) {
        float angle = (float)i / segmentCount * 2.0f * PI;
        float x = cos(angle) * radius;
        float y = sin(angle) * radius;

        // Position
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f);
        // Color 
        vertices.push_back(1.0f);
        vertices.push_back(1.0f);
        vertices.push_back(1.0f);
        // UV (ไม่ได้ใช้ แต่ใส่ไว้ให้ format ตรงกับ Shader เดิม)
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
    }

    // 2. Generate Indices สำหรับ GL_LINES (ทำเส้นประ)
    // เราจะเชื่อมจุด i กับ i+1 แล้วข้ามไป 1 ช่อง เพื่อให้เกิดช่องว่าง
    for (int i = 0; i < segmentCount; i += 2) {
        indices.push_back(i);
        indices.push_back((i + 1) % segmentCount);
    }

    // 3. OpenGL Buffers (Copy Logic เดิมมา)
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    GLsizei stride = 8 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // หมายเหตุ: indexCount ที่ return ไปจะเป็นจำนวนจุดของเส้น ไม่ใช่จำนวนสามเหลี่ยม
    return { VAO, VBO, EBO, (int)indices.size() };
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

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

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("5.1.transform.vs", "5.1.transform.fs");

    // --- 1. Generate Vertices ---
    // Create the circle once
    Mesh sunCircle = createCircle(64, 0.3f);
    Mesh earthCircle = createCircle(64, 0.2f);
    Mesh moonCircle = createCircle(64, 0.1f);

    // สร้างเส้นวงโคจร (Orbit Lines)
    // รัศมี 1.0f คือวงโคจรโลก (ต้องเท่ากับ orbitRadius ของโลกใน Loop)
    Mesh earthOrbitLine = createDashedCircle(100, 0.75f);
    // รัศมี 0.5f คือวงโคจรดวงจันทร์ (ต้องเท่ากับ orbitRadius ของดวงจันทร์ใน Loop)
    Mesh moonOrbitLine = createDashedCircle(60, 0.25f);

    // load and create a texture 
    // -------------------------
    unsigned int texture1, texture2;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load(FileSystem::getPath("resources/textures/earth.png").c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load(FileSystem::getPath("resources/textures/moon.png").c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use();
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);
        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		// set time uniform
        ourShader.use();
        float timeValue = glfwGetTime();
        ourShader.setFloat("iTime", timeValue);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

		// Get mouse position in NDC
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

		// Get window size
        int width, height;
        glfwGetWindowSize(window, &width, &height);

		// Convert to NDC
        float xNDC = (2.0f * (float)xpos / width) - 1.0f;
        float yNDC = 1.0f - (2.0f * (float)ypos / height);

		// Set mouse position uniform (in screen coordinates)
        int mouseLoc = glGetUniformLocation(ourShader.ID, "uMousePos");
        glUniform2f(mouseLoc, (float)xpos, (float)(height - ypos));

		// Get framebuffer size for correct gl_FragCoord mapping
        int currentWidth, currentHeight;
        glfwGetFramebufferSize(window, &currentWidth, &currentHeight); 

		// Set center position uniform (will be updated for Earth and Moon)
        int CenterLoc = glGetUniformLocation(ourShader.ID, "uCenter");

        // Correct for aspect ratio
        float aspect = (float)width / (float)height;

        // 1. วาดดวงอาทิตย์ (ตรงกลาง / ตามเมาส์)
        glm::mat4 sunTransform = glm::mat4(1.0f);
        sunTransform = glm::translate(sunTransform, glm::vec3(xNDC, yNDC, 0.0f));
        sunTransform = glm::scale(sunTransform, glm::vec3(1.0f / aspect, 1.0f, 1.0f));
        ourShader.setInt("useTexture", 0); 
        unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(sunTransform));
        glBindVertexArray(sunCircle.vao);
        glDrawElements(GL_TRIANGLES, sunCircle.indexCount, GL_UNSIGNED_INT, 0);


        // 2. วาดโลก (โคจรรอบดวงอาทิตย์ + มี Texture)
        // bind textures on corresponding texture units
        glm::mat4 earthTransform = glm::mat4(1.0f);
        // --- ลำดับการคูณ Matrix สำคัญมาก (อ่านจากล่างขึ้นบนในโค้ด หรือ ขวาไปซ้ายในสมการ) ---
        float orbitRadius = 0.75f; // ระยะห่างจากดวงอาทิตย์
        float orbitSpeed = 0.3f;
        float selfRotationSpeed = 3.0f;
        // 1. Scale แก้ Aspect Ratio (ทำสุดท้ายใน Matrix Stack)
        earthTransform = glm::scale(earthTransform, glm::vec3(1.0f / aspect, 1.0f, 1.0f));
        // 2. ย้ายไปหาดวงอาทิตย์ (จุดศูนย์กลางการโคจร)
        earthTransform = glm::translate(earthTransform, glm::vec3(xNDC * aspect, yNDC, 0.0f));
        // 3. การโคจร (Orbit)
        // ใช้ Matrix แยกเพื่อทำวงรี (ถ้าอยากทำ) หรือใช้วงกลมแบบเดิมก็ได้
        float orbitAngle = timeValue * orbitSpeed;
        // ตัวอย่างวงรี: x = a cos(t), y = b sin(t)
        float earthX = cos(orbitAngle) * orbitRadius;
        float earthY = sin(orbitAngle) * orbitRadius * 1.0f; // 0.9 ถ้าอยากให้เป็นวงรีนิดๆ
        earthTransform = glm::translate(earthTransform, glm::vec3(earthX, earthY, 0.0f));
        // 4. จำลองแกนเอียง (Axial Tilt)
        // เอียงแกน 23.5 องศา (รอบแกน Z เพราะเรามองแบบ 2D)
        earthTransform = glm::rotate(earthTransform, glm::radians(-23.5f), glm::vec3(0.0f, 0.0f, 1.0f)); // เอียงแกน
        // 5. หมุนรอบตัวเอง (Rotation)
        // สังเกต: เราหมุนรอบแกน Y ของ Earth (ซึ่งตอนนี้เอียงอยู่)
        earthTransform = glm::rotate(earthTransform, timeValue * selfRotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
        // 1. หาตำแหน่งโลกใน NDC (-1 ถึง 1)
        glm::vec4 earthPosNDC = earthTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        // 2. แปลงเป็นพิกัดหน้าจอ (Screen Coordinates) เพื่อให้หน่วยเดียวกับ gl_FragCoord
        float screenEarthX = (earthPosNDC.x + 1.0f) * 0.5f * currentWidth;
        float screenEarthY = (earthPosNDC.y + 1.0f) * 0.5f * currentHeight;
        glUniform2f(CenterLoc, screenEarthX, screenEarthY);
        ourShader.setInt("useTexture", 1); // เปิดโหมด Texture
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(earthTransform));
        glBindVertexArray(earthCircle.vao); // ใช้วงกลมอันเดิม หรืออันใหม่ที่ขนาดต่างกันก็ได้
        glDrawElements(GL_TRIANGLES, earthCircle.indexCount, GL_UNSIGNED_INT, 0);


		// 3. วาดดวงจันทร์ (โคจรรอบโลก + มี Texture)
        glm::mat4 moonTransform = glm::mat4(1.0f);
        orbitRadius = 0.25f; // ระยะห่างจากโลก
        orbitSpeed = 0.3f;
        selfRotationSpeed = 0.3f;
        moonTransform = glm::scale(moonTransform, glm::vec3(1.0f / aspect, 1.0f, 1.0f));
        moonTransform = glm::translate(moonTransform, glm::vec3(earthPosNDC.x * aspect, earthPosNDC.y, 0.0f));
        orbitAngle = timeValue * orbitSpeed;
        float moonX = cos(orbitAngle) * orbitRadius;
        float moonY = sin(orbitAngle) * orbitRadius * 1.0f;
        moonTransform = glm::translate(moonTransform, glm::vec3(moonX, moonY, 0.0f));
        // 4. จำลองแกนเอียง (Axial Tilt)
        moonTransform = glm::rotate(moonTransform, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        // 5. หมุนรอบตัวเอง (Rotation)
        // สังเกต: เราหมุนรอบแกน Z ของ Moon
        moonTransform = glm::rotate(moonTransform, timeValue * selfRotationSpeed, glm::vec3(0.0f, 0.0f, 1.0f));
        // 1. หาตำแหน่งโลกใน NDC (-1 ถึง 1)
        glm::vec4 moonPosNDC = moonTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        // 2. แปลงเป็นพิกัดหน้าจอ (Screen Coordinates) เพื่อให้หน่วยเดียวกับ gl_FragCoord
        float screenMoonX = (moonPosNDC.x + 1.0f) * 0.5f * currentWidth;
        float screenMoonY = (moonPosNDC.y + 1.0f) * 0.5f * currentHeight;
        glUniform2f(CenterLoc, screenMoonX, screenMoonY);
        ourShader.setInt("useTexture", 2);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(moonTransform));
        glBindVertexArray(moonCircle.vao);
        glDrawElements(GL_TRIANGLES, moonCircle.indexCount, GL_UNSIGNED_INT, 0);


        // --- วาดเส้นวงโคจรโลก (รอบดวงอาทิตย์) ---
        glm::mat4 orbitTransform = glm::mat4(1.0f);
        orbitTransform = glm::scale(orbitTransform, glm::vec3(1.0f / aspect, 1.0f, 1.0f));
        orbitTransform = glm::translate(orbitTransform, glm::vec3(xNDC * aspect, yNDC, 0.0f));
        ourShader.setInt("useTexture", 3); 
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(orbitTransform));
        glBindVertexArray(earthOrbitLine.vao);
        glDrawElements(GL_LINES, earthOrbitLine.indexCount, GL_UNSIGNED_INT, 0);

        // --- วาดเส้นวงโคจรดวงจันทร์ (รอบโลก) ---
        glm::mat4 moonOrbitTransform = glm::mat4(1.0f);
        moonOrbitTransform = glm::scale(moonOrbitTransform, glm::vec3(1.0f / aspect, 1.0f, 1.0f));
        moonOrbitTransform = glm::translate(moonOrbitTransform, glm::vec3(earthPosNDC.x * aspect, earthPosNDC.y, 0.0f));
        ourShader.setInt("useTexture", 3);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(moonOrbitTransform));
        glBindVertexArray(moonOrbitLine.vao);
        glDrawElements(GL_LINES, moonOrbitLine.indexCount, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &sunCircle.vao);
    glDeleteBuffers(1, &sunCircle.vbo);
    glDeleteBuffers(1, &sunCircle.ebo);

    glDeleteVertexArrays(1, &earthCircle.vao);
    glDeleteBuffers(1, &earthCircle.vbo);
    glDeleteBuffers(1, &earthCircle.ebo);

    glDeleteVertexArrays(1, &moonCircle.vao);
    glDeleteBuffers(1, &moonCircle.vbo);
    glDeleteBuffers(1, &moonCircle.ebo);

    glDeleteVertexArrays(1, &earthOrbitLine.vao);
    glDeleteBuffers(1, &earthOrbitLine.vbo);
    glDeleteBuffers(1, &earthOrbitLine.ebo);

    glDeleteVertexArrays(1, &moonOrbitLine.vao);
    glDeleteBuffers(1, &moonOrbitLine.vbo);
    glDeleteBuffers(1, &moonOrbitLine.ebo);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}