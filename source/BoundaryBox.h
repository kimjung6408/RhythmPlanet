#pragma once
#include"d3dApp.h"
#include"GeometryGenerator.h"
#include"RenderState.h"
#include<xnacollision.h>
#include"Entity.h"
#include"EntityShader.h"
class BoundaryBox :public Entity
{
private:
	XMFLOAT3 Position;
	XMFLOAT3 Direction;
	XMFLOAT3 Rotation;
	XMFLOAT4 Ambient;
	XMFLOAT3 Scale;
	float velocity;

	XMFLOAT2 UVOffset;
public:
	BoundaryBox(XMFLOAT3 Position, XMFLOAT3 Scale)
	{
		this->Position = Position;
		this->Scale = Scale;
		Direction = XMFLOAT3(0, 0, 0);
		velocity = 0.0f;
		UVOffset = XMFLOAT2(0, 0);
		Ambient = XMFLOAT4(0, 0, 0, 0);
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

		XMMATRIX TranslationMatrix = XMMatrixTranslation(Position.x, Position.y, Position.z);
		XMMATRIX ScaleMatrix = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
		XMMATRIX WorldMatrix = ScaleMatrix*TranslationMatrix;
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
		//Move(Direction, velocity, dt);
	}

	void SetPosition(XMFLOAT3 position)
	{
		this->Position = position;
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
	}
	XMFLOAT3 GetPosition()
	{
		return Position;
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