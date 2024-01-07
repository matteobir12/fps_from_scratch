#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 vertexNorm;
layout (location = 2) in vec2 aTextCoord;

uniform mat4 uMat;

uniform mat4 uWorld;
uniform mat4 uWorldInverseTranspose;
uniform vec3 uViewerWorldPosition;

out vec3 vNormal;
out vec3 surfaceWorldPosition;
out vec3 vSurfaceToViewer;
out vec2 vTextCoord;

void main() {
    vTextCoord = aTextCoord;
    gl_Position = uMat * vec4(aPosition, 1.0);

    vNormal = mat3(uWorldInverseTranspose) * vertexNorm;

    surfaceWorldPosition = mat3(uWorld) * aPosition;
    vSurfaceToViewer = uViewerWorldPosition - surfaceWorldPosition;
}