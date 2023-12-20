#version 330 core
precision highp float;

layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec2 aTextCoord;

uniform mat4 uMat;
uniform vec4 u_color;

out vec2 vTextCoord;
out vec4 frag_color;

void main() {
    vTextCoord = aTextCoord;
    gl_Position = uMat * aPosition;
    frag_color = u_color;
}