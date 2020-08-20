cbuffer CBuf : register(b1)
{
	float color;
};

float4 main() : SV_Target
{
	return color;
}