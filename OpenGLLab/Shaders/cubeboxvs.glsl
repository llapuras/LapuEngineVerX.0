#version 330
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;

out Data
{
    vec4 eyespace_position;
    vec4 eyespace_normal;
    vec4 worldspace_position;
    vec4 raw_position;
} vtx_data;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat4 view_without_translation = view;
    view_without_translation[3][0] = 0.0f;
    view_without_translation[3][1] = 0.0f;
    view_without_translation[3][2] = 0.0f;

    vtx_data.raw_position = position;
    vtx_data.worldspace_position = position;
    vtx_data.eyespace_position =  view_without_translation * vtx_data.worldspace_position;

    gl_Position = (projection * vtx_data.eyespace_position).xyww;
}