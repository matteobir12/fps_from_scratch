#version 330 core

in vec3 vNormal;
in vec3 surfaceWorldPosition;
in vec3 vSurfaceToViewer;
in vec2 vTextCoord;

uniform bool uUseTexture;

uniform sampler2D uTexture;
uniform vec3 uLightPosition[3];
uniform vec3 uLightDirection[3];
uniform vec3 uLightColor[3]; // multiply the light's color with the diffuse and specular components to affect the surface color.
uniform bool uLightIsDirectional[3];
uniform bool uLightIsOn[3];
uniform float uShininess;
uniform float lightCuttoff;
uniform vec3 ambientLight;
uniform vec3 specColor;
uniform vec4 u_color;

//uniform vec4 u_color;

out vec4 outColor;

void main() {
    vec4 color = u_color;
    if (uUseTexture){
        color = texture(uTexture,vTextCoord);
    }
    vec3 diffuse;
    vec3 specular;
    vec3 ambient = ambientLight * color.xyz;

    vec3 normal = normalize(vNormal);
    
    for(int i = 0; i < 3; i++) {
        
        if (!uLightIsOn[i]){
            continue;
        }
        vec3 lightDirection = normalize(uLightPosition[i]-surfaceWorldPosition);
        vec3 halfVector = normalize(lightDirection + normalize(vSurfaceToViewer));
        if (uLightIsDirectional[i]) {
            float dp = dot(lightDirection, normalize(uLightDirection[i]));
            if ( dp >= lightCuttoff) {

                diffuse += max(dot(normal, normalize(uLightDirection[i])),0.0) * color.xyz;
                float s =  max(dot(normal, halfVector),0.0);
                float specIntense = pow(s, uShininess);
                specular += s * specIntense  * specColor;
    
            }
        }
        else  {

            diffuse += max(dot(normal, normalize(lightDirection)),0.0) * color.xyz;
            float s =  max(dot(normal, halfVector),0.0);
            float specIntense = pow(s, uShininess);
            specular += s * specIntense  * specColor;
        }
    }
    //outColor = vec4(normal,1.0);
    outColor = vec4(diffuse + specular +ambient,color.w);
}