#pragma once
#include"d3dApp.h"
#include"Camera.h"
#include"EntityShader.h"
//CAUTION_TYPE_LEFT : ���� ����
//CAUTION_TYPE_RIGHT : ������ ����
//CAUTION_TYPE_FRONT  : �̹����� ������ ����
//CAUTION_TYPE_FLOOR : �̹����� �ٴڸ鿡�� ��µǴ� ���
#define CAUTION_TYPE_LEFT 0
#define CAUTION_TYPE_RIGHT 1
#define CAUTION_TYPE_FRONT 2
#define CAUTION_TYPE_FLOOR 3
class CautionMark
{
private:
	float lifeLength;
	XMFLOAT3 Position;
	float rotationAngle;
	float Alpha;
	XMFLOAT3 RotAxis;
	float lifeTimeSum;
	XMFLOAT4 Ambient;
	int frontType;
public:

	CautionMark()
	{
		lifeLength = 30.0f / 150.0f;
		this->Alpha = 0.0f;
		this->rotationAngle = 0.0f;
		this->Position = XMFLOAT3(0, 1, 0);
		this->RotAxis = XMFLOAT3(0, 1, 0);
		lifeTimeSum = 0.0f;
		Ambient = XMFLOAT4(0, 0, 0, 0);
		frontType = CAUTION_TYPE_FRONT;
	}

	//CAUTION_TYPE_LEFT : ���� ����
	//CAUTION_TYPE_RIGHT : ������ ����
	//CAUTION_TYPE_FRONT  : �̹����� ������ ����
	//CAUTION_TYPE_FLOOR : �̹����� �ٴڸ鿡�� ��µǴ� ���
	CautionMark(float BPM, XMFLOAT3 Position, XMFLOAT3 RotationAxis/*, int imageFrontType*/)
	{
		lifeLength = 120.0f / BPM;
		this->Alpha = 0.0f;
		this->rotationAngle = rotationAngle;
		this->Position = Position;
		this->RotAxis = RotationAxis;
		lifeTimeSum = 0.0f;
		Ambient = XMFLOAT4(0, 0, 0, 0);
	}

	//cautionmark�� ������ ���ߴ����� bool�� �����Ѵ�.
	//true : alive.
	//false : dead. ������Ʈ ����.
	bool Update(float dt)
	{
		bool isAlive = true;
		lifeTimeSum += dt;
		
		//if (lifeTimeSum >= lifeLength)
		//{
		//	isAlive = false;
		//	return isAlive;
		//}
		
		float lifeRatio = lifeTimeSum / lifeLength;
		
		rotationAngle = MathHelper::Pi*lifeRatio;
		//Alpha = lifeRatio;
		Alpha = 1.0f;

		return isAlive;
	}

	void Render(EntityShader* shader, Camera& cam, XMFLOAT3 lightPosition)
	{
		Global::Context()->IASetInputLayout(shader->InputLayout());
		Global::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Global::Context()->RSSetState(RenderState::NoCullRS);

		//VertexBuffer�� Index Buffer�� �����Ѵ�.
		UINT stride = sizeof(EntityShader::EntityVertex);
		UINT offset = 0;
		ID3D11Buffer* pVB = shader->VB();
		ID3D11Buffer* pIB = shader->IB();
		Global::Context()->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
		Global::Context()->IASetIndexBuffer(pIB, DXGI_FORMAT_R32_UINT, 0);

		shader->getTech()->GetPassByIndex(0)->Apply(0, Global::Context());

		XMMATRIX TranslationMatrix = XMMatrixTranslation(Position.x, Position.y+1, Position.z);
		XMMATRIX ScaleMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);
		XMMATRIX PreRot;

		if (RotAxis.x >= 0.5f)
		{
			PreRot = XMMatrixRotationRollPitchYaw(0, 0, MathHelper::Pi / 2.0f);
		}
		else if (RotAxis.z >= 0.5f)
		{
			PreRot = XMMatrixRotationRollPitchYaw(MathHelper::Pi / 2.0f, 0, 0);
		}
		else
			PreRot = XMMatrixIdentity();

		XMFLOAT4 tmpRotAxis = XMFLOAT4(RotAxis.x, RotAxis.y, RotAxis.z, 1);
		XMMATRIX RotMatrix = XMMatrixRotationAxis(XMLoadFloat4(&tmpRotAxis), rotationAngle);
		//XMMATRIX RotMatrix = //XMMatrixRotationRollPitchYaw(0, 0, rotationAngle);
		XMMATRIX WorldMatrix = ScaleMatrix*PreRot*RotMatrix*TranslationMatrix;//RotMatrix*TranslationMatrix;
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

	bool isDead()
	{
		if (lifeLength <= lifeTimeSum)
			return true;
		else
			return false;
	}
};