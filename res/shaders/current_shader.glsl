#shader vertex
#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;

uniform mat4 u_mvp;

void main() {
  gl_Position =  u_mvp * vec4(vertexPosition_modelspace,1);

}

#shader fragment
#version 330 core

out vec3 color;

void main() {
  color = vec3(0.05,0.9,0.6);
}
