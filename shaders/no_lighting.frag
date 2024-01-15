#version 330 core

in vec4 frag_color;
in vec2 vTextCoord;

uniform bool uUseTexture;
uniform sampler2D textureSampler;

out vec4 outColor;

void main() {
    if (uUseTexture) {
        vec4 texColor = texture(textureSampler, vTextCoord);
        outColor = texColor;
    } else {
        outColor = frag_color;
    }

}