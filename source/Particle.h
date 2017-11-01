#pragma once
#include"Camera.h"
#include"ParticleTexture.h"
#include"Utils.hpp"

#define PARTICLE_GRAVITY -50

class Particle
{
private:
	XMFLOAT3 position; //particle�� ��ġ
	XMFLOAT3 velocity; //particle�� �ӵ�.
	float gravityEffectFactor; //gravity�����. clamp[0,1]
	float lifeLength; //particle�� ���� �ð�.
	float rotation; //2���� ȸ��
	XMFLOAT3 scale; //ũ�� ����

	float elapsedTime; //����� �ð�. elapsed time>=lifeLength�̸� particle �Ҹ�

	float distanceFromCamera; //ī�޶������ �Ÿ�

	float blend; //������ �ؽ��� ȥ�� ����

					 //��ƼŬ �̹��� ����
	ParticleTexture* texture;

	//ȥ���� �ؽ��� ������ uv ��ǥ
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

	//�ð����� ���� �ؽ��ĸ� update�Ѵ�. lifetime �ʰ��� false, �ʰ����� ������ true( alive )
	bool update(Camera& cam, float dt) {
		float deltaTime = dt;
		//�߷�*�߷��� �ۿ��ϴ� ����*�帥�ð�(deltaTime)
		
		velocity.y += PARTICLE_GRAVITY*gravityEffectFactor*deltaTime;
		//�ӵ��� deltaTime��ŭ scaling �Ѵ�.
		XMFLOAT3 change = velocity;
		change = nsMath::Scaling(change, deltaTime);

		//change��ŭ ��ġ�� ��ȭ�� �ش�.
		position = nsMath::Add(position, change);

		//ī�޶���� distance�� ����Ѵ�. ������ ����
		distanceFromCamera = nsMath::length(nsMath::Subtract(cam.GetPosition(),position));
		distanceFromCamera *= distanceFromCamera;

		//elapedTime�� ���� �� �������ɽð��� ���Ѵ�.
		elapsedTime += deltaTime;

		//texture atlas�� � �ؽ��ĸ� ����
		//�ð����� ���� ������Ʈ�� �Ѵ�.
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
	//�ð����� ����
	//texture atlas �󿡼� � �ؽ��ĸ� ���� �����Ѵ�.
	void updateTextureCoordinate()
	{
		//texture interpolation�� ���� life factor
		float lifeFactor = elapsedTime / lifeLength;

		//texture atlas���� atomic image ����
		int stageCount = texture->getNumOfRows()*texture->getNumOfRows();

		//���� �������� image�� index�� ���ϱ� ���� ��
		float progressValue = lifeFactor*stageCount;

		//���� �������� �̹����� �ε���
		int index1 = (int)progressValue;

		//blend�� ���� ���� �̹����� �ε���
		int index2 = index1 < stageCount - 1 ? index1 + 1 : index1;

		setTextureOffset(textureOffset1, index1);
		setTextureOffset(textureOffset2, index2);
	}

	//updateTextureCoordinate���� ȣ���ϸ�, �ؽ����� �ε����� �����Ѵ�.
	void setTextureOffset(XMFLOAT2 offset, int index)
	{
		int column = index%texture->getNumOfRows();
		int row = index%texture->getNumOfRows();

		offset.x = (float)column / (float)texture->getNumOfRows();
		offset.y = (float)row / (float)texture->getNumOfRows();
	}

};