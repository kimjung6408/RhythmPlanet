#pragma once
#include"d3dApp.h"
#include"GeometryGenerator.h"
#include"RenderState.h"
#include<xnacollision.h>
#include"Entity.h"
#include"EntityShader.h"
class Ball :public Entity
{
private:
	float radius;
	XMFLOAT3 centerPos;
	XMFLOAT3 Direction;
	XMFLOAT3 Rotation;
	float velocity;

	XMFLOAT2 UVOffset;
	XMFLOAT4 Ambient;
public:
	Ball(float radius, XMFLOAT3 center, float velocity, XMFLOAT3 Direction)
	{

		this->radius = radius;
		this->centerPos = center;
		this->Direction = Direction;
		this->velocity = velocity;
		UVOffset = XMFLOAT2(0, 0);
		Ambient = XMFLOAT4(0, 0, 0, 0);
		Rotation = XMFLOAT3(0, 0, 0);
	}

	float GetRadius()
	{
		return radius;
	}


	void Render(EntityShader* shader, Camera& cam, XMFLOAT3 lightPosition)
	{
		Global::Context()->IASetInputLayout(shader->InputLayout());
		Global::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//VertexBuffer�� Index Buffer�� �����Ѵ�.
		UINT stride = sizeof(EntityShader::EntityVertex);
		UINT offset = 0;
		ID3D11Buffer* pVB = shader->VB();
		ID3D11Buffer* pIB = shader->IB();
		Global::Context()->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
		Global::Context()->IASetIndexBuffer(pIB, DXGI_FORMAT_R32_UINT, 0);

		shader->getTech()->GetPassByIndex(0)->Apply(0, Global::Context());

		XMMATRIX TranslationMatrix = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
		XMMATRIX ScaleMatrix = XMMatrixScaling(radius, radius, radius);
		XMMATRIX RotMatrix = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
		XMMATRIX WorldMatrix = ScaleMatrix*RotMatrix*TranslationMatrix;
		//���̴��� �����͸� �ε��Ѵ�.
		shader->LoadWorldViewProjMatrix(WorldMatrix*cam.View()*cam.Proj());
		shader->LoadWorldMatrix(WorldMatrix);
		shader->LoadWorldInvTranspose(WorldMatrix);
		shader->LoadLightPosition(lightPosition);
		shader->LoadUVOffset(XMFLOAT2(0.0f, 0.0f));
		shader->LoadAmbientColor(Ambient);

		//�׸���.
		Global::Context()->DrawIndexed(shader->getIndexCount(), 0, 0);

		//���� ���¸� ���󺹱� ��Ų��.
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
		float length=nsMath::length(Direction);

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

	void SetAmbientColor(XMFLOAT3 color, float Alpha)
	{
		Ambient.x = color.x;
		Ambient.y = color.y;
		Ambient.z = color.z;
		Ambient.w = Alpha;
	}

	XMFLOAT4 GetAmbientColor()
	{
		return Ambient;
	}


};