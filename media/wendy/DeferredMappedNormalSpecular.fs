
uniform sampler2D colormap;
uniform sampler2D normalmap;

in vec3 normal;
in vec2 texCoord;

void main()
{
  vec4 NS = texture2D(normalmap, texCoord);

  gl_FragData[0] = texture2D(colormap, texCoord);
  gl_FragData[1] = vec4(normalize(normal) / 2 + 0.5, NS.a);
}

