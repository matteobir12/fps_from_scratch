#version 330 core
precision mediump float;

uniform samplerCube uSkybox;
uniform mat4 uViewDirectionProjectionInverse;
 
in vec4 vPosition;

out vec4 outColor;

void main() {
    vec4 t = uViewDirectionProjectionInverse * vPosition;
    outColor = texture(uSkybox, normalize(t.xyz / t.w));
}