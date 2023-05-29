#include "scop.hpp"
#include "Screen.hpp"
#include "TextureManager.hpp"
#include <vector>
#include <iostream>
#include "../glm/gtc/matrix_transform.hpp"
#include <sys/time.h>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "ObjLoader.hpp"

void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

bool firstMouse = true;
float yaw   = -90.0f;
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  800.0f / 2.0;
float fov   =  45.0f;
bool isSmooth = false;
bool disco = false;
bool isGrey = true;
float cameraSpeed = .55f;
glm::vec3   bgColor;

struct Vertex {
    glm::vec3   pos;
    glm::vec3   color;
};

Screen	screen;
std::vector<Vertex> vertices;
std::vector<glm::vec3> out_vertices;
std::vector<GLuint> indices;
float angle = 0.0f;
float tmp_angle = 0.0f;
glm::mat4 mtxCam;
glm::mat4 mtxTransform;

glm::vec3 cameraPos   = glm::vec3(0.0f, 7.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
std::vector<glm::vec3> colors;

int mod = 0;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if ((key == GLFW_KEY_F) && (action == GLFW_PRESS))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if ((key == GLFW_KEY_P) && (action == GLFW_PRESS))
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    if ((key == GLFW_KEY_L) && (action == GLFW_PRESS))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if ((key == GLFW_KEY_C) && (action == GLFW_PRESS)) {
        if (mod % 2 == 0)
            glfwSetInputMode(screen.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(screen.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        mod++;
    }
    if ((key == GLFW_KEY_SPACE) && (action == GLFW_PRESS))
        tmp_angle += 0.1f;
    if ((key == GLFW_KEY_1) && (action == GLFW_PRESS))
        isSmooth = !isSmooth;
    if ((key == GLFW_KEY_2) && (action == GLFW_PRESS))
        glEnable(GL_MULTISAMPLE);
    if ((key == GLFW_KEY_3) && (action == GLFW_PRESS))
        glDisable(GL_MULTISAMPLE);
    if ((key == GLFW_KEY_LEFT_SHIFT) && (action == GLFW_PRESS))
        cameraSpeed = 5.0f;
    if ((key == GLFW_KEY_LEFT_SHIFT) && (action == GLFW_RELEASE))
        cameraSpeed = .55f;
    if ((key == GLFW_KEY_BACKSPACE) && (action == GLFW_PRESS)) {
        tmp_angle = 0.0f;
    }
    if ((key == GLFW_KEY_4) && (action == GLFW_PRESS))
        bgColor = glm::vec3(static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX);
     if ((key == GLFW_KEY_G) && (action == GLFW_PRESS))
        isGrey = !isGrey;
    if ((key == GLFW_KEY_5) && (action == GLFW_PRESS))
        disco = !disco;
}

void loadObjFile(const char* filename) {
    std::ifstream in(filename);
    std::string line;
    std::vector<std::string> lines;
    glm::vec3   Blue(1.0f, 0.0f, 0.0f);
    glm::vec3   Yellow(1.0f, 1.0f, 0.0f);
    glm::vec3   Black(1.0f, 0.0f, 0.0f);
    glm::vec3   Grey(0.5f, 0.5f, 0.5f);
    int len;

    if (!in.is_open()) {
        std::cout << "File cannot be opened!\n";
        exit(1);
    }

    while (std::getline(in, line)) {
        lines.push_back(line);
        if (line[0] == 'v')
            len++;
    }

    int idx = 0;
    for (const std::string& line : lines) {
        if (line[0] == 'v') {
            float x = 0.0f, y = 0.0f, z = 0.0f;
            Vertex v;

            std::istringstream iss(line);
            char type;
            iss >> type >> std::setprecision(6) >> x >> y >> z;
            v.pos.x = x;
            v.pos.y = y;
            v.pos.z = z;
            v.color = glm::vec3(static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX);
            // if (idx < len / 2)
            //     v.color = Blue;
            // else 
            //     v.color = Yellow;
            idx++;
            vertices.push_back(v);
        }
        else if (line[0] == 'f') {
            unsigned int f1 = 0, f2 = 0, f3 = 0, f4 = 0;

            std::istringstream iss(line);
            char type;
            iss >> type >> f1 >> f2 >> f3 >> f4;
            indices.push_back(f1 - 1);
            indices.push_back(f2 - 1);
            indices.push_back(f3 - 1);
            if (f4 != 0) {
                indices.push_back(f1 - 1);
                indices.push_back(f3 - 1);
                indices.push_back(f4 - 1);
            }
        }
    }
}

void loadBlenderOBJ(const char *filename)
{
    std::stringstream ss;
    std::ifstream   in_file(filename);
    std::string line;
    std::string prefix;
    GLint       tmp_glint = 0;

    if (!in_file.is_open()) {
        std::cout << "File can't open !\n";
        exit(1);
    }
    std::vector<std::string>    lines;
    while (std::getline(in_file, line))
    {
        ss.clear();
        ss.str(line);
        ss >> prefix;

        if (prefix == "#") {

        }
        else if (prefix == "o") {
            
        }
        else if (prefix == "s") {
            
        }
        else if (prefix == "use_mtl") {

        }
        else if (prefix == "v") {
            Vertex v;
            v.color = glm::vec3(static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX);
            ss >> v.pos.x >> v.pos.y >> v.pos.z;
            vertices.push_back(v);
        }
        else if (prefix == "vt") {
        }
        else if (prefix == "vn") {
        }
        else if (prefix == "f") {
            int counter = 0;
            while (ss >> tmp_glint)
            {
                if (counter == 0) {
                    indices.push_back(tmp_glint);
                }
                if (counter == 1) {
                }
                if (counter == 2) {
                }
                if (ss.peek() == '/') {
                    ++counter;
                    ss.ignore(1, '/');
                }
                else if (ss.peek() == ' ') {
                    ++counter;
                    ss.ignore(1, ' ');
                }
                if (counter > 2)
                    counter = 0;
            }
        }
    }
}

unsigned int VBO;
unsigned int VAO;
unsigned int EBO;
unsigned int texture;

int main(int argc, const char** argv)
{
	ShaderProgram	program;

    glfwSetInputMode(screen.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    program.attachShader("./shaders/fragmentShader.glsl", GL_FRAGMENT_SHADER);
    program.attachShader("./shaders/vertexShader.glsl", GL_VERTEX_SHADER);
    program.link();
    glfwMakeContextCurrent(screen.getWindow());
    loadObjFile(argv[1]);

    glGenVertexArrays(1, &VAO);

    glGenBuffers(1,&VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);

    glEnableVertexAttribArray(0);
    glBufferData(GL_ARRAY_BUFFER,sizeof(Vertex) * vertices.size(), &vertices[0],GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(sizeof(float) * 3));

    program.use();
    glfwSetKeyCallback(screen.getWindow(), key_callback);
    glfwSetCursorPosCallback(screen.getWindow(), mouse_callback);
    glfwSetScrollCallback(screen.getWindow(), scroll_callback);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), &indices[0], GL_STATIC_DRAW);
    // loadOBJ("/goinfre/rtosun/amIJoke/resources/cube.obj");
    program.addUniform("uMtxTransform");
    program.addUniform("view");
    program.addUniform("projection");
    program.addUniform("model");
    program.addUniform("u_IsSmooth");
    program.addUniform("u_IsGrey");

    TextureManager::getInstance()->activateTexture(GL_TEXTURE0, texture);
	glEnable(GL_DEPTH_TEST);

	// Gorus acisi, cozunurluk, yakinlik duzlemi, uzaklik duzlemi
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)1920 / (float)1080, 0.1f, 5000.0f);
    program.setMat4("projection", &projection);
    std::cout << "Keys :\nCamera movement : wasd\nColor mode : G\nSmooth/Unsmooth mode : 1\nFill mode : F\nPoint mode : P\nLine mode : M\nModels rotation : Space\nModels stop rotation : backspace\nCamera move speed : Left Shift\nBgColor : 4\nDisco : 5\n";
	while(glfwGetKey(screen.getWindow(), GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(screen.getWindow()))
	{
        processInput(screen.getWindow());
        if (disco)
            bgColor = glm::vec3(static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX);
		glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        program.setMat4("view", &view);
        program.setBool("u_IsSmooth", isSmooth);
        program.setBool("u_IsGrey", isGrey);
        float radius = 10.0f;
        float camX = static_cast<float>(sin(glfwGetTime()) * radius);
        float camZ = static_cast<float>(cos(glfwGetTime()) * radius);
        view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        float posx = 0.0f;
        float posz = 0.0f;

        for (int i = 0; i < 1; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            if (i % 55 == 0 && i != 0) {
                posx = 0.0f;
                posz += 13.0f;
            }
            model = glm::translate(model, glm::vec3(posx, 0.0f, posz));

            if (i >= 500)
            {
                model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
            }
            else
                model = glm::rotate(model, glm::radians(-angle), glm::vec3(0.0f, 1.0f, 0.0f));
            
            // model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
            program.setMat4("model", &model);
    		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            posx += 13.0f;
        }
        angle += tmp_angle;
		glfwSwapBuffers(screen.getWindow());
		glfwPollEvents();
	}
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

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
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}
