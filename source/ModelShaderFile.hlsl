#include"Lighting.hlsl"

cbuffer cbPerObject
{
	float4x4 worldMatrix;
	float4x4 worldViewProjMatrix;
	float4x4 worldInvTranspose;
	float4 AmbientColor;
	float2 UVOffset;
	int MeshNumber;
};

cbuffer cbLightCalc
{
	float3 lightPosition;
};

struct VertexIn
{
	float3 Pos   : POSITION;
	float3 Normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float3 PosW :POSITION;
	float3 Normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

//ball�� ���� �̹��� ������ shader resource view
Texture2D texture_diffuse1;
Texture2D texture_diffuse2;
Texture2D texture_diffuse3;
Texture2D texture_specular1;
Texture2D texture_specular2;

//Normal Map texture
//Texture2D NormalMap;

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	//��� ���� world���� �������� ��ȯ��.
	vout.PosW=mul(float4(vin.Pos,1.0f), worldMatrix).xyz;
	vout.Normal=mul(float4(vin.Normal,0.0f), worldInvTranspose).xyz;

	//Homogeneous space ���� ��ǥ.
	vout.PosH=mul(float4(vin.Pos, 1.0f), worldViewProjMatrix);

	//texture sample ��ǥ.
	vout.texCoord=vin.texCoord;

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	pin.Normal=normalize(pin.Normal);

	//float3 toCameraVector=camPosition-pin.PosW;

	////camera�� �� ��ǥ ������ �Ÿ��� ����.
	//float distanceToCam=length(toCameraVector);

	//����ȭ.
	//toCameraVector/=distanceToCam;

	//distance from light
	float3 toLightVector=lightPosition-pin.PosW;
	toLightVector=normalize(toLightVector);
	//float distanceToLight=length(toLightVector);

	float4 texColor=float4(1,1,0,1);
	
	//switch(MeshNumber)
	//{
	//	case 1:
	//	texColor=texture_diffuse1.Sample(samLinear, pin.texCoord+UVOffset);
	//	break;
	//	case 2:
	//	texColor=texture_diffuse2.Sample(samLinear, pin.texCoord+UVOffset);
	//	break;
	//	case 3:
	//	texColor=texture_diffuse3.Sample(samLinear, pin.texCoord+UVOffset);
	//	break;
	//}
///*begin get Normal*/
//	float3 tangentSpaceSample=NormalMap.Sample(samLinear, pin.texCoord).rgb;
//	float3 tangentSpaceNormal=2.0f*tangentSpaceSample-1.0f;

//	//Tangent space ��ǥ�踦 ���Ѵ�.
//	//Gram-Schmidt �˰��� ����.
//	float3 N=pin.Normal;
//	float3 T=normalize(pin.Tangent-dot(pin.Tangent, N)*N);
//	float3 B=cross(N,T);

//	//tangent space���� world space�� ���� ����� ���Ѵ�.
//	float3x3 tangentToWorld=float3x3(T,B,N);

//	//Normal�� tangent space���� world space�� ��ȯ�Ѵ�.
//	//calculate world space normal
//	float3 worldSpaceNormal=mul(tangentSpaceNormal, tangentToWorld);
//	worldSpaceNormal=normalize(worldSpaceNormal);
//	/*end getNormal*/

	//texColor to diffuse Color
	float4 diffuseColor=texColor;

	float3 resultColor=lerp(diffuseColor.rgb, float3(1.0f,0.0f,0.0f), AmbientColor.a-0.1f)*saturate(dot(pin.Normal,toLightVector));
	//float3 resultColor=AmbientColor.rgb;
	float4 retColor=float4(resultColor, 1.0f);
   return retColor;
}

technique11 ModelTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}
