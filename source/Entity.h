#pragma once
#include"d3dApp.h"
#include"EntityShader.h"
#include"Camera.h"
class Entity
{
private:
	//3D Animation 함수를 아래에 기술.
	//blar....

public:
	virtual void Update(float dt) = 0;
	virtual void Render(EntityShader* shader, Camera& cam, XMFLOAT3 lightPosition) = 0;
	virtual void SetPosition(XMFLOAT3 position) = 0;
	virtual void SetVelocity(float velocity) = 0;
	virtual void SetDirection(XMFLOAT3 direction)=0;
	virtual void SetUVOffset(XMFLOAT2 UVOffset) = 0;
	virtual void Move(XMFLOAT3 Direction, float Velocity, float dt) = 0;
	virtual XMFLOAT3 GetPosition() = 0;
	virtual XMFLOAT3 GetRotation() = 0;
	virtual	XMFLOAT3 GetDirection() = 0;
	virtual float GetVelocity() = 0;
	virtual XMFLOAT2 GetUVOffset() = 0;
	virtual void SetAmbientColor(XMFLOAT3 Color, float Alpha) = 0;
	virtual XMFLOAT4 GetAmbientColor() = 0;
};