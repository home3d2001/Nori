
struct Out
{
  float4 color : COLOR0;
  float4 normal : COLOR1;
};

Out main(uniform float3 color, in float3 normal)
{
  Out result;
  result.color = float4(color, 1);
  result.normal = float4(normalize(normal) / 2 + float3(0.5), 0);
  return result;
}
