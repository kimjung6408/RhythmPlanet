#pragma once
#include"d3dApp.h"
#include"Utils.hpp"
#include"globalDeviceContext.h"
#include"GeometryGenerator.h"
#include"Camera.h"
#include"FontShader.h"
#include"RenderState.h"
#define BLING_TIME 0.4f
#define TEXT_ALIGN_LEFT 0
#define TEXT_ALIGN_RIGHT 1
class Text
{
private:
	XMFLOAT2 Position;
	XMFLOAT2 Scale;
	XMFLOAT3 Rotation;

	//int NumOfRows;
	//int NumOfCols;

	bool Fade;
	float FadeTimeDifference;
	float FadeTimeSum;
	float FadeFactor;
	float Alpha;
	string text;
	//fadeFactor=(FadeTimeSum/FadeTimeDifference);
	//Color의 색상은 Color*=clamp(1-FadeFactor, 0.1,1.0);
	//int CurrentRowPos;
	//int CurrentColPos;
	float characterGap;
	int TextAlign;
public:
	Text() {}

	Text(XMFLOAT2 Position, XMFLOAT2 Scale, string text)
	{
			this->Position = Position;
		this->Scale = Scale;
		this->Fade = false;
		this->FadeFactor = 0;
		this->FadeTimeDifference = 0.4f;
		this->FadeTimeSum = 0;
		this->Rotation = XMFLOAT3(0, 0, 0);
		this->Alpha = 1.0f;
		this->text = text;
		TextAlign = TEXT_ALIGN_LEFT;
		characterGap = 0.06f;
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

	void Render(FontShader* shader)
	{
		Global::Context()->RSSetState(RenderState::NoCullRS);
		Global::Context()->IASetInputLayout(shader->InputLayout());
		Global::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT stride = sizeof(XMFLOAT2);
		UINT offset = 0;
		ID3D11Buffer* pVB = shader->VB();
		Global::Context()->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);

		//Load world Matrix (position, scale 설정)

		shader->LoadFadeFactor(FadeFactor);
		shader->LoadAlpha(Alpha);

		float alignOffset = 0.0f;
		if (TextAlign == TEXT_ALIGN_RIGHT)
		{
			alignOffset = (-characterGap*(float)(text.size()-1));
		}

		for (int i = 0; i < text.size(); i++)
		{

			shader->LoadCharacter(text[i]);
			XMMATRIX PosMatrix = XMMatrixTranslation(alignOffset+Position.x+characterGap*(float)i, Position.y, 0);
			XMMATRIX rotMatrix = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
			XMMATRIX ScaleMatrix = XMMatrixScaling(Scale.x, Scale.y, 0);
			XMMATRIX WorldMatrix = ScaleMatrix*rotMatrix*PosMatrix;
			shader->LoadWorldMatrix(WorldMatrix);
			shader->getTech()->GetPassByIndex(0)->Apply(0, Global::Context());


			Global::Context()->Draw(6, 0);
		}

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

	void SetText(string txt)
	{
		text = txt;
	}

	//TEXT_ALIGN_LEFT
	//TEXT_ALIGN_RIGHT
	void SetAlign(int align)
	{
		if(align==TEXT_ALIGN_LEFT || align==TEXT_ALIGN_RIGHT)
		TextAlign = align;
	}

	float getFadeFactor()
	{
		return FadeFactor;
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

	void SetCharacterGap(float gap)
	{
		characterGap = gap;
	}

	~Text()
	{

	}
};