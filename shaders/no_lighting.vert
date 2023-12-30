#version 330 core
precision highp float;

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 vertexNorm;
layout (location = 2) in vec2 aTextCoord;

uniform mat4 uMat;
uniform vec4 u_color;

out vec2 vTextCoord;
out vec4 frag_color;

void main() {
    vTextCoord = aTextCoord;
    gl_Position = uMat * vec4(aPosition, 1.0);
    frag_color = u_color;
}