#pragma once
#include"ParticleManager.h"
#include"ParticleTexture.h"
#include"d3dApp.h"
#include"MathHelper.h"
#include"Utils.hpp"
//파티클 데이터를 관리하는 클래스
//메인 루프에서 생성하여 이용한다.
class ParticleEmitter
{
private:

	//초당 파티클 생성 개수
	float ParticlesPerSecond;
	
	//평균값 정의
	float averageSpeed, averageLifelength;
	XMFLOAT3 averageScale;

	//중력의 영향을 받는 정도. clamp[0,1]
	float gravityEffectFactor;

	//파티클이 방출되는 평균적인 방향
	XMFLOAT3 direction;

	//파티클이 방출되는 방향에 대하여 가해질 왜곡의 정도
	float directionDeviation;

	//파티클에 랜덤한 회전을 적용할 것인지 아닌지를 나타내는 변수
	// true : 랜덤 회전 적용
	// false : 랜덤 회전을 적용하지 않음
	bool randomRotation;

	//속도에 가해질 왜곡의 정도.
	float speedError;

	//수명에 가해질 왜곡의 정도.
	float lifeError;

	//크기에 가해질 왜곡의 정도.
	float scaleError;

	//파티클 텍스쳐 포인터
	ParticleTexture* texture;

private:

	//[-1,1] 사이의 값을 생성하여 error값을 구해준다.
	XMFLOAT3 generateValue(XMFLOAT3 average, float errorMargin) {
		float offset = (MathHelper::RandF() - 0.5f) * 2.0f * errorMargin;
		XMFLOAT3 newScale = XMFLOAT3(average.x + offset, average.y + offset, average.z + offset);
		return newScale;
	}

	float generateValue(float average, float errorMargin) {
		float offset = (MathHelper::RandF() - 0.5f) * 2.0f * errorMargin;
		return average + offset;
	}

	//랜덤 회전 각도를 구한다.
	float generateRotation() {
		if (randomRotation) {
			return MathHelper::RandF() * 360.0f;
		}
		else {
			return 0;
		}
	}

	//원뿔 구간 이내에서 방향을 랜덤하게 생성한다.
	XMFLOAT3 generateRandomDirectionInCone(XMFLOAT3 coneDirection, float angle) {
		XMVECTOR coneDirVector = XMLoadFloat3(&coneDirection);
		XMVECTOR dirVector;

		if (angle == 0) return coneDirection;

		//회전각 설정에 따라 랜덤하게 방향을 생성함.
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

			//외적인데, 함수 호출이 적용되지 않는 현상이 있어서 직접 계산을 함.
			//회전축을 계산
			XMFLOAT3 rotateAxis = XMFLOAT3(coneDirection.y*dirZ.z - coneDirection.z*dirZ.y, -(coneDirection.x*dirZ.z - coneDirection.x*dirZ.z), coneDirection.x*dirZ.y - coneDirection.x*dirZ.y);

			//Axis vector를 normalize함.
			float length = nsMath::length(rotateAxis);
			rotateAxis = XMFLOAT3(rotateAxis.x / length, rotateAxis.y / length, rotateAxis.z / length);

			//z축과 direction vector가 이루는 각도를 계산.
			float rotateAngle = acos(nsMath::dot(coneDirection, XMFLOAT3(0, 0, 1)));

			//회전을 적용
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

	//파티클 1개를(Atomic) 방출한다.
	void emitAtomicParticle(XMFLOAT3 genPosition)
	{
		//[-1,1] 사이의 값을 direction value로 정한다.
		float xDirection = MathHelper::RandF()*2.0f - 1.0f;
		float zDirection = MathHelper::RandF()*2.0f - 1.0f;

		//direction값에 따른 속도의 방향을 구한다.
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

		//particle 크기를 구한다.
		XMFLOAT3 scale = generateValue(averageScale, scaleError);

		//파티클 수명을 구한다.
		float lifeLength = generateValue(averageLifelength, lifeError);

		//파티클을 생성하고, 매니저에 전달한다.
		ParticleManager::addParticle(Particle(texture, genPosition, velocity, gravityEffectFactor, lifeLength, generateRotation(), scale));
	}
public:

	/*
	direction : 파티클이 분출될 방향
	ParticlePerSecond: 초당 생성 파티클 개수
	speed : 파티클 속력
	gravityEffectFactor : gravity의 영향을 받는 정도
	lifeLength : 파티클 수명
	scale : 파티클 크기.
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

		//error값들의 디폴트 설정
		speedError = 0.1f;
		lifeError = 0.1f;
		scaleError = 0.1f;
		directionDeviation = 0.2f;
	}

	//파티클이 분출될 방향과 deviation을 설정한다.
	void setDirection(XMFLOAT3 direction, float deviation) {
		this->direction = direction;
		this->directionDeviation = (float)(deviation);
	}

	//파티클의 크기를 설정한다.
	void setScale(XMFLOAT3 scaleFactor, float scaleError)
	{
		this->averageScale = scaleFactor;
		this->scaleError = scaleError;
	}

	//파티클의 수명을 설정한다.
	void setLifelength(float lifelength, float lifeError)
	{
		this->averageLifelength = lifelength;
		this->lifeError;
	}

	//rotation을 랜덤으로 설정한다.
	void randomizeRotation() {
		randomRotation = true;
	}

	void offfRandomRotation()
	{
		randomRotation = false;
	}

	//속도에 가해질 왜곡을 설정. 0~1사이의 값을 가지며, 0은 왜곡이 없고 1에 가까워질수록 왜곡이 크다.
	void setSpeedError(float error) {
		this->speedError = error * averageSpeed;
	}

	//파티클을 지정된 방향으로 분출한다.
	void emitParticles(XMFLOAT3 genPosition, float dt)
	{
		float NumOfParticlesToCreate = ParticlesPerSecond*dt;

		int NumOfParticles = (int)NumOfParticlesToCreate;
		for (int i = 0; i < NumOfParticles; i++)
		{
			emitAtomicParticle(genPosition);
		}
	}

	//파티클을 해당 position에서 분출한다.
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