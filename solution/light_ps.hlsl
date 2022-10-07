// Light pixel shader
// Calculate diffuse lighting for a single directional light(also texturing)

Texture2D shaderTexture1 : register(t0);
Texture2D shaderTexture2 : register(t1);

SamplerState SampleType : register(s0);


cbuffer LightBuffer : register(b0)
{
	float4 ambientColor;
    float4 diffuseColor;
    float3 lightPosition;
    float padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 position3D : TEXCOORD2;
};

float4 main(InputType input) : SV_TARGET
{
	float4	textureColor;
    float3	lightDir;
    float	lightIntensity;
    float4	color;

	// Invert the light direction for calculations.
	lightDir = normalize(input.position3D - lightPosition);

	// Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(input.normal, -lightDir));

	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
	color = ambientColor + (diffuseColor * lightIntensity); //adding ambient
	color = saturate(color);

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.

	if (padding == 1)
	{
		textureColor = shaderTexture2.Sample(SampleType, input.tex);
	}
	else
	{
		textureColor = shaderTexture1.Sample(SampleType, input.tex);
	}
	color = color * textureColor;
	
	
	if (input.position3D.y < -5 && input.position3D.y >-10 && input.position3D.x > -7.475 && input.position3D.x < -2.52 && input.position3D.z > -7.5)
	{
		color.y = color.y + 0.1;
	}

	if (input.position3D.y < -9.9f && input.position3D.y > -10.001f && input.position3D.x < -5 && input.position3D.x > -6 && input.position3D.z > -4 && input.position3D.z < -3)
	{
		color = color / 2.f;
	}

    return color;
}

