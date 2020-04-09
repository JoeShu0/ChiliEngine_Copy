Texture2D tex;
SamplerState splr;

static const float PI = 3.14159265f;

float4 main(float4 pos : SV_Position, float3 Wpos : WorldPosition) : SV_Target
{
	//return float4(normalize(Wpos),1.0f);
	
	float3 dir = normalize(Wpos);

	float v = atan(length(float2(dir.x, dir.z)) / dir.y) / PI;

	float2 u_vect = normalize(float2(dir.x, dir.z));
	float u = sign(u_vect.x) * acos(u_vect.y) / PI * 0.5f;

	//float u1 = atan(dir.x / dir.z) / PI * 0.5f;


	//return float4(u-1.0f, 0.0f, 0.0f, 1.0f);
	return tex.Sample(splr, float2(u, v)) * 0.45f;
	//return float4(normalize(Wpos),1.0f);
}