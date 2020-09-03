#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"
#include "PointLight.hlsl"

cbuffer ObjectCBuf
{
    float specularIntensity;
    float specularPower;
	float padding[2];
};

Texture2D tex;
SamplerState splr;

cbuffer CameraCBuf
{
    float3 CameraWPos;
};

float4 main(float4 pos : SV_Position,
        float3 worldPos : WPosition,
        float3 worldnormal : WNormal,
        float3 viewPos : VPosition,
        float3 viewnormal : VNormal,
        float2 tc : Texcoord) : SV_Target
{
    //Normalize normal
    worldnormal = normalize(worldnormal);
    viewnormal = normalize(viewnormal);

    // fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(lightPos, worldPos);
    
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);

    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, worldnormal);

    const float3 specular = Speculate(specularIntensity.rrr, 1.0f, worldnormal, lv.dirToL, worldPos, CameraWPos, att, specularPower);
    
    return float4(saturate(diffuse + ambient ) * tex.Sample(splr, tc).rgb + specular, 1.0f);
}