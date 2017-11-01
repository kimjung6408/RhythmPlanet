#pragma once
#include"d3dApp.h"
#include"Utils.hpp"
#include"globalDeviceContext.h"
#include"GeometryGenerator.h"
#include"Camera.h"
#include"GUIShader.h"
#define BLING_TIME 0.4f;
class GUI
{
private:
	XMFLOAT2 Position;
	XMFLOAT2 Scale;
	XMFLOAT3 Rotation;

	ID3D11ShaderResourceView* SRV;
	//int NumOfRows;
	//int NumOfCols;

	bool Fade;
	float FadeTimeDifference;
	float FadeTimeSum;
	float FadeFactor;
	float Alpha;
	XMFLOAT2 UVOffset;
	//fadeFactor=(FadeTimeSum/FadeTimeDifference);
	//Color의 색상은 Color*=clamp(1-FadeFactor, 0.1,1.0);
	//int CurrentRowPos;
	//int CurrentColPos;

public:
	GUI() {}
	GUI(LPCWSTR imagePath, XMFLOAT2 Position, XMFLOAT2 Scale)
		:SRV(0)
	{
		HR(D3DX11CreateShaderResourceViewFromFileW(Global::Device(), imagePath, 0, 0, &SRV, 0));
			this->Position = Position;
		this->Scale = Scale;
		this->Fade = false;
		this->FadeFactor = 0;
		this->FadeTimeDifference = 0.4f;
		this->FadeTimeSum = 0;
		this->Rotation = XMFLOAT3(0, 0, 0);
		this->Alpha = 1.0f;
		this->UVOffset = XMFLOAT2(0, 0);
	}

	void Update(float dt)
	{
		if (Fade)
		{
			FadeTimeSum += dt;

			if (FadeTimeSum >= FadeTimeDifference)
			{
				FadeTimeSum -= FadeTimeDifference;
			}

			FadeFactor = (FadeTimeSum / FadeTimeDifference);
		}
		else
		{
			FadeTimeSum = 0;
			FadeFactor = 0;
		}
	}

	void Render(GUIShader* shader)
	{
		Global::Context()->RSSetState(RenderState::NoCullRS);
		Global::Context()->IASetInputLayout(shader->InputLayout());
		Global::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT stride = sizeof(XMFLOAT2);
		UINT offset = 0;
		ID3D11Buffer* pVB = shader->VB();
		Global::Context()->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);

		//Load world Matrix (position, scale 설정)
		XMMATRIX PosMatrix = XMMatrixTranslation(Position.x, Position.y, 0);
		XMMATRIX rotMatrix = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
		XMMATRIX ScaleMatrix = XMMatrixScaling(Scale.x, Scale.y, 0);
		XMMATRIX WorldMatrix = ScaleMatrix*rotMatrix*PosMatrix;
		shader->LoadWorldMatrix(WorldMatrix);

		shader->LoadTexture(SRV);
		shader->LoadFadeFactor(FadeFactor);
		shader->LoadAlpha(Alpha);
		shader->LoadUVOffset(UVOffset);

		shader->getTech()->GetPassByIndex(0)->Apply(0, Global::Context());


		Global::Context()->Draw(6, 0);

		Global::finishRender();
	}

	void SetScale(XMFLOAT2 scale)
	{
		this->Scale = scale;
	}

	void fade(bool OnOff)
	{
		Fade = OnOff;
	}

	float getFadeFactor()
	{
		return FadeFactor;
	}

	XMFLOAT2 GetPosition()
	{
		return Position;
	}

	XMFLOAT3 GetRotation()
	{
		return Rotation;
	}

	XMFLOAT2 GetScale()
	{
		return Scale;
	}

	void UpdateRotate(float xAngle, float yAngle, float zAngle)
	{
		Rotation.x += xAngle;
		Rotation.y += yAngle;
		Rotation.z += zAngle;
	}

	void movePosition(float xDelta, float yDelta)
	{
		Position.x += xDelta;
		Position.y += yDelta;
	}

	void SetPosition(float x, float y)
	{
		Position.x = x;
		Position.y = y;
	}

	void SetAlpha(float alpha)
	{
		this->Alpha = alpha;
	}

	void SetUVOffset(XMFLOAT2 uvOffset)
	{
		this->UVOffset = uvOffset;
	}

	void LoadSRV(ID3D11ShaderResourceView* inSRV, bool deleteShaderResourceView)
	{
		if (deleteShaderResourceView)
			ReleaseCOM(SRV);


		SRV = inSRV;
	}

	void SetRotation(XMFLOAT3 Rotation)
	{
		this->Rotation = Rotation;
	}

	~GUI()
	{

	}
};