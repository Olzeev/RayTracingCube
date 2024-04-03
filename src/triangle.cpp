
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <fstream>
#include <iostream>
#include <string>

float ang = 1;

int WIDTH = 1280;
int HEIGHT = 720;

GLuint compileShader(GLenum shaderType, const std::string& shaderSource) {
    GLuint shaderID = glCreateShader(shaderType);
    const char* source = shaderSource.c_str();
    shaderSource(shaderID, 1, &source, NULL);
    compileShader(shaderID);

    

    return shaderID;
}

std::string readFile(const std::string& filePath) {
    std::ifstream fileStream(filePath);
    if (!fileStream.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }

    std::string content((std::istreambuf_iterator<char>(fileStream)), (std::istreambuf_iterator<char>()));
    fileStream.close();
    return content;
}


// Clears the current window and draws a triangle.
void display() {
  glClear(GL_COLOR_BUFFER_BIT);
    
    glFlush();
}

void timer(int value) {
  glutPostRedisplay(); // Помечаем окно как требующее перерисовки
  glutTimerFunc(16, timer, 0); // Вызываем функцию через 16 миллисекунд (около 60 кадров в секунду)
}
// Initializes GLUT, the display mode, and main window; registers callbacks;
// enters the main event loop.
int main(int argc, char** argv) {
  // Use a single buffered window in RGB mode (as opposed to a double-buffered
  // window or color-index mode).
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

  // Position window at (80,80)-(480,380) and give it a title.
  glutInitWindowPosition(80, 80);
  glutInitWindowSize(WIDTH, HEIGHT);
  glutCreateWindow("A Simple Triangle");
std::string fragmentShaderSource = readFile("fragment_shader.glsl");
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    if (fragmentShader == 0) {
        std::cerr << "Failed to compile fragment shader" << '\n';
        return -1;
    }

    // Удаление шейдера
    glDeleteShader(fragmentShader);
    
  // Tell GLUT to start reading and processing events.  This function
  // never returns; the program only exits when the user closes the main
  // window or kills the process.
  glutMainLoop();
}