
struct Vertex
{
  vec3 position;
  vec3 normal;
};

uniform mat4 MVP;
uniform mat4 MV;

in Vertex vertex;

varying vec3 eyepos;
varying vec3 normal;

void main()
{
  eyepos = MV * vec4(vertex.position, 1);
  normal = MV * vec4(vertex.normal, 0);

  gl_Position = MVP * vec4(vertex.position, 1);
}
