#version 330 core
out vec4 FragColor;

in vec3 color;
//in vec4 vertexColor; // �Ӷ�����ɫ�����������������������ͬ��������ͬ��

void main()
{
    FragColor = vertexColor;
}