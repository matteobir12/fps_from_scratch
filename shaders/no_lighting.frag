#version 330 core
precision highp float;

in vec4 frag_color;
in vec2 vTextCoord;

out vec4 outColor;

void main() {
    outColor = frag_color;
}