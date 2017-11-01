#pragma once
#include"ParticleManager.h"
#include"ParticleTexture.h"
#include"d3dApp.h"
#include"MathHelper.h"
#include"Utils.hpp"
//��ƼŬ �����͸� �����ϴ� Ŭ����
//���� �������� �����Ͽ� �̿��Ѵ�.
class ParticleEmitter
{
private:

	//�ʴ� ��ƼŬ ���� ����
	float ParticlesPerSecond;
	
	//��հ� ����
	float averageSpeed, averageLifelength;
	XMFLOAT3 averageScale;

	//�߷��� ������ �޴� ����. clamp[0,1]
	float gravityEffectFactor;

	//��ƼŬ�� ����Ǵ� ������� ����
	XMFLOAT3 direction;

	//��ƼŬ�� ����Ǵ� ���⿡ ���Ͽ� ������ �ְ��� ����
	float directionDeviation;

	//��ƼŬ�� ������ ȸ���� ������ ������ �ƴ����� ��Ÿ���� ����
	// true : ���� ȸ�� ����
	// false : ���� ȸ���� �������� ����
	bool randomRotation;

	//�ӵ��� ������ �ְ��� ����.
	float speedError;

	//���� ������ �ְ��� ����.
	float lifeError;

	//ũ�⿡ ������ �ְ��� ����.
	float scaleError;

	//��ƼŬ �ؽ��� ������
	ParticleTexture* texture;

private:

	//[-1,1] ������ ���� �����Ͽ� error���� �����ش�.
	XMFLOAT3 generateValue(XMFLOAT3 average, float errorMargin) {
		float offset = (MathHelper::RandF() - 0.5f) * 2.0f * errorMargin;
		XMFLOAT3 newScale = XMFLOAT3(average.x + offset, average.y + offset, average.z + offset);
		return newScale;
	}

	float generateValue(float average, float errorMargin) {
		float offset = (MathHelper::RandF() - 0.5f) * 2.0f * errorMargin;
		return average + offset;
	}

	//���� ȸ�� ������ ���Ѵ�.
	float generateRotation() {
		if (randomRotation) {
			return MathHelper::RandF() * 360.0f;
		}
		else {
			return 0;
		}
	}

	//���� ���� �̳����� ������ �����ϰ� �����Ѵ�.
	XMFLOAT3 generateRandomDirectionInCone(XMFLOAT3 coneDirection, float angle) {
		XMVECTOR coneDirVector = XMLoadFloat3(&coneDirection);
		XMVECTOR dirVector;

		if (angle == 0) return coneDirection;

		//ȸ���� ������ ���� �����ϰ� ������ ������.
		float cosAngle = cos(angle);
		float theta = (MathHelper::RandF() * 2.0f * MathHelper::Pi);
		float z = cosAngle + (MathHelper::RandF() * (1 - cosAngle));
		float rootOneMinusZSquared = sqrt(1 - z * z);
		float x = (rootOneMinusZSquared * cos(theta));
		float y = (rootOneMinusZSquared * sin(theta));
		XMFLOAT3 direction = XMFLOAT3(x, y, z);
		dirVector = XMLoadFloat3(&direction);

		if (coneDirection.x != 0 || coneDirection.y != 0 || (coneDirection.z != 1 && coneDirection.z != -1)) {
			XMFLOAT3 dirZ = XMFLOAT3(0, 0, 1);

			//�����ε�, �Լ� ȣ���� ������� �ʴ� ������ �־ ���� ����� ��.
			//ȸ������ ���
			XMFLOAT3 rotateAxis = XMFLOAT3(coneDirection.y*dirZ.z - coneDirection.z*dirZ.y, -(coneDirection.x*dirZ.z - coneDirection.x*dirZ.z), coneDirection.x*dirZ.y - coneDirection.x*dirZ.y);

			//Axis vector�� normalize��.
			float length = nsMath::length(rotateAxis);
			rotateAxis = XMFLOAT3(rotateAxis.x / length, rotateAxis.y / length, rotateAxis.z / length);

			//z��� direction vector�� �̷�� ������ ���.
			float rotateAngle = acos(nsMath::dot(coneDirection, XMFLOAT3(0, 0, 1)));

			//ȸ���� ����
			XMMATRIX rotationMatrix = XMMatrixIdentity();
			rotationMatrix = XMMatrixRotationAxis(XMLoadFloat3(&rotateAxis), -rotateAngle);
			XMVECTOR tmp = XMVector4Transform(XMLoadFloat3(&direction), rotationMatrix);
			XMStoreFloat3(&direction, tmp);
		}
		else if (coneDirection.z == -1) {
			direction.z *= -1;
		}
		return XMFLOAT3(direction.x, direction.y, direction.z);
	}

	XMFLOAT3 generateRandomUnitVector() {
		float theta = (float)(MathHelper::RandF() * 2.0f * MathHelper::Pi);
		float z = (MathHelper::RandF() * 2.0f) - 1;
		float rootOneMinusZSquared = (float)sqrtf(1 - z * z);
		float x = (float)(rootOneMinusZSquared * cosf(theta));
		float y = (float)(rootOneMinusZSquared * sinf(theta));

		return XMFLOAT3(x, y, z);
	}

	//��ƼŬ 1����(Atomic) �����Ѵ�.
	void emitAtomicParticle(XMFLOAT3 genPosition)
	{
		//[-1,1] ������ ���� direction value�� ���Ѵ�.
		float xDirection = MathHelper::RandF()*2.0f - 1.0f;
		float zDirection = MathHelper::RandF()*2.0f - 1.0f;

		//direction���� ���� �ӵ��� ������ ���Ѵ�.
		XMFLOAT3 velocity;

		if (randomRotation)
		velocity = generateRandomDirectionInCone(direction, MathHelper::Pi*directionDeviation);
		else
		{
			velocity = direction;
		}

		float velocityLength = nsMath::length(velocity);
		//normalise
		velocity = XMFLOAT3(velocity.x / velocityLength, velocity.y / velocityLength, velocity.z / velocityLength);
		float randomSpeedValue = generateValue(averageSpeed, speedError);
		velocity = XMFLOAT3(velocity.x*randomSpeedValue, velocity.y*randomSpeedValue, velocity.z*randomSpeedValue);

		//particle ũ�⸦ ���Ѵ�.
		XMFLOAT3 scale = generateValue(averageScale, scaleError);

		//��ƼŬ ������ ���Ѵ�.
		float lifeLength = generateValue(averageLifelength, lifeError);

		//��ƼŬ�� �����ϰ�, �Ŵ����� �����Ѵ�.
		ParticleManager::addParticle(Particle(texture, genPosition, velocity, gravityEffectFactor, lifeLength, generateRotation(), scale));
	}
public:

	/*
	direction : ��ƼŬ�� ����� ����
	ParticlePerSecond: �ʴ� ���� ��ƼŬ ����
	speed : ��ƼŬ �ӷ�
	gravityEffectFactor : gravity�� ������ �޴� ����
	lifeLength : ��ƼŬ ����
	scale : ��ƼŬ ũ��.
	*/
	ParticleEmitter(ParticleTexture* texture, XMFLOAT3 direction, float ParticlesPerSecond, float speed, float gravityEffectFactor, float lifeLength, XMFLOAT3 scale)
	{
		this->texture = texture;
		this->ParticlesPerSecond = ParticlesPerSecond;
		this->averageSpeed = speed;
		this->gravityEffectFactor = gravityEffectFactor;
		this->averageLifelength = lifeLength;
		this->averageScale = scale;
		this->direction = direction;
		
		randomRotation = false;

		//error������ ����Ʈ ����
		speedError = 0.1f;
		lifeError = 0.1f;
		scaleError = 0.1f;
		directionDeviation = 0.2f;
	}

	//��ƼŬ�� ����� ����� deviation�� �����Ѵ�.
	void setDirection(XMFLOAT3 direction, float deviation) {
		this->direction = direction;
		this->directionDeviation = (float)(deviation);
	}

	//��ƼŬ�� ũ�⸦ �����Ѵ�.
	void setScale(XMFLOAT3 scaleFactor, float scaleError)
	{
		this->averageScale = scaleFactor;
		this->scaleError = scaleError;
	}

	//��ƼŬ�� ������ �����Ѵ�.
	void setLifelength(float lifelength, float lifeError)
	{
		this->averageLifelength = lifelength;
		this->lifeError;
	}

	//rotation�� �������� �����Ѵ�.
	void randomizeRotation() {
		randomRotation = true;
	}

	void offfRandomRotation()
	{
		randomRotation = false;
	}

	//�ӵ��� ������ �ְ��� ����. 0~1������ ���� ������, 0�� �ְ��� ���� 1�� ����������� �ְ��� ũ��.
	void setSpeedError(float error) {
		this->speedError = error * averageSpeed;
	}

	//��ƼŬ�� ������ �������� �����Ѵ�.
	void emitParticles(XMFLOAT3 genPosition, float dt)
	{
		float NumOfParticlesToCreate = ParticlesPerSecond*dt;

		int NumOfParticles = (int)NumOfParticlesToCreate;
		for (int i = 0; i < NumOfParticles; i++)
		{
			emitAtomicParticle(genPosition);
		}
	}

	//��ƼŬ�� �ش� position���� �����Ѵ�.
	void generateParticles(XMFLOAT3 systemCenter, float dt) {
		float delta = dt;
		float particlesToCreate = ParticlesPerSecond * delta;
		int count = floorf(particlesToCreate);
		float partialParticle = particlesToCreate - count;
		for (int i = 0; i < count; i++) {
			emitAtomicParticle(systemCenter);
		}
		if (MathHelper::RandF() < partialParticle) {
			emitAtomicParticle(systemCenter);
		}
	}

};