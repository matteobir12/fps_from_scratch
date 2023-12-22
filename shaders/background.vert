#version 330 core
in vec4 aPosition;

out vec4 vPosition;
void main() {
    vPosition = aPosition;
    gl_Position = aPosition;
    gl_Position.z = 1.0;
}