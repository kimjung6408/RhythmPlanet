#pragma once
#include"Camera.h"
#include"ParticleTexture.h"
#include"Utils.hpp"

#define PARTICLE_GRAVITY -50

class Particle
{
private:
	XMFLOAT3 position; //particle의 위치
	XMFLOAT3 velocity; //particle의 속도.
	float gravityEffectFactor; //gravity적용률. clamp[0,1]
	float lifeLength; //particle의 생존 시간.
	float rotation; //2차원 회전
	XMFLOAT3 scale; //크기 비율

	float elapsedTime; //경과된 시간. elapsed time>=lifeLength이면 particle 소멸

	float distanceFromCamera; //카메라까지의 거리

	float blend; //인접한 텍스쳐 혼합 비율

					 //파티클 이미지 파일
	ParticleTexture* texture;

	//혼합할 텍스쳐 각각의 uv 좌표
	XMFLOAT2 textureOffset1;
	XMFLOAT2 textureOffset2;

public:
	Particle(ParticleTexture* texture, XMFLOAT3 position, XMFLOAT3 velocity, float gravityEffectFactor, float lifeLength, float rotation, XMFLOAT3 scale)
	{
		this->texture = texture;
		this->position = position;
		this->velocity = velocity;
		this->gravityEffectFactor = gravityEffectFactor;
		this->lifeLength = lifeLength;
		this->rotation = rotation;
		this->scale = scale;
		blend = 0;
		elapsedTime = 0;
		distanceFromCamera = 0;
		textureOffset1 = XMFLOAT2(0, 0);
		textureOffset2 = XMFLOAT2(0, 0);
	}

	XMFLOAT3 getPosition() { return position; }
	float getRotation() { return rotation; }
	XMFLOAT3 getScale() { return scale; }

	//시간값에 따라 텍스쳐를 update한다. lifetime 초과시 false, 초과하지 않으면 true( alive )
	bool update(Camera& cam, float dt) {
		float deltaTime = dt;
		//중력*중력이 작용하는 정도*흐른시간(deltaTime)
		
		velocity.y += PARTICLE_GRAVITY*gravityEffectFactor*deltaTime;
		//속도를 deltaTime만큼 scaling 한다.
		XMFLOAT3 change = velocity;
		change = nsMath::Scaling(change, deltaTime);

		//change만큼 위치에 변화를 준다.
		position = nsMath::Add(position, change);

		//카메라와의 distance를 계산한다. 길이의 제곱
		distanceFromCamera = nsMath::length(nsMath::Subtract(cam.GetPosition(),position));
		distanceFromCamera *= distanceFromCamera;

		//elapedTime을 높여 총 생존가능시간과 비교한다.
		elapsedTime += deltaTime;

		//texture atlas상에 어떤 텍스쳐를 쓸지
		//시간값에 따른 업데이트를 한다.
		updateTextureCoordinate();

		if (elapsedTime < lifeLength)
			return true;
		else
			return false;
	}

	ParticleTexture* getTexture()
	{
		return texture;
	}
	float getDistanceFromCamera()
	{
		return distanceFromCamera;
	}

	float getBlendFactor()
	{
		return blend;
	}

	XMFLOAT2 getTexOffset1()
	{
		return textureOffset1;
	}

	XMFLOAT2 getTexOffset2()
	{
		return textureOffset2;
	}
private:
	//시간값에 따라
	//texture atlas 상에서 어떤 텍스쳐를 쓸지 결정한다.
	void updateTextureCoordinate()
	{
		//texture interpolation을 위한 life factor
		float lifeFactor = elapsedTime / lifeLength;

		//texture atlas상의 atomic image 개수
		int stageCount = texture->getNumOfRows()*texture->getNumOfRows();

		//현재 진행중인 image의 index를 구하기 위한 값
		float progressValue = lifeFactor*stageCount;

		//먼저 진행중인 이미지의 인덱스
		int index1 = (int)progressValue;

		//blend할 나중 진행 이미지의 인덱스
		int index2 = index1 < stageCount - 1 ? index1 + 1 : index1;

		setTextureOffset(textureOffset1, index1);
		setTextureOffset(textureOffset2, index2);
	}

	//updateTextureCoordinate에서 호출하며, 텍스쳐의 인덱스를 설정한다.
	void setTextureOffset(XMFLOAT2 offset, int index)
	{
		int column = index%texture->getNumOfRows();
		int row = index%texture->getNumOfRows();

		offset.x = (float)column / (float)texture->getNumOfRows();
		offset.y = (float)row / (float)texture->getNumOfRows();
	}

};