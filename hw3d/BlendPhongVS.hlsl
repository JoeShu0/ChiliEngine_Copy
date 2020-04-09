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
    float3 viewPos : VPosition;
    float3 viewnormal : VNormal;
    float3 color : Color;
};

VSOut main(float3 pos : Position, float3 n : Normal, float3 color : Color)
{
    VSOut vso;
    vso.worldPos = (float3) mul(float4(pos, 1.0f), model);
    vso.worldnormal = mul(n, (float3x3) model);
    vso.viewPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.viewnormal = mul(n, (float3x3) modelView);
    vso.ScreenPos = mul(float4(pos, 1.0f), modelViewProj);
    vso.color = color;
    return vso;
}