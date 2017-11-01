#pragma once

#include"d3dApp.h"
#include"GeometryGenerator.h"
#include"RenderState.h"
#include<xnacollision.h>
#include"Entity.h"
#include"EntityShader.h"
class Bar :public Entity
{
private:
	XMFLOAT3 centerPos;
	XMFLOAT3 Direction;
	XMFLOAT3 Scale;
	XMFLOAT3 Rotation;
	bool DirectionChanged;
	float velocity;
	float LifeLength;
	float LifeTimeSum;
	BoxCollider collider;

	XMFLOAT2 UVOffset;
	XMFLOAT4 Ambient;
public:
	Bar(XMFLOAT3 Position, XMFLOAT3 Scale, float velocity, XMFLOAT3 Direction, float LifeLength)
	{

		this->LifeLength = LifeLength;
		DirectionChanged = false;
		LifeTimeSum = 0.0f;
		this->centerPos = Position;
		this->Direction = Direction;
		this->velocity = velocity;
		UVOffset = XMFLOAT2(0, 0);
		Ambient = XMFLOAT4(0, 0, 0, 0);
		Rotation = XMFLOAT3(0, 0, 0);
		collider.MinPoint = XMFLOAT3(-Scale.x / 2.0f, -Scale.y / 2.0f, -Scale.z / 2.0f);
		collider.MaxPoint = XMFLOAT3(Scale.x / 2.0f, Scale.y / 2.0f, Scale.z / 2.0f);
		this->Scale = Scale;
	}


	void Render(EntityShader* shader, Camera& cam, XMFLOAT3 lightPosition)
	{
		Global::Context()->IASetInputLayout(shader->InputLayout());
		Global::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//VertexBuffer와 Index Buffer를 설정한다.
		UINT stride = sizeof(EntityShader::EntityVertex);
		UINT offset = 0;
		ID3D11Buffer* pVB = shader->VB();
		ID3D11Buffer* pIB = shader->IB();
		Global::Context()->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
		Global::Context()->IASetIndexBuffer(pIB, DXGI_FORMAT_R32_UINT, 0);

		shader->getTech()->GetPassByIndex(0)->Apply(0, Global::Context());

		XMMATRIX TranslationMatrix = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
		XMMATRIX ScaleMatrix = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
		XMMATRIX RotMatrix = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
		XMMATRIX WorldMatrix = ScaleMatrix*RotMatrix*TranslationMatrix;
		//쉐이더에 데이터를 로딩한다.
		shader->LoadWorldViewProjMatrix(WorldMatrix*cam.View()*cam.Proj());
		shader->LoadWorldMatrix(WorldMatrix);
		shader->LoadWorldInvTranspose(WorldMatrix);
		shader->LoadLightPosition(lightPosition);
		shader->LoadUVOffset(XMFLOAT2(0.0f, 0.0f));
		shader->LoadAmbientColor(Ambient);

		//그린다.
		Global::Context()->DrawIndexed(shader->getIndexCount(), 0, 0);

		//렌더 상태를 원상복귀 시킨다.
		Global::finishRender();
	}
	//Procedure: SpherePlaneCheck
	//	Input(s) : Sphere, Plane
	//	Output(s) : Result
	//
	//	Distance = Sphere.Center Dot Plane.Normal + Plane.D
	//	if Distance >= Sphere.Radius
	//		Result = false
	//		Else
	//		Result = true
	//		// Now we know it's a collision, you can either 
	//		// stop your object (thus preventing it from penetrating 
	//		// the wall), or
	//		// make it slide along the wall (read Paul Nettle's 
	//		// article to know how. If you still can't figure out 
	//		// how, tell me)
	//		End Procedure

	void Update(float dt)
	{
		LifeTimeSum += dt;

		if (LifeTimeSum >= LifeLength / 2.0f && !DirectionChanged)
		{
			DirectionChanged = true;
			Direction.x *= -1;
			Direction.y *= -1;
			Direction.z *= -1;
		}

		Move(Direction, velocity, dt);
	}

	void SetPosition(XMFLOAT3 position)
	{
		this->centerPos = position;
	}

	void SetRotation(float xAngle, float yAngle, float zAngle)
	{
		Rotation = XMFLOAT3(xAngle, yAngle, zAngle);
	}

	void SetVelocity(float velocity)
	{
		this->velocity = velocity;
	}

	void SetDirection(XMFLOAT3 direction)
	{
		//normalize direction
		this->Direction = direction;

		float length = sqrtf(Direction.x*Direction.x + Direction.y*Direction.y + Direction.z*Direction.z);
	}

	void SetUVOffset(XMFLOAT2 UVOffset)
	{
		this->UVOffset = UVOffset;
	}
	void Move(XMFLOAT3 Direction, float Velocity, float dt)
	{
		float length = nsMath::length(Direction);

		if (length > 0.000000000001f)
		{
			Direction = XMFLOAT3(Direction.x / length, Direction.y / length, Direction.z / length);
		}

		centerPos.x += Direction.x*Velocity*dt;
		centerPos.y += Direction.y*Velocity*dt;
		centerPos.z += Direction.z*Velocity*dt;
	}
	XMFLOAT3 GetPosition()
	{
		return centerPos;
	}
	XMFLOAT3 GetRotation()
	{
		return Rotation;
	}
	XMFLOAT3 GetDirection()
	{
		return Direction;
	}
	float GetVelocity()
	{
		return velocity;
	}
	XMFLOAT2 GetUVOffset()
	{
		return UVOffset;
	}

	XMFLOAT4 GetAmbientColor()
	{
		return Ambient;
	}

	void SetAmbientColor(XMFLOAT3 color, float Alpha)
	{
		Ambient.x = color.x;
		Ambient.y = color.y;
		Ambient.z = color.z;
		Ambient.w = Alpha;
	}

	bool isDead()
	{
		if (LifeTimeSum >= LifeLength)
			return true;
		else
			return false;
	}

	BoxCollider GetBoxCollider()
	{
		BoxCollider retCollider;
		retCollider.MinPoint=XMFLOAT3(collider.MinPoint);
		retCollider.MaxPoint = XMFLOAT3(collider.MaxPoint);
		retCollider.MinPoint.x += centerPos.x;
		retCollider.MinPoint.y += centerPos.y;
		retCollider.MinPoint.z += centerPos.z;
		retCollider.MaxPoint.x += centerPos.x;
		retCollider.MaxPoint.y += centerPos.y;
		retCollider.MaxPoint.z += centerPos.z;

		return retCollider;
	}
};