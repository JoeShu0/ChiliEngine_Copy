cbuffer CBuf
{
    matrix model;
    matrix medelView;
    matrix modelViewProj;
};

struct VSOut
{
	float4 pos : SV_Position;
	float3 Wpos : WorldPosition;
};

VSOut main(float3 pos : Position)
{
	VSOut vso;
	vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.Wpos = (float3) mul(float4(pos, 1.0f), model);
	return vso;
}