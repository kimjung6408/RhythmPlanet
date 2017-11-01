#pragma once
#include"d3dApp.h"
#include"Utils.hpp"
#include"globalDeviceContext.h"
#include"GeometryGenerator.h"
#include"Camera.h"
#include"TriangleShader.h"
class Triangle
{
private:
	XMFLOAT3 positionAngle;
	XMFLOAT3 rotationAngle;
	float distanceFromCenter;
	float centerXPos;
public:
	Triangle(float centerXPos)
	{
		this->positionAngle = XMFLOAT3(MathHelper::RandF()*MathHelper::Pi / 4.0f, MathHelper::RandF()*MathHelper::Pi / 2.0f, MathHelper::RandF()*2.0f*MathHelper::Pi);;
		this->rotationAngle = XMFLOAT3(MathHelper::RandF()*MathHelper::Pi / 4.0f, MathHelper::RandF()*MathHelper::Pi , MathHelper::RandF()*2.0f*MathHelper::Pi);
		this->distanceFromCenter = MathHelper::Clamp(8.0f*MathHelper::RandF(), 2.0f, 8.0f);
		this->centerXPos = centerXPos;
	}

	void Render(Camera& cam, TriangleShader* shader)
	{
		Global::Context()->RSSetState(RenderState::NoCullRS);
		Global::Context()->IASetInputLayout(shader->InputLayout());
		Global::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT stride = sizeof(XMFLOAT3);
		UINT offset = 0;
		ID3D11Buffer* mVB = shader->VB();
		Global::Context()->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);


		shader->getTech()->GetPassByIndex(0)->Apply(0, Global::Context());
		XMMATRIX localMatrix = XMMatrixScaling(0.8f, 0.8f, 0.8f)*XMMatrixRotationRollPitchYaw(-rotationAngle.x, rotationAngle.y, -rotationAngle.z)*XMMatrixTranslation(centerXPos,0,4);
		XMMATRIX moveMatrix = XMMatrixTranslation(distanceFromCenter,0.0f,0.0f)*XMMatrixRotationRollPitchYaw(positionAngle.x, positionAngle.y, positionAngle.z+MathHelper::Pi);
		XMMATRIX WorldViewProj = (moveMatrix*localMatrix)*cam.ViewProj();
		shader->loadModelViewProjMatrix(WorldViewProj);


		Global::Context()->Draw(3, 0);

		Global::finishRender();
	}

	void Rotate(float xAngle, float yAngle, float zAngle)
	{
		rotationAngle.x += xAngle;
		rotationAngle.y += yAngle;
		rotationAngle.z += zAngle;
	}

	void Update(float dt)
	{
		Rotate(MathHelper::RandF()*MathHelper::Pi*dt, -(dt *1.2f)*MathHelper::Pi-MathHelper::RandF()*dt, cosf(MathHelper::RandF())*dt*MathHelper::Pi);
		distanceFromCenter +=dt*(MathHelper::RandF()*cosf(dt)-0.5f);
		distanceFromCenter = MathHelper::Clamp(distanceFromCenter, 3.0f, 8.0f);
	}
};