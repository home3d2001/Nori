
uniform mat4 P;

struct Vertex
{
  vec2 position;
  vec2 mapping;
};

attribute Vertex vertex;

varying vec2 mapping;

void main()
{
  mapping = vertex.mapping;

  gl_Position = P * vec4(vertex.position, 0, 1);
}
