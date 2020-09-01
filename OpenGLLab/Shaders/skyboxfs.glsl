#version 330
in Data
{
    vec4 eyespace_position;
    vec4 eyespace_normal;
    vec4 worldspace_position;
    vec4 raw_position;
} vtx_data;

out vec4 outputColor;

uniform vec4 skytop = vec4(1.0f, 1.0f, 1.0f, 1.0f);
uniform vec4 skyhorizon = vec4(0.54f, 0.54f, 0.54f, 1.0f);
uniform float horizonintensity = 3;

void main()
{  
    vec3 pointOnSphere = normalize(vtx_data.worldspace_position.xyz);
    
    float a = pointOnSphere.y * horizonintensity;
    vec4 gradientsky = mix(skyhorizon, skytop, a);
    
    outputColor = gradientsky;
}