#pragma once
#include"d3dApp.h"
#include"ModelShader.h"
#include"Model.h"
#define MAX_HP 100.0f
#define MAX_STAMINA 100.0f
#define HP_WEIGHT (5.0f);
#define STAMINA_WEIGHT (8.0f);
#define HOR_LEFT -1
#define HOR_RIGHT 1
#define VER_UP 1
#define VER_DOWN -1
#define GROUND_VELOCITY 10.0f
#define GRAVITY 70.0f
#define JUMP_VELOCITY 24.0f
#define INVINCIBLE_TIME 0.4f //�浹���� ��, �����ð� 0.4��
#define DASH_WEIGHT 3.0f // �뽬�ϸ� �̵��ӵ��� 3��� ������
#define DASH_TIME 0.2f //�뽬�� 0.2�� ���� ������
#define JUMP_AMBIENTWHITE_TIME 0.3f;


enum PlayerMovingStatus{FLOOR, JUMP, DASH};
enum PlayerCollisionStatus{INVINCIBLE, NORMAL};

class Player :public Entity
{
private:

public:
	PlayerMovingStatus MovingStatus;
	PlayerCollisionStatus CollisionStatus;
	float HP;
	float Stamina;
	XMFLOAT3 Direction;
	XMFLOAT3 Position;
	XMFLOAT3 Scale;
	XMFLOAT3 Rotation;
	XMFLOAT4 Ambient;
	XMFLOAT2 UVOffset;
	float yVelocity;
	float moveVelocity;

	//�����ð� ���� �հ�
	float InvincibleTimeSum;

	//�뽬 �ð� ���� �հ�
	float DashTimeSum;
	float JumpTimeSum;

	float AnimationTimeSum;

private:

	void ProcessKey()
	{
		float Horizontal = 0;
		float Vertical = 0;

		//�������°� �ƴϸ�
		if (MovingStatus != PlayerMovingStatus::JUMP)
		{
			if (GetAsyncKeyState(0x58) & 0x8000)
			{
				if (Stamina >= 20.0f && MovingStatus != JUMP && MovingStatus != DASH)
				{
					MovingStatus = PlayerMovingStatus::JUMP;
					yVelocity = JUMP_VELOCITY;
					Stamina -= 20.0f;
				
					Ambient = XMFLOAT4(1, 1, 1, 1);
				}
			}
			else if (GetAsyncKeyState(0x5A) & 0x8000)
			{
				if (Stamina >= 20.0f && MovingStatus != PlayerMovingStatus::DASH && MovingStatus != PlayerMovingStatus::JUMP)
				{
					MovingStatus = PlayerMovingStatus::DASH;
					Ambient = XMFLOAT4(0, 1, 1, 1);
					Stamina -= 20.0f;
				}
			}

			//Ű�� �����ִ´�� ������ �����Ѵ�.

			if (GetAsyncKeyState(VK_LEFT)&0x8000)
			{
				Horizontal = HOR_LEFT;
			}
			
			if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
			{
				Horizontal = HOR_RIGHT;
			}

			if (GetAsyncKeyState(VK_UP) & 0x8000)
			{
				Vertical = VER_UP;
			}
			
			if (GetAsyncKeyState(VK_DOWN) & 0x8000)
			{
				Vertical = VER_DOWN;
			}


			float length = sqrtf(Horizontal*Horizontal + Vertical*Vertical);

			//normalize
			if (length != 0)
			{
				Horizontal /= length;
				Vertical /= length;
			}

			Direction.x = Horizontal;
			Direction.z = Vertical;
		}
		else //���� �����̸�.
		{

		}
	}

	void ProcessMove(float dt)
	{

		if (MovingStatus != PlayerMovingStatus::DASH)
		{
			Position.x += Direction.x*moveVelocity*dt;
			Position.z += Direction.z*moveVelocity*dt;
		}
		else //status DASH
		{
			Position.x += Direction.x*moveVelocity*DASH_WEIGHT*dt;
			Position.z += Direction.z*moveVelocity*DASH_WEIGHT*dt;
		}

		if (-4.5f > Position.x)
		{
			Position.x = -4.5f;
		}
		else if (Position.x > 3.5f)
		{
			Position.x = 3.5f;
		}

		if (-4.5f > Position.z)
		{
			Position.z = -4.5f;
		}
		else if (Position.z > 3.0f)
		{
			Position.z = 3.0f;
		}
	}

public:
	Player()
	{
		HP = 100.0f;
		Stamina = 0.0f;
		Position = XMFLOAT3(2, 0, 0);
		Direction = XMFLOAT3(0, 0,0);
		Rotation = XMFLOAT3(0, 0, 0);
		Scale = XMFLOAT3(0.9f, 0.9f, 0.9f);
		yVelocity = 0;
		moveVelocity = 8.0f;
		InvincibleTimeSum = 0.0f;
		MovingStatus = PlayerMovingStatus::FLOOR;
		CollisionStatus = PlayerCollisionStatus::NORMAL;
		DashTimeSum = 0;
		JumpTimeSum = 0;
		UVOffset = XMFLOAT2(0, 0);
		Ambient = XMFLOAT4(0, 0, 0, 0);
		AnimationTimeSum = 0;
	}

	void SetInvincible()
	{
		CollisionStatus = PlayerCollisionStatus::INVINCIBLE;

	}

	void Update(float dt)
	{
		ProcessKey(); //Ű�Է� ó��. ���⼭ ���� ó���� ��. ������ Floor, Dash ������ ���� ����.
		HP += dt*HP_WEIGHT;
		Stamina += dt*STAMINA_WEIGHT;
		AnimationTimeSum += dt;
		Rotation.z += dt * MathHelper::Pi;

		if (HP >= MAX_HP)
			HP = MAX_HP;

		if (Stamina >= MAX_STAMINA)
			Stamina = MAX_STAMINA;

		//���� ó��.
		if (CollisionStatus == PlayerCollisionStatus::INVINCIBLE)
		{
			//�����ð��� ����ϸ�
			if (InvincibleTimeSum >= INVINCIBLE_TIME)
			{
				//��� ���·� �ǵ�����.
				CollisionStatus = PlayerCollisionStatus::NORMAL;
				InvincibleTimeSum = 0.0f;
				Ambient = XMFLOAT4(0, 0, 0, 0);
			}
			else
			{
				InvincibleTimeSum += dt;
				SetAmbientColor(XMFLOAT3(1, 0, 0), 1.0f-InvincibleTimeSum / INVINCIBLE_TIME);
			}
		}

		//�����̴� ���¿� ���� ó��
		switch (MovingStatus)
		{
		case PlayerMovingStatus::FLOOR:
			ProcessMove(dt);
			break;
		case PlayerMovingStatus::JUMP:
			ProcessMove(dt);
			//y��ǥ ��ȭ.
			Position.y += yVelocity*dt;
			JumpTimeSum += dt;
			//y���� ���ӵ�.
			yVelocity -= GRAVITY*dt;

			if(CollisionStatus!=INVINCIBLE)
			Ambient.w = 1 - JumpTimeSum / JUMP_AMBIENTWHITE_TIME;

			//���º�ȭ. y��ǥ�� 0���� �۰ų� ���� ��, y��ǥ�� 0���� ����. FLOOR ���·� ��ȯ.
			if (Position.y <= 0)
			{
				Position.y = 0;

				if(CollisionStatus!=INVINCIBLE)
				Ambient = XMFLOAT4(0, 0, 0, 0);

				MovingStatus = PlayerMovingStatus::FLOOR;
			}
			
			break;
		case PlayerMovingStatus::DASH:
			if (DashTimeSum >= DASH_TIME)
			{
				MovingStatus = PlayerMovingStatus::FLOOR;
				Ambient = XMFLOAT4(0, 0, 0, 0);
				DashTimeSum = 0;
			}
			else
			{
				DashTimeSum += dt;

				if(CollisionStatus!=INVINCIBLE)
				Ambient.w=1.0f - DashTimeSum / DASH_TIME;
			}

			ProcessMove(dt);
			break;
		}
	}

	void SetPosition(XMFLOAT3 position)
	{
		this->Position = position;
	}

	void SetRotation(float xAngle, float yAngle, float zAngle)
	{
		Rotation = XMFLOAT3(xAngle, yAngle, zAngle);
	}

	void SetVelocity(float velocity)
	{
		moveVelocity = velocity;
	}

	void SetDirection(XMFLOAT3 direction)
	{
		//normalize direction
		this->Direction = direction;

		float length = sqrtf(Direction.x*Direction.x + Direction.y*Direction.y + Direction.z*Direction.z);
	}

	void SetUVOffset(XMFLOAT2 UVOffset)
	{
		this->UVOffset = UVOffset;
	}
	void Move(XMFLOAT3 Direction, float Velocity, float dt)
	{
		float length = nsMath::length(Direction);

		if (length > 0.000000000001f)
		{
			Direction = XMFLOAT3(Direction.x / length, Direction.y / length, Direction.z / length);
		}

		Position.x += Direction.x*Velocity*dt;
		Position.y += Direction.y*Velocity*dt;
		Position.z += Direction.z*Velocity*dt;
	}
	XMFLOAT3 GetPosition()
	{
		return Position;
	}
	XMFLOAT3 GetRotation()
	{
		return Rotation;
	}
	XMFLOAT3 GetDirection()
	{
		return Direction;
	}
	float GetVelocity()
	{
		return moveVelocity;
	}
	XMFLOAT2 GetUVOffset()
	{
		return UVOffset;
	}

	void SetAmbientColor(XMFLOAT3 color, float Alpha)
	{
		Ambient.x = color.x;
		Ambient.y = color.y;
		Ambient.z = color.z;
		Ambient.w = Alpha;
	}

	float GetHP()
	{
		return HP;
	}

	float GetStamina()
	{
		return Stamina;
	}

	void Render(EntityShader* shader, Camera& cam, XMFLOAT3 lightPosition)
	{
		Global::Context()->IASetInputLayout(shader->InputLayout());
		Global::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//VertexBuffer�� Index Buffer�� �����Ѵ�.
		UINT stride = sizeof(EntityShader::EntityVertex);
		UINT offset = 0;
		ID3D11Buffer* pVB = shader->VB();
		ID3D11Buffer* pIB = shader->IB();
		Global::Context()->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
		Global::Context()->IASetIndexBuffer(pIB, DXGI_FORMAT_R32_UINT, 0);

		shader->getTech()->GetPassByIndex(0)->Apply(0, Global::Context());

		XMMATRIX TranslationMatrix = XMMatrixTranslation(Position.x, Position.y, Position.z);
		XMMATRIX ScaleMatrix = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
		XMMATRIX RotMatrix = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
		XMMATRIX WorldMatrix = ScaleMatrix*RotMatrix*TranslationMatrix;
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

	PlayerMovingStatus GetMovingStatus()
	{
		return MovingStatus;
	}

	PlayerCollisionStatus GetCollisionStatus()
	{
		return CollisionStatus;
	}

	//if true, player�� alive
	//if false, player�� dead
	bool ProcCollisionWithEntity()
	{
		if (CollisionStatus == PlayerCollisionStatus::INVINCIBLE)
			return true;

		CollisionStatus = PlayerCollisionStatus::INVINCIBLE;
		HP -= 20.0f;
		bool isAlive;

		if (HP <= 0.0f)
			isAlive = false;
		else
			isAlive = true;

		return isAlive;
	}

	XMFLOAT4 GetAmbientColor()
	{
		return Ambient;
	}
};