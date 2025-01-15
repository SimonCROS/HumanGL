#shader vertex
#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;

void main() {
  gl_Position.xyz = vertexPosition_modelspace;
  gl_Position.w = 1.0;
}

#shader fragment
#version 330 core

out vec3 color;

void main() {
  color = vec3(1,0,0);
}
