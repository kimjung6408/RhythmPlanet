#pragma once
#include"Particle.h"
#include"ParticleShader.h"
#include"Camera.h"
#include"Utils.hpp"
#include<map>

#define MAX_PARTICLE_NUMBERS 10000
#define INSTANCE_DATAS 21


//��ƼŬ�� �������ϴ� Ŭ����
//ParticleManager ���ο��� �̿��Ѵ�.
class ParticleRenderingSystem
{
private:
	ID3D11Buffer* mVB;
	static const int VERTEX_COUNT = 6;

	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 modelMatrix;
	XMFLOAT4X4 modelViewMatrix;
public:
	ParticleRenderingSystem()
	{
		//��ƼŬ�� �׷��� ����� vertex ����.
		XMFLOAT3 v3(-0.5f, 0.5f, 0);
		XMFLOAT3 v2(-0.5f, -0.5f, 0);
		XMFLOAT3 v1(0.5f, 0.5f, 0);
		XMFLOAT3 v6(0.5f, 0.5f, 0);
		XMFLOAT3 v5(-0.5f, -0.5f, 0);
		XMFLOAT3 v4(0.5f, -0.5f, 0);

		vector<XMFLOAT3> vertices;
		//vertices.push_back(v4);
		vertices.push_back(v1);
		vertices.push_back(v2);
		vertices.push_back(v3);
		vertices.push_back(v4);
		vertices.push_back(v5);
		vertices.push_back(v6);

		mVB = nsCreator::createVertexBuffer(vertices);
	}


	void render(ParticleShader* shader, Camera& cam, map < ParticleTexture*, vector<Particle>>& particles)
	{
		//�������� ���� ���̴� ȯ�漳���� �Ѵ�.
		prepare();
		//ī�޶��� viewMatrix�� �޾ƿ´�.
		XMStoreFloat4x4(&viewMatrix,cam.View());

		for (map < ParticleTexture*, vector<Particle>>::iterator iter = particles.begin(); iter != particles.end(); iter++)
		{		//���̴��� modelViewMatrix�� update�Ѵ�.
			vector<Particle>& particleList = iter->second;

			for (unsigned int i = 0; i < particleList.size(); i++)
			{
				Particle& p = particleList[i];
				updateModelViewMatrix(p.getPosition(),
									  p.getRotation(),
									  p.getScale()
									 );

				Global::Context()->IASetInputLayout(shader->InputLayout());
				Global::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				//VertexBuffer�� �����Ѵ�.
				UINT stride = sizeof(XMFLOAT3);
				UINT offset = 0;
				Global::Context()->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);

				//Context�� shader�� technique pass�� �����Ѵ�.
				shader->getTech()->GetPassByIndex(0)->Apply(0, Global::Context());
				
				//�������� factor���� �ε��Ѵ�.
				shader->loadModelViewProjMatrix(XMLoadFloat4x4(&modelViewMatrix)*cam.Proj());
				shader->loadTexture(p.getTexture()->getSRV());
				shader->loadBlendFactor(p.getBlendFactor());
				shader->loadTextureOffset1(p.getTexOffset1());
				shader->loadTextureOffset2(p.getTexOffset2());
				shader->loadNumberOfRows(iter->first->getNumOfRows());

				Global::Context()->Draw(VERTEX_COUNT, 0);
			}

			Global::finishRender();
		}



		//�������� ��ġ�� ���̴� ȯ�漳���� �ʱ�ȭ�Ѵ�.
		finishRender();
	}


void prepare()
{
	//���ĺ��� ���

	//depth masking�� ������� ����


}

void updateModelViewMatrix(XMFLOAT3 position, float rotation, XMFLOAT3 scaleFactor)
{
	//modelMatrix�� 0x0~2x2 ������
	//viewMatrix�� 0x0~2x2 ������ transpose�� �ǰ� �Ͽ�
	//x,y �� ȸ���� �������� �ʴ´� ->front face�� �׻� ī�޶� ������ ���ϰ� ��

	//translate ����
	XMStoreFloat4x4(&modelMatrix,XMMatrixTranslation(position.x,position.y,position.z));

	//1x1~3x3 transpose ����
	modelMatrix._11 = viewMatrix._11;
	modelMatrix._12 = viewMatrix._21;
	modelMatrix._13 = viewMatrix._31;
	modelMatrix._21 = viewMatrix._12;
	modelMatrix._22 = viewMatrix._22;
	modelMatrix._23 = viewMatrix._32;
	modelMatrix._31 = viewMatrix._13;
	modelMatrix._32 = viewMatrix._23;
	modelMatrix._33 = viewMatrix._33;

	XMStoreFloat4x4(&modelViewMatrix ,XMLoadFloat4x4(&modelMatrix)*XMLoadFloat4x4(&viewMatrix));

	//zȸ���� �����Ѵ�.
	XMFLOAT3 axis = XMFLOAT3(0, 0, 1);
	XMVECTOR rotAxis = XMVectorSet(0, 0, 1, 0);
	XMMATRIX rotMatrix=XMMatrixRotationAxis(rotAxis, XMConvertToRadians(rotation));

	//scaling
	XMMATRIX scaleMatrix=XMMatrixScaling(scaleFactor.x, scaleFactor.y, scaleFactor.z);
	
	//m=SRT;
	XMStoreFloat4x4(&modelViewMatrix ,scaleMatrix*rotMatrix*XMLoadFloat4x4(&modelViewMatrix));
}

//�������� ��ģ ��, ���̴� ȯ�漳���� �ʱ�ȭ.
void finishRender()
{
//depth test �ʱ�ȭ
	Global::Context()->OMSetDepthStencilState(0, 0);
//blend state �ʱ�ȭ
	Global::Context()->OMSetBlendState(0, 0, 0xffffffff);
}

};