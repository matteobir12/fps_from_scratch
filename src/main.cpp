#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include "ShaderProgram.h"

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(getShaderProgram());

    glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f( 0.5f, -0.5f);
        glVertex2f( 0.0f,  0.5f);
    glEnd();

    glutSwapBuffers();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("GLSL Shaders with GLUT");

    GLenum glewInitResult = glewInit();
    if (GLEW_OK != glewInitResult) {
        std::cerr << "ERROR: " << glewGetErrorString(glewInitResult) << std::endl;
        return 1;
    }
    glutDisplayFunc(display);

    glutMainLoop();

    return 0;
}