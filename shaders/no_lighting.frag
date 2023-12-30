#version 330 core
precision highp float;

in vec4 frag_color;
in vec2 vTextCoord;

uniform bool uUseTexture;
uniform sampler2D textureSampler;

out vec4 outColor;

void main() {
    if (uUseTexture) {
        vec4 texColor = texture(textureSampler, vTextCoord);
        outColor = texColor * frag_color;
    } else {
        outColor = frag_color;
    }

}