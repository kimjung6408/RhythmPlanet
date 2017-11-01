#include"samplerState.hlsl"

float4 getAmbientColor(float4 ambientColor, float intensity)
{
	return intensity*ambientColor;
}

float4 getDiffuseColor(float4 diffuseColor, float3 LightDirection, float3 Normal, float intensity)
{
	float3 LightDirUnit=normalize(LightDirection);
	float3 normalUnit=normalize(Normal);
	float shine=saturate(dot(-LightDirUnit, normalUnit ))*intensity;
	return diffuseColor*shine;
}

float4 getSpecularColor(float4 specularColor, float3 toCamera, float3 Normal, float shineDamper, float intensity)
{
	float3 toCamUnit=normalize(toCamera);
	float3 normalUnit=normalize(Normal);
	float specularIntensity=pow(saturate(dot(toCamUnit,normalUnit)), shineDamper) * intensity;
	return specularColor*specularIntensity;
}