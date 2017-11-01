#include"samplerState.hlsl"
#include"Lighting.hlsl"

cbuffer cbPerObject
{
	float4x4 worldMatrix;
	float4x4 worldViewProjMatrix;
	float4x4 worldInvTranspose;
};

cbuffer cbLightCalc
{
	float3 	camPosition;
	float3 lightDirection;
	//Attenuation Factors
	float attConst;
	float attLinear;
	float attQuadratic;
};

struct VertexIn
{
	float3 Pos   : POSITION;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float2 texCoord : TEXCOORD;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float3 PosW :POSITION;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float2 texCoord : TEXCOORD;
};

//ball�� ���� �̹��� ������ shader resource view
Texture2D wallTexture;

//Normal Map texture
Texture2D NormalMap;

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	//��� ���� world���� �������� ��ȯ��.
	vout.PosW=mul(float4(vin.Pos,1.0f), worldMatrix).xyz;
	vout.Normal=mul(float4(vin.Normal,0.0f), worldInvTranspose).xyz;
	vout.Tangent=mul(float4(vin.Tangent, 0.0f), worldMatrix).xyz;

	//Homogeneous space ���� ��ǥ.
	vout.PosH=mul(float4(vin.PosL, 1.0f), worldViewProjMatrix);

	//texture sample ��ǥ.
	vout.TexCoord=vin.TexCoord;

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	pin.Normal=normalize(pin.Normal);

	float3 toCameraVector=camPosition-pin.PosW;

	//camera�� �� ��ǥ ������ �Ÿ��� ����. (attenuation ����� ���� �� ����)
	float distanceToCam=length(toCameraVector);

	//����ȭ.
	toCameraVector/=distanceToCam;

	float4 texColor=wallTexture.Sample(samLinear, pin.texCoord);
/*begin get Normal*/
	float3 tangentSpaceSample=NormalMap.Sample(samLinear, pin.texCoord).rgb;
	float3 tangentSpaceNormal=2.0f*tangentSpaceSample-1.0f;

	//Tangent space ��ǥ�踦 ���Ѵ�.
	//Gram-Schmidt �˰��� ����.
	float3 N=pin.Normal;
	float3 T=normalize(pin.Tangent-dot(pin.Tangent, N)*N);
	float3 B=cross(N,T);

	//tangent space���� world space�� ���� ����� ���Ѵ�.
	float3x3 tangentToWorld=float3x3(T,B,N);

	//Normal�� tangent space���� world space�� ��ȯ�Ѵ�.
	//calculate world space normal
	float3 worldSpaceNormal=mul(tangentSpaceNormal, tangentToWorld);
/*end getNormal*/

	//calculate light attenuation;
	float attFactor=1/(attConst+attLinear*distToCam+attQuadratic*distToCam*distToCam);

	//getTextureColor
	float4 texColor=wallTexture.Sample(samLinear, pin.texCoord);

	float4 ambientColor=getAmbientColor(float4(1,1,1,1), 0.1);

	//texColor to diffuse Color
	float4 diffuseColor=getDiffuseColor(texColor, lightDirection, worldSpaceNormal, 0.3f)*attFactor;

	//get specular color
	float4 specularColor=getSpecularColor(float4(1,1,1,1), toCameraVector, worldSpaceNormal, 0.8f, 1.0f)*attFactor;

	float4 resultColor=ambientColor+diffuseColor+specularColor;

   return resultColor;
}

technique11 wallTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}
