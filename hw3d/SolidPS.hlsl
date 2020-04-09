cbuffer CBuf
{
	float color;
};

float4 main() : SV_Target
{
	return color;
}