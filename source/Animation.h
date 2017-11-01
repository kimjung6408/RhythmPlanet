#pragma once
#include"d3dApp.h"
#include"globalDeviceContext.h"
#include"RenderState.h"
#include"AnimationShader.h"
#include<string>
using namespace std;

//2D �ִϸ��̼� Ŭ����.
class Animation {
	enum AnimState { NO_LOOP, LOOP, REVERSE_LOOP, FREEZE };

	//�ִϸ��̼��� �ݺ� �����̸� ���ѷ����� ����.
	//�ִϸ��̼��� �� ���������̸� 1->2->3->4 ���ٰ� �ٽ� 3->2->1 ����� �ϴ� ���� �ݺ��Ѵ�.
	//�ִϸ��̼��� ���� ���°� �ƴϸ�, ���κп� ������ ��� ������ �����.
	//�ִϸ��̼��� ���� �����̸�, �׳� �� ���� �״�� �����.
	AnimState state;
	int rowIdx;
	int columnIdx;

	int startColumn;
	int startRow;
	int maxColumn;
	int maxRow;
	ID3D11ShaderResourceView* imageSRV;
	int updateTimeDifference;
	int loopDirection;

	//������������ �� �� ĭ�� ����� ���ΰ�?
	int totalCount;

	//���� �� ��° ������ΰ�?
	int currentCount;

	//���� ���� �ð�
	float updateTimeSum;

	XMFLOAT2 Position;
	XMFLOAT3 Rotation;
	XMFLOAT2 Scale;
	float FadeFactor;
	float Alpha;
public:
	Animation(XMFLOAT2 Position, XMFLOAT3 Rotation, XMFLOAT2 Scale,
			  int numColumn, int numRow,
			  float updateTimeDifference, AnimState animState, LPCWSTR ImagePath)
	{
		rowIdx = 0;
		columnIdx = -1;
		maxColumn = numColumn;
		maxRow = numRow;
		startColumn = -1;
		startRow = -1;
		this->updateTimeDifference = updateTimeDifference;
		this->state = animState;
		totalCount = 0;
		currentCount = 1;
		//�̹��� ���� ���ʿ��� ������ ������� Ž��
		//-1�̸� �����ʿ��� �������� Ž��.
		loopDirection = 1;
		FadeFactor = 0.0f;
		Alpha = 1.0f;

		this->Position = Position;
		this->Rotation = Rotation;
		this->Scale = Scale;


		HR(D3DX11CreateShaderResourceViewFromFileW(Global::Device(), ImagePath, 0, 0, &imageSRV, 0));
	}

	void setTimeInterval(float time)
	{
		updateTimeDifference = time;
		updateTimeSum = 0;
	}

	//������������ ������ ����Ѵ�.
	void start()
	{
		this->startColumn = 0;
		this->startRow = 0;
		columnIdx = startColumn;
		rowIdx = startRow;
		this->totalCount = maxColumn*maxRow;
	}






	int getRowIdx() {
		return rowIdx;
	}

	int getColumnIdx() {
		return columnIdx;
	}

	void setImageIndex(int row, int column)
	{
		rowIdx = row;
		columnIdx = column;
		currentCount = (rowIdx*(maxColumn + 1) + (columnIdx + 1))
			- (startRow*(maxColumn + 1) + (startColumn + 1)) - 1;
	}



	boolean isStarted()
	{
		if (columnIdx != -1)
			return true;
		else
			return false;
	}

	void pause()
	{
		state = AnimState::FREEZE;
	}

	void resumeLoop()
	{
		if (state == AnimState::FREEZE)
			state = AnimState::LOOP;
	}

	void resumeNoLoop()
	{
		if (state == AnimState::FREEZE)
			state = AnimState::NO_LOOP;
	}

	void resumeReverseLoop()
	{
		if (state == AnimState::FREEZE)
			state = AnimState::REVERSE_LOOP;
	}


	void SetAnimationState(AnimState state)
	{
		this->state = state;
	}

	//�������̸� alive true, ������ �ʾ����� alive false�� ������.
	bool Update(float dt)
	{
		bool isAlive = true;
		//��������
		if (state == AnimState::FREEZE || columnIdx == -1 || (maxRow == 0 && maxColumn == 0) || totalCount == 1)
		{
			isAlive = true;
			return isAlive;
		}

		updateTimeSum += dt;

		//������Ʈ ����. dif time �̻��� �ð��� ���
		if (updateTimeSum >= updateTimeDifference)
		{
			//���� ������ �������� count�� �Ϸ��ߴٸ�,
			if (currentCount == totalCount)
			{

				switch (state)
				{
					//������ ���� ��� �ð��� �����ϰ�, alive�� false�� �Ͽ� ���ش�.
				case NO_LOOP:
					isAlive = false;
					break;

					//������ �����ϴ� ���, ������ direction�� ���� ó��
				case LOOP:
					if (loopDirection == 1)
					{
						columnIdx = startColumn;
						rowIdx = startRow;
					}
					else
					{
						columnIdx = (startColumn + totalCount - 1) % (maxColumn + 1);
						rowIdx = startRow + (startColumn + totalCount - 1) / (maxColumn + 1);
					}
					currentCount = 1;
					break;

					//��ĭ ������ ������ direction�� �ݴ�� ��ȯ
				case REVERSE_LOOP:
					if (loopDirection == 1)
					{

						columnIdx = (startColumn + totalCount - 2) % (maxColumn + 1);
						rowIdx = startRow + (startColumn + totalCount - 2) / (maxColumn + 1);
					}
					else
					{
						columnIdx = (startColumn + 1) % (maxColumn + 1);
						rowIdx = startRow + (startColumn + 1) / (maxColumn + 1);
					}
					currentCount = 2;
					loopDirection *= (-1);
					break;
				}
			}
			else //���� ��� ���� count���� ������ ���
			{
				currentCount++; //ī��Ʈ ������ �ϳ� �ø���, �������⿡ ���� ������ ó��.
				if (loopDirection == 1)
				{
					columnIdx = (startColumn + currentCount - 1) % (maxColumn + 1);
					rowIdx = startRow + (startColumn + currentCount - 1) / (maxColumn + 1);
				}
				else
				{
					columnIdx = (startColumn + totalCount - currentCount) % (maxColumn + 1);
					rowIdx = startRow + (startColumn + totalCount - currentCount) / (maxColumn + 1);
				}
			}
			updateTimeSum -= updateTimeDifference;
		}

		return isAlive;
	}

	~Animation()
	{
		ReleaseCOM(imageSRV);
	}

	void Render(AnimationShader* shader)
	{
		Global::Context()->RSSetState(RenderState::NoCullRS);
		Global::Context()->IASetInputLayout(shader->InputLayout());
		Global::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT stride = sizeof(XMFLOAT2);
		UINT offset = 0;
		ID3D11Buffer* pVB = shader->VB();
		Global::Context()->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);

		//Load world Matrix (position, scale ����)
		XMMATRIX PosMatrix = XMMatrixTranslation(Position.x, Position.y, 0);
		XMMATRIX rotMatrix = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
		XMMATRIX ScaleMatrix = XMMatrixScaling(Scale.x, Scale.y, 0);
		XMMATRIX WorldMatrix = ScaleMatrix*rotMatrix*PosMatrix;
		shader->LoadWorldMatrix(WorldMatrix);

		shader->LoadTexture(imageSRV);
		shader->LoadNumRowColumn(maxRow, maxColumn);
		shader->LoadCurrentIndex((float)rowIdx, (float)columnIdx);
		shader->LoadFadeFactor(FadeFactor);
		shader->LoadAlpha(Alpha);

		shader->getTech()->GetPassByIndex(0)->Apply(0, Global::Context());


		Global::Context()->Draw(6, 0);

		Global::finishRender();
	}
};
