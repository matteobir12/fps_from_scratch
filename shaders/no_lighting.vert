#version 330 es
in vec4 aPosition;
in vec2 aTextCoord;

uniform mat4 uMat;

uniform mat4 uWorld;
uniform mat4 uWorldInverseTranspose;
uniform vec3 uViewerWorldPosition;
uniform vec4 u_color;

out vec3 surfaceWorldPosition;
out vec2 vTextCoord;
out vec4 frag_color;
void main() {
    vTextCoord = aTextCoord;
    gl_Position = uMat * aPosition;
    surfaceWorldPosition = (uWorld * aPosition).xyz;
    frag_color = u_color;
}