cbuffer CBuf
{
	matrix model;// model local to world
    matrix modelView; // model local to view local
	matrix modelViewProj; // model local to screen
};

struct VSOut
{
	float4 ScreenPos : SV_Position;
	float3 worldPos : WPosition;
	float3 worldnormal : WNormal;
    float3 viewPos : VPosition;
    float3 viewnormal : VNormal;
	float2 Texcoord : Texcoord;
};

VSOut main(float3 pos : Position, float3 n : Normal, float2 tc : Texcoord)
{
	VSOut vso;
	vso.worldPos = (float3)mul(float4(pos, 1.0f), model);
	vso.worldnormal = mul(n, (float3x3)model);
    vso.viewPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.viewnormal = mul(n, (float3x3) modelView);
    vso.ScreenPos = mul(float4(pos, 1.0f), modelViewProj);
	vso.Texcoord = tc;
	return vso;
}