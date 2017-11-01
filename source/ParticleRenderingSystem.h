#pragma once
#include"Particle.h"
#include"ParticleShader.h"
#include"Camera.h"
#include"Utils.hpp"
#include<map>

#define MAX_PARTICLE_NUMBERS 10000
#define INSTANCE_DATAS 21


//파티클을 렌더링하는 클래스
//ParticleManager 내부에서 이용한다.
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
		//파티클이 그려질 평면의 vertex 설정.
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
		//렌더링을 위한 쉐이더 환경설정을 한다.
		prepare();
		//카메라의 viewMatrix를 받아온다.
		XMStoreFloat4x4(&viewMatrix,cam.View());

		for (map < ParticleTexture*, vector<Particle>>::iterator iter = particles.begin(); iter != particles.end(); iter++)
		{		//쉐이더의 modelViewMatrix를 update한다.
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

				//VertexBuffer를 설정한다.
				UINT stride = sizeof(XMFLOAT3);
				UINT offset = 0;
				Global::Context()->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);

				//Context에 shader의 technique pass를 연결한다.
				shader->getTech()->GetPassByIndex(0)->Apply(0, Global::Context());
				
				//여러가지 factor들을 로딩한다.
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



		//렌더링을 마치고 쉐이더 환경설정을 초기화한다.
		finishRender();
	}


void prepare()
{
	//알파블렌딩 허용

	//depth masking을 허용하지 않음


}

void updateModelViewMatrix(XMFLOAT3 position, float rotation, XMFLOAT3 scaleFactor)
{
	//modelMatrix의 0x0~2x2 성분이
	//viewMatrix의 0x0~2x2 성분의 transpose가 되게 하여
	//x,y 축 회전을 적용하지 않는다 ->front face가 항상 카메라 방향을 향하게 됨

	//translate 연산
	XMStoreFloat4x4(&modelMatrix,XMMatrixTranslation(position.x,position.y,position.z));

	//1x1~3x3 transpose 연산
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

	//z회전만 적용한다.
	XMFLOAT3 axis = XMFLOAT3(0, 0, 1);
	XMVECTOR rotAxis = XMVectorSet(0, 0, 1, 0);
	XMMATRIX rotMatrix=XMMatrixRotationAxis(rotAxis, XMConvertToRadians(rotation));

	//scaling
	XMMATRIX scaleMatrix=XMMatrixScaling(scaleFactor.x, scaleFactor.y, scaleFactor.z);
	
	//m=SRT;
	XMStoreFloat4x4(&modelViewMatrix ,scaleMatrix*rotMatrix*XMLoadFloat4x4(&modelViewMatrix));
}

//렌더링을 마친 후, 쉐이더 환경설정을 초기화.
void finishRender()
{
//depth test 초기화
	Global::Context()->OMSetDepthStencilState(0, 0);
//blend state 초기화
	Global::Context()->OMSetBlendState(0, 0, 0xffffffff);
}

};