#include "ShaderOps.hlsl"
#include "PointLight.hlsl"
#include "LightVectorData.hlsl"

cbuffer ObjectCBuf
{
    bool normalMapEnabled;
    bool specularMapEnabled;
    bool hasGloss;
    float specularPowerConst;
    float3 specularColor;
    float specularMapWeight;
};

Texture2D tex;
Texture2D spec;
Texture2D nmap;
SamplerState splr;

cbuffer CameraCBuf:register(b3)
{
    float3 CameraWPos;
};


float4 main(float4 pos : SV_Position,
    float3 worldPos : WPosition,
    float3 worldnormal : WNormal,
    float3 viewPos : VPosition,
    float3 viewnormal : VNormal,
    float2 tc : Texcoord,
    float3 worldtangent : WTangent,
    float3 worldbitangent : WBiTangent) : SV_Target
{
    //Manual Ztest
    float4 dtex = tex.Sample(splr, tc);
    
    #ifdef MASK_BOI
    clip(dtex.a < 0.1f ? -1 : 1);
    //flip normal when render back face
    if(dot(worldnormal, (worldPos - CameraWPos)) > 0.0f)
    {
        worldnormal = -worldnormal;
    }
    #endif
    
    worldtangent = normalize(worldtangent);
    worldbitangent = normalize(worldbitangent);
    worldnormal = normalize(worldnormal);

    if (normalMapEnabled)
    {
        worldnormal = NormalMapWorldSpace(worldtangent, worldbitangent, worldnormal, tc, nmap, splr);
    }
    
    const LightVectorData lv = CalculateLightVectorData(lightPos, worldPos);
	// fragment to light vector data
    //const float3 vToL = lightPos - worldPos;
    //const float distToL = length(vToL);
    //const float3 dirToL = vToL / distToL;
    
    /*
    // diffuse attenuation
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
    //relected light vector
    const float3 w = worldnormal * dot(dirToL, worldnormal);
    const float3 r = w * 2.0f - dirToL;
    */
    
    // cal the specular 
    float3 specularRColor;
    float specularPower = specularPowerConst;
    if (specularMapEnabled)
    {
        const float4 specularSample = spec.Sample(splr, tc);
        specularRColor = specularSample.rgb * specularMapWeight;
        if (hasGloss)
        {
            specularPower = pow(2.0f, specularSample.a * 13.0f);
        }
    }
    else
    {
        specularRColor = specularColor;
    }
    
    /*
    const float3 specular = att * (diffuseColor * diffuseIntensity) * pow(max(0.0f, dot(normalize(r), normalize(CameraWPos - worldPos))), specularPower);
	// diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, worldnormal));
	// final color
    //return float4(saturate(specular) * materialColor, 1.0f);
    return float4(saturate(diffuse + ambient) * tex.Sample(splr, tc).rgb + specular * specularRColor, 1.0f);
    */
    //sample diffuse
    //float4 dtex = tex.Sample(splr, tc);

    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, worldnormal);

    const float3 specularReflected = Speculate(specularRColor, 1.0f, worldnormal, lv.dirToL, worldPos, CameraWPos, att, specularPower);
    return float4(saturate((diffuse + ambient) * dtex.rgb + specularReflected), dtex.a);
}