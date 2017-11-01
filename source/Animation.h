#pragma once
#include"d3dApp.h"
#include"globalDeviceContext.h"
#include"RenderState.h"
#include"AnimationShader.h"
#include<string>
using namespace std;

//2D 애니메이션 클래스.
class Animation {
	enum AnimState { NO_LOOP, LOOP, REVERSE_LOOP, FREEZE };

	//애니메이션이 반복 상태이면 무한루프를 돈다.
	//애니메이션이 역 루프상태이면 1->2->3->4 갔다가 다시 3->2->1 재생을 하는 것을 반복한다.
	//애니메이션이 루프 상태가 아니면, 끝부분에 도달한 경우 루프를 멈춘다.
	//애니메이션이 동결 상태이면, 그냥 그 상태 그대로 멈춘다.
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

	//시작지점부터 총 몇 칸을 출력할 것인가?
	int totalCount;

	//현재 몇 개째 출력중인가?
	int currentCount;

	//현재 진행 시간
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
		//이미지 파일 왼쪽에서 오른쪽 순서대로 탐색
		//-1이면 오른쪽에서 왼쪽으로 탐색.
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

	//시작지점부터 끝까지 출력한다.
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

	//진행중이면 alive true, 끝나지 않았으면 alive false를 리턴함.
	bool Update(float dt)
	{
		bool isAlive = true;
		//리턴조건
		if (state == AnimState::FREEZE || columnIdx == -1 || (maxRow == 0 && maxColumn == 0) || totalCount == 1)
		{
			isAlive = true;
			return isAlive;
		}

		updateTimeSum += dt;

		//업데이트 조건. dif time 이상의 시간이 경과
		if (updateTimeSum >= updateTimeDifference)
		{
			//만약 정해진 개수까지 count를 완료했다면,
			if (currentCount == totalCount)
			{

				switch (state)
				{
					//루프가 없는 경우 시간을 갱신하고, alive를 false로 하여 없앤다.
				case NO_LOOP:
					isAlive = false;
					break;

					//루프가 존재하는 경우, 역루프 direction에 따라서 처리
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

					//한칸 옆으로 보내고 direction을 반대로 전환
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
			else //아직 모든 것을 count하지 못했을 경우
			{
				currentCount++; //카운트 개수를 하나 늘리고, 루프방향에 따라 별도의 처리.
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

		//Load world Matrix (position, scale 설정)
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
