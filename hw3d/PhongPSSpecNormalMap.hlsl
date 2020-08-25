cbuffer LightCBuf
{
    float3 lightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

cbuffer ObjectCBuf
{
    bool normalMapEnabled;
    float padding[3];
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
    if (normalMapEnabled)
    {
        const float3x3 TransformToWorld = float3x3(
            normalize(worldtangent),
            normalize(worldbitangent),
            normalize(worldnormal));

        const float3 SampledNormal = nmap.Sample(splr, tc).xyz;
        worldnormal.x = SampledNormal.x * 2.0f - 1.0f;
        worldnormal.y = (SampledNormal.y * 2.0f - 1.0f);
        worldnormal.z = (SampledNormal.z * 2.0f - 1.0f);

        worldnormal = mul(worldnormal, TransformToWorld);
    }
    
	// fragment to light vector data
    const float3 vToL = lightPos - worldPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
    
    // diffuse attenuation
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
    //relected light vector
    const float3 w = worldnormal * dot(dirToL, worldnormal);
    const float3 r = w * 2.0f - dirToL;
    // cal the specular 
    const float4 specularSample = spec.Sample(splr, tc);
    const float3 specularColor = specularSample.rgb;
    const float spacularPower = pow(2.0f, specularSample.a * 13.0f);
    const float3 specular = att * (diffuseColor * diffuseIntensity) * pow(max(0.0f, dot(normalize(r), normalize(CameraWPos - worldPos))), spacularPower);
	// diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, worldnormal));
	// final color
    //return float4(saturate(specular) * materialColor, 1.0f);
    return float4(saturate(diffuse + ambient) * tex.Sample(splr, tc).rgb + specular * specularColor, 1.0f);
}