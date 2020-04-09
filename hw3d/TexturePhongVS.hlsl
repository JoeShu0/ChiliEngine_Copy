cbuffer CBuf
{
    matrix model;
    matrix modelView;
    matrix modelViewProj;
};

struct VSOut
{
    float4 ScreenPos : SV_Position;
    float3 worldPos : WPosition;
    float3 worldnormal : WNormal;
    float2 tc : Texcoord;
};

VSOut main(float3 pos : Position, float3 n : Normal, float2 tc : Texcoord)
{
    VSOut vso;
    vso.worldPos = (float3) mul(float4(pos, 1.0f), model);
    vso.worldnormal = mul(n, (float3x3) model);
    vso.ScreenPos = mul(float4(pos, 1.0f), modelViewProj);
    vso.tc = tc;
    return vso;
}