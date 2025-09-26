#pragma once
#include"Scene.h"
#include"GUI.h"
#include"GUIShader.h"
#include"BackgroundShader.h"
#include"Triangle.h"
#include"Music.h"
#include"SoundSystem.h"
#include"Ball.h"
#include"BoundaryBox.h"
#include"Tile.h"
#include"ParticleManager.h"
#include"ParticleEmitter.h"
#include"Text.h"
#include"Model.h"
#include"Animation.h"
#include"AnimationShader.h"
#include"ScoreSystem.h"
#include"Player.h"
#include"CautionMark.h"
#include"SideBar.h"
#include<string>
#include <json/json.h>
#include <fstream>
#include<queue>
#include<deque>
//#include"Model.h"
using namespace std;
#define PRESS_TIME_DIFFERENCE (4.0f)

#define FADE_VALUE_SLOW (0.3f)
#define FADEOUT_TIME_DIFFERENCE (0.14f)
#define FADE_IN 0
#define FADE_OUT 1
#define NO_FADE 2

#define HPSTAMINA_ZERO_POSITION (-1.375f)
#define HPSTAMINA_MAX_POSITION (-0.7f)
#define PROGRESS_POINTER_BEGIN_POSITION (0.1f)
#define PROGRESS_POINTER_END_POSITION (0.9f)

#define PROJECTILE_SPEED 0.04
#define PROJECTILE_UNDER_HEIGHT 0.5f
#define PROJECTILE_UPPER_HEIGHT 1.25f
#define PROJECTILE_CAUTION_UNDER_HEIGHT -0.8f
#define PROJECTILE_CAUTION_UPPER_HEIGHT 0.0f

#define SIDEBALL_SIZE 0.22f

#define CAUTION_SIGN_TICK_LENGTH 4.0f

#define SIDEBAR_VELOCITY 0.01389f

//공중 4개의 지정된 위치 중에서 임의로 1개를 택하여 목표지점 x,y에 떨어지게 한다.
const XMFLOAT3 GenPosition[4]
= {
	XMFLOAT3(-4.0f,5.0f, -3.0f),
	XMFLOAT3(-4.0f, 5.0f,-5.0f),
	XMFLOAT3(4.0f, 5.0f, -3.0f),
	XMFLOAT3(4.0f, 5.0f, -5.0f)
};

class InGameScene :public Scene
{
private:


	Player player;

	Json::Value ObstacleData;
	ScoreSystem* scoreSystem;
	FontShader* fontShader;
	Text* txtScore;
	SoundSystem* soundSystem;
	XMFLOAT3 lightPosition;
	//쉐이더 선언
	EntityShader* UpperBallShader;
	EntityShader* UnderBallShader;
	EntityShader* FallBallShader;
	EntityShader* BoxShader;
	EntityShader* BarShader;
	EntityShader* TileShader;
	EntityShader* SideBallCautionShader;
	EntityShader* SideBarCautionShader;
	EntityShader* FloorBarCautionShader;
	EntityShader* FloorBombCautionShader;
	EntityShader* FallCautionShader;
	ModelShader* modelShader;

	Model* model;

	//게임에 활력을 불어넣어줄 이벤트들!
	deque<FloorEvent> FloorEvents;
	deque<FallEvent> FallEvents;
	deque<SideEvent> SideEvents;
	deque<CautionSideEvent> CautionSideEvents;
	deque<CautionFloorEvent> CautionFloorEvents;
	deque<CautionFallEvent> CautionFallEvents;


	//오브젝트 선언
	vector<Ball> UpperBalls;
	vector<Ball> UnderBalls;
	vector<Ball> FallBalls;
	vector<Bar> Sidebars;
	vector<Bar> Bombs;
	//vector<SideBar> bars;
	//
	vector<Animation> animations;
	vector<CautionMark> SideBallCautionMarks;
	vector<CautionMark> SideBarCautionMarks;
	vector<CautionMark> FloorBarCautionMarks;
	vector<CautionMark> FloorBombCautionMarks;
	vector<CautionMark> FallCautionMarks;

	BoundaryBox* LeftBoxes[2][9];
	BoundaryBox* FarBoxes[2][9];
	BoundaryBox* RightBoxes[2][9];
	BoundaryBox* tiles[10][9];
	Camera* cam;

	bool KeyPressed;
	bool GamePlaying;
	bool MusicPlay;
	float GameTimeSum;
	float pressTimeSum;
	float scaleTimeSum;
	vector<GUI> ImageGUIs;

	GUI HealthBar;
	GUI StaminaBar;
	GUI FadeBlack;
	GUI BG;
	GUI GameUndergroundImageGUI;
	GUI ProgressPointer;
	GUIShader* MenuGUIShader;
	ParticleEmitter* StarEmitter;
	ParticleEmitter* SparkEmitter;
	ParticleEmitter* BombEmitter;
	ParticleEmitter* FuseEmitter;
	ParticleTexture* FuseTexture;
	ParticleTexture* StarTexture;
	ParticleTexture* SparkTexture;
	ParticleTexture* BombTexture;
	ParticleShader* starShader;
	float FadeAlpha;
	int FadeScreen;
	float TextureAnimationFactor;

	SceneStatus nextScene;

	float StartWaitTime;

	float BPM;
	unsigned int SongTotalLength;

private:
	void UpdateGUI(float dt)
	{
		TextureAnimationFactor += dt*0.5f;
		if (TextureAnimationFactor >= 1.0f)
		{
			TextureAnimationFactor -= 1.0f;
		}
		HealthBar.SetUVOffset(XMFLOAT2(-TextureAnimationFactor, 0));
		StaminaBar.SetUVOffset(XMFLOAT2(-TextureAnimationFactor, 0));
		GameUndergroundImageGUI.SetUVOffset(XMFLOAT2(0, TextureAnimationFactor));
		
		//Update progress Ptr Position
		float ProgPtrLerp = (GameTimeSum*1000.0f) / ((float)SongTotalLength);
		float ProgPtr_x = (1 - ProgPtrLerp)*PROGRESS_POINTER_BEGIN_POSITION + ProgPtrLerp*PROGRESS_POINTER_END_POSITION;
		ProgressPointer.SetPosition(ProgPtr_x, 0.9f);
		ProgressPointer.SetRotation(XMFLOAT3(0, 0, -8.0f*MathHelper::Pi*TextureAnimationFactor));


		//현재의 HP, Stamina의 값에 따라, HP Bar, Stamina Bar의 게이지 상태를 Linear Interpolation 한다.
		float CurrentHPRatio = player.GetHP() / MAX_HP;
		float CurrentStaminaRatio = player.GetStamina() / MAX_STAMINA;
		float HP_xPos = CurrentHPRatio*HPSTAMINA_MAX_POSITION + (1 - CurrentHPRatio)*HPSTAMINA_ZERO_POSITION;
		float Stamina_xPos = CurrentStaminaRatio*HPSTAMINA_MAX_POSITION + (1 - CurrentStaminaRatio)*HPSTAMINA_ZERO_POSITION;
		HealthBar.SetPosition(HP_xPos, 0.9f);
		StaminaBar.SetPosition(Stamina_xPos, 0.72f);

		switch (FadeScreen)
		{
		case NO_FADE:
			FadeAlpha = 0.0f;
			break;
		case FADE_IN:
			FadeAlpha -= dt / FADE_VALUE_SLOW;
			break;
		case FADE_OUT:
			FadeAlpha += dt / FADE_VALUE_SLOW;
			break;
		}
		FadeAlpha = MathHelper::Clamp(FadeAlpha, 0.0f, 1.0f);
		FadeBlack.SetAlpha(FadeAlpha);

		if (FadeAlpha == 0.0f)
			FadeScreen = NO_FADE;


		//	0.3f*i - 1.8f
	}

	void UpdateScore(float dt)
	{
		//UpdateScore
		if (GamePlaying)
		{
			scoreSystem->Update(dt);
			stringstream ss;
			ss << scoreSystem->GetScore();
			txtScore->SetText(ss.str());
		}
	}

	//GameTimeSum을 이용하여 이벤트를 발생시킨다.
	//

	//side : 왼쪽인지, 앞쪽 벽면인지, 오른쪽인지
	//subtype : 벽면 위에서 나오는지, 아래서 나오는지, 튀어나왔다가 들어가는 것인지
	//place :1,2,3,4,5,6,7
	void GenerateSideObject(int Side, int Subtype, int place)
	{		
		//create side object by event data
		switch (Side)
		{
		case SIDE_LEFT:

			switch (Subtype)
			{
			case SIDESUBTYPE_BAR:
				Sidebars.push_back(Bar(XMFLOAT3(-5.0f, 1.0f, -4.0f + (float)place), XMFLOAT3(3.0f, 0.99f, 1.0f), SIDEBAR_VELOCITY*BPM, XMFLOAT3(1, 0, 0), 60.0 / BPM));
				BombEmitter->generateParticles(XMFLOAT3(-6.0f, 1.0f, -4.0f + (float)place), 0.1f);
				break;
			case SIDESUBTYPE_UNDER:
				UnderBalls.push_back(Ball(SIDEBALL_SIZE, XMFLOAT3(-5.0f, PROJECTILE_UNDER_HEIGHT, -4.0f + (float)place), PROJECTILE_SPEED*BPM, XMFLOAT3(1, 0, 0)));
				BombEmitter->generateParticles(XMFLOAT3(-6.0f, 1.0f, -4.0f + (float)place), 0.1f);
				break;
			case SIDESUBTYPE_UPPER:
				UpperBalls.push_back(Ball(SIDEBALL_SIZE, XMFLOAT3(-5.0f, PROJECTILE_UPPER_HEIGHT, -4.0f + (float)place), PROJECTILE_SPEED*BPM, XMFLOAT3(1, 0, 0)));
				BombEmitter->generateParticles(XMFLOAT3(-6.0f, 1.25f, -4.0f + (float)place), 0.1f);
				break;
			}
			break;

		case SIDE_RIGHT:

			switch (Subtype)
			{
			case SIDESUBTYPE_BAR:
				Sidebars.push_back(Bar(XMFLOAT3(4.0f, 1.0f, -4.0f + (float)place), XMFLOAT3(3.0f, 0.99f, 1.0f), SIDEBAR_VELOCITY*BPM, XMFLOAT3(-1, 0, 0), 60.0 / BPM));
				BombEmitter->generateParticles(XMFLOAT3(5.0f, 1.0f, -4.0f + (float)place), 0.1f);
				break;
			case SIDESUBTYPE_UNDER:
				UnderBalls.push_back(Ball(SIDEBALL_SIZE, XMFLOAT3(4.0f, PROJECTILE_UNDER_HEIGHT, -4.0f + (float)place), PROJECTILE_SPEED*BPM, XMFLOAT3(-1, 0, 0)));
				BombEmitter->generateParticles(XMFLOAT3(5.0f, 1.0f, -4.0f + (float)place), 0.1f);
				break;
			case SIDESUBTYPE_UPPER:
				UpperBalls.push_back(Ball(SIDEBALL_SIZE, XMFLOAT3(4.0f, PROJECTILE_UPPER_HEIGHT, -4.0f + (float)place), PROJECTILE_SPEED*BPM, XMFLOAT3(-1, 0, 0)));
				BombEmitter->generateParticles(XMFLOAT3(5.0f, 1.25f, -4.0f + (float)place), 0.1f);
				break;
			}
			break;
		case SIDE_TOP:

			switch (Subtype)
			{
			case SIDESUBTYPE_BAR:
				Sidebars.push_back(Bar(XMFLOAT3(-4.0f + (float)place, 1.0f, 3.5f), XMFLOAT3(1.0f, 0.99f, 3.0f), SIDEBAR_VELOCITY*BPM, XMFLOAT3(0, 0, -1), 60.0 / BPM));
				BombEmitter->generateParticles(XMFLOAT3(-4.0f + (float)place, 1.0f, 4.5f), 0.1f);
				break;
			case SIDESUBTYPE_UNDER:
				UnderBalls.push_back(Ball(SIDEBALL_SIZE, XMFLOAT3(-4.0f + (float)place, PROJECTILE_UNDER_HEIGHT, 3.5f), PROJECTILE_SPEED*BPM, XMFLOAT3(0, 0, -1)));
				BombEmitter->generateParticles(XMFLOAT3(-4.0f + (float)place, 1.0f, 4.5f), 0.1f);
				break;
			case SIDESUBTYPE_UPPER:
				UpperBalls.push_back(Ball(SIDEBALL_SIZE, XMFLOAT3(-4.0f + (float)place, PROJECTILE_UPPER_HEIGHT, 3.5f), PROJECTILE_SPEED*BPM, XMFLOAT3(0, 0, -1)));
				BombEmitter->generateParticles(XMFLOAT3(-4.0f + (float)place, 1.25f, 4.5f), 0.1f);
				break;
			}
			break;
		}
	}

	void GenerateSideCautionMark(int Side, int Subtype, int place)
	{
		//create side object by event data
		switch (Side)
		{
		case SIDE_LEFT:

			switch (Subtype)
			{
			case SIDESUBTYPE_BAR:
				SideBarCautionMarks.push_back(CautionMark(BPM, XMFLOAT3(-4.9f, 0.1f, -4.0f + (float)place), XMFLOAT3(1, 0, 0)));
				break;
			case SIDESUBTYPE_UNDER:
				SideBallCautionMarks.push_back(CautionMark(BPM, XMFLOAT3(-5.0f, PROJECTILE_CAUTION_UNDER_HEIGHT-1.5f, -2.0f + (float)place), XMFLOAT3(1, 0, 0)));
				break;
			case SIDESUBTYPE_UPPER:
				SideBallCautionMarks.push_back(CautionMark(BPM, XMFLOAT3(-5.0f, PROJECTILE_CAUTION_UPPER_HEIGHT-1.3f, -2.0f + (float)place), XMFLOAT3(1, 0, 0)));
				break;
			}
			break;

		case SIDE_RIGHT:

			switch (Subtype)
			{
			case SIDESUBTYPE_BAR:
				SideBarCautionMarks.push_back(CautionMark(BPM, XMFLOAT3(3.9f, 0.1f, -4.0f + (float)place), XMFLOAT3(1, 0, 0)));
				break;
			case SIDESUBTYPE_UNDER:
				SideBallCautionMarks.push_back(CautionMark(BPM, XMFLOAT3(4.0f, PROJECTILE_CAUTION_UNDER_HEIGHT-1.5f, -2.0f + (float)place), XMFLOAT3(1, 0, 0)));
				break;
			case SIDESUBTYPE_UPPER:
				SideBallCautionMarks.push_back(CautionMark(BPM, XMFLOAT3(4.0f, PROJECTILE_CAUTION_UPPER_HEIGHT-1.3f, -2.0f + (float)place), XMFLOAT3(1, 0, 0)));
				break;
			}
			break;
		case SIDE_TOP:

			switch (Subtype)
			{
			case SIDESUBTYPE_BAR:
				SideBarCautionMarks.push_back(CautionMark(BPM, XMFLOAT3(-4.0f + (float)place, 0.3f, 3.5f), XMFLOAT3(0, 0, 1)));
				break;
			case SIDESUBTYPE_UNDER:
				SideBallCautionMarks.push_back(CautionMark(BPM, XMFLOAT3(-4.0f + (float)place, PROJECTILE_CAUTION_UNDER_HEIGHT, 3.5f), XMFLOAT3(0, 0, 1)));
				break;
			case SIDESUBTYPE_UPPER:
				SideBallCautionMarks.push_back(CautionMark(BPM, XMFLOAT3(-4.0f + (float)place, PROJECTILE_CAUTION_UPPER_HEIGHT, 3.5f), XMFLOAT3(0, 0, 1)));
				break;
			}
			break;
		}
	}

	void GenerateFloorObject(int Size, int x, int y)
	{
		switch (Size)
		{

		//튀어나왔다가 들어가는 막대기를 생성한다.
		case FLOORSIZE_SMALL:
			Sidebars.push_back(Bar(XMFLOAT3(-4.0f+(float)x, -1, -4.0f + (float)y), XMFLOAT3(1.0f, 2.0f, 0.99f), SIDEBAR_VELOCITY*BPM, XMFLOAT3(0, 1, 0), 60.0 / BPM));
			break;

		//폭발체를 생성한다.
		case FLOORSIZE_NORMAL:
			Bombs.push_back(Bar(XMFLOAT3(-4.0f + (float)x, 0, -4.0f + (float)y), XMFLOAT3(3.3f, 2.0f, 3.5f), 20.0f*15.0 / BPM, XMFLOAT3(0, 0, 0), 20.0f*15.0 / BPM));
			SparkEmitter->generateParticles(XMFLOAT3(-4.0f + (float)x+1, 1, -4.0f + (float)y+1), 0.1f);
			SparkEmitter->generateParticles(XMFLOAT3(-4.0f + (float)x+1, 0, -4.0f + (float)y-1), 0.1f);
			SparkEmitter->generateParticles(XMFLOAT3(-4.0f + (float)x-1, 1, -4.0f + (float)y+1), 0.1f);
			SparkEmitter->generateParticles(XMFLOAT3(-4.0f + (float)x-1, 0, -4.0f + (float)y-1), 0.1f);
			break;
		}
	}

	void GenerateFloorCautionMark(int Size, int x, int y)
	{
		switch (Size)
		{

			//튀어나왔다가 들어가는 막대기의 경고마크 (무지개 원) 생성한다.
		case FLOORSIZE_SMALL:
			FloorBarCautionMarks.push_back(CautionMark(BPM, XMFLOAT3(-4.0f+(float)x, -1.3f, -4.0f+(float)y), XMFLOAT3(0, 1, 0)));
			break;

			//폭발체를 생성한다.
		case FLOORSIZE_NORMAL:
			FloorBombCautionMarks.push_back(CautionMark(BPM, XMFLOAT3(-4.0f + (float)x, -0.1f, -4.0f + (float)y), XMFLOAT3(0, 1, 0)));
			break;
		}
	}

	void GenerateFallObject(int genIdx,int x, int z)
	{

		XMFLOAT3 generatePos = GenPosition[genIdx];

		float target_x = x - 4.0f;
		float target_z = z - 4.0f;
		XMFLOAT3 TargetPos = XMFLOAT3(target_x, 0, target_z);
		XMFLOAT3 Direction = XMFLOAT3(TargetPos.x - generatePos.x, TargetPos.y - generatePos.y, TargetPos.z - generatePos.z);

		//normalise
		float dirLength = sqrt(Direction.x*Direction.x + Direction.y*Direction.y + Direction.z*Direction.z);
		Direction.x /= dirLength;
		Direction.y /= dirLength;
		Direction.z /= dirLength;

		FallBalls.push_back(Ball(0.2f, generatePos, PROJECTILE_SPEED*BPM, Direction));
		BombEmitter->generateParticles(generatePos, 0.1f);
	}

	void GenerateFallCautionMark(int genIdx, int x, int z)
	{
		XMFLOAT3 genPos = GenPosition[genIdx];
		FallCautionMarks.push_back(CautionMark(BPM, genPos, XMFLOAT3(0, 1, 0)));
	}

	void UpdateEvents()
	{
		while (!CautionSideEvents.empty())
		{
			CautionSideEvent sideEventObj = CautionSideEvents.front();
			float eventTime = (15.0f / BPM)*(float)sideEventObj.tick;
			if (eventTime <= GameTimeSum)
			{
				GenerateSideCautionMark(sideEventObj.side, sideEventObj.subtype, sideEventObj.place);
				//pop front data
				CautionSideEvents.pop_front();
			}
			else
				break;
		}

		while (!CautionFloorEvents.empty())
		{

			CautionFloorEvent FloorEventObj = CautionFloorEvents.front();
			float eventTime = (15.0f / BPM)*(float)FloorEventObj.tick;
			if (eventTime <= GameTimeSum)
			{
				//create floor object by event data
				GenerateFloorCautionMark(FloorEventObj.size, FloorEventObj.x, FloorEventObj.y);
				//pop front data
				CautionFloorEvents.pop_front();
			}
			else
				break;
		}

		while (!CautionFallEvents.empty())
		{
			CautionFallEvent FallEventObj = CautionFallEvents.front();
			float eventTime = (15.0f / BPM)*(float)FallEventObj.tick;
			if (eventTime <= GameTimeSum)
			{
				//create fall object by event data
				srand(time(NULL));
				GenerateFallCautionMark(FallEventObj.genIdx, FallEventObj.x, FallEventObj.y);
				//pop front data
				CautionFallEvents.pop_front();
			}
			else
				break;
		}


		//update side event
		while (!SideEvents.empty())
		{
			SideEvent sideEventObj = SideEvents.front();
			float eventTime = (15.0f / BPM)*(float)sideEventObj.tick;
			if (eventTime<=GameTimeSum)
			{
				GenerateSideObject(sideEventObj.side, sideEventObj.subtype, sideEventObj.place);
				//pop front data
				SideEvents.pop_front();
			}
			else
				break;
		}

		while (!FloorEvents.empty())
		{
			
			FloorEvent FloorEventObj = FloorEvents.front();
			float eventTime = (15.0f / BPM)*(float)FloorEventObj.tick;
			if (eventTime <= GameTimeSum)
			{
				//create floor object by event data
				GenerateFloorObject(FloorEventObj.size, FloorEventObj.x, FloorEventObj.y);
				//pop front data
				FloorEvents.pop_front();
			}
			else
				break;
		}

		while (!FallEvents.empty())
		{
			FallEvent FallEventObj = FallEvents.front();
			float eventTime = (15.0f / BPM)*(float)FallEventObj.tick;
			if (eventTime <= GameTimeSum)
			{
				//create fall object by event data
				srand(time(NULL));
				GenerateFallObject(FallEventObj.genIdx, FallEventObj.x, FallEventObj.y);
				//pop front data
				FallEvents.pop_front();
			}
			else
				break;
		}
	}

	void UpdateGameObjects(float dt)
	{
		for (int i = 0; i < UpperBalls.size(); i++)
		{
			UpperBalls[i].Update(dt);
		}

		for (int i = 0; i < UnderBalls.size(); i++)
		{
			UnderBalls[i].Update(dt);
		}

		for (int i = 0; i < FallBalls.size(); i++)
		{
			FallBalls[i].Update(dt);
		}

		 for(int i=0; i<Sidebars.size();i++)
		{
			Sidebars[i].Update(dt);
		}

		 for (int i = 0; i < SideBallCautionMarks.size(); i++)
			 SideBallCautionMarks[i].Update(dt);

		 for (int i = 0; i < SideBarCautionMarks.size(); i++)
			 SideBarCautionMarks[i].Update(dt);

		 for (int i = 0; i < FloorBarCautionMarks.size(); i++)
		 {
			 FloorBarCautionMarks[i].Update(dt);
		 }

		 for (int i = 0; i < FloorBombCautionMarks.size(); i++)
		 {
			 FloorBombCautionMarks[i].Update(dt);
		 }

		 for (int i = 0; i < FallCautionMarks.size(); i++)
		 {
			 FallCautionMarks[i].Update(dt);
		 }
		
		

		//Floor 폭발을 업데이트한다.



		//2D애니메이션 및 이펙트를 업데이트한다.
		/*
		
		*/
	}

	void CheckObjectLives()
	{
		for (int i = 0; i < Sidebars.size(); i++)
		{
			if (Sidebars[i].isDead())
			{
				Sidebars.erase(Sidebars.begin() + i);
			}
		}

		for (int i = 0; i < SideBallCautionMarks.size();)
		{
			if (SideBallCautionMarks[i].isDead())
			{
				SideBallCautionMarks.erase(SideBallCautionMarks.begin() + i);
				continue;
			}

			i++;
		}

		for (int i = 0; i < SideBarCautionMarks.size();)
		{
			if (SideBarCautionMarks[i].isDead())
			{
				SideBarCautionMarks.erase(SideBarCautionMarks.begin() + i);
				continue;
			}

			i++;
		}

		for (int i = 0; i < FloorBarCautionMarks.size();)
		{
			if (FloorBarCautionMarks[i].isDead())
			{
				FloorBarCautionMarks.erase(FloorBarCautionMarks.begin() + i);
				continue;
			}

			i++;
		}

		for (int i = 0; i < FloorBombCautionMarks.size();)
		{
			if (FloorBombCautionMarks[i].isDead())
			{
				FloorBombCautionMarks.erase(FloorBombCautionMarks.begin() + i);
				continue;
			}

			i++;
		}

		for (int i = 0; i < FallCautionMarks.size();)
		{
			if (FallCautionMarks[i].isDead())
			{
				FallCautionMarks.erase(FallCautionMarks.begin() + i);
				continue;
			}

			i++;
		}


	}


	//플레이어가 살았으면 true, 죽었으면 false를 return
	bool ProcessCollision()
	{
		CheckObjectOut();

		if (player.GetCollisionStatus() == PlayerCollisionStatus::INVINCIBLE)
			return true;


		for (int i = 0; i < UnderBalls.size(); i++)
		{
			if (nsCollision::CheckSphereSphereCollision(player.GetPosition(), 0.9f, UnderBalls[i].GetPosition(), UnderBalls[i].GetRadius()))
			{
				bool playerAlive=player.ProcCollisionWithEntity();
				FuseEmitter->generateParticles(UnderBalls[i].GetPosition(), 0.4f);
				UnderBalls.erase(UnderBalls.begin() + i);

				scoreSystem->InitializeAcceleration();
				soundSystem->PlaySoundEffect(SOUND_BOMB);

				if (playerAlive)
				{
					//create effect at player position

					return true;
				}
				else
					return false;
			}
		}

		for (int i = 0; i < UpperBalls.size(); i++)
		{
			if (nsCollision::CheckSphereSphereCollision(player.GetPosition(), 0.9f, UpperBalls[i].GetPosition(), UpperBalls[i].GetRadius()))
			{
				bool playerAlive = player.ProcCollisionWithEntity();
				FuseEmitter->generateParticles(UpperBalls[i].GetPosition(), 0.4f);
				UpperBalls.erase(UpperBalls.begin() + i);

				scoreSystem->InitializeAcceleration();
				soundSystem->PlaySoundEffect(SOUND_BOMB);

				if (playerAlive)
				{
					//create effect at player position

					return true;
				}
				else
					return false;
			}
		}

		for (int i = 0; i < FallBalls.size(); i++)
		{
			if (nsCollision::CheckSphereSphereCollision(player.GetPosition(), 0.9f, FallBalls[i].GetPosition(), FallBalls[i].GetRadius()))
			{
				bool playerAlive = player.ProcCollisionWithEntity();
				FuseEmitter->generateParticles(FallBalls[i].GetPosition(), 0.4f);
				FallBalls.erase(FallBalls.begin() + i);

				scoreSystem->InitializeAcceleration();
				soundSystem->PlaySoundEffect(SOUND_BOMB);

				if (playerAlive)
				{
					//create effect at player position

					return true;
				}
				else
					return false;
			}
		}

		//Collision with sidebar
		for (int i = 0; i < Sidebars.size(); i++)
		{
			XMFLOAT3 MinP = XMFLOAT3(player.GetPosition());
			XMFLOAT3 MaxP = XMFLOAT3(player.GetPosition());
			MinP.x -= 0.45f;
			MinP.y -= 0.45;
			MinP.z -= 0.45f;
			MaxP.x += 0.45f;
			MaxP.y += 0.45f;
			MaxP.z += 0.45f;

			BoxCollider collider = Sidebars[i].GetBoxCollider();
			//if (nsCollision::CheckCubeSphereCollision(Sidebars[i].GetBoxCollider(), player.GetPosition(), 1.0f))

			if(nsCollision::CheckCubeCubeCollision(MinP, MaxP,collider.MinPoint, collider.MaxPoint ))
			{
				bool playerAlive = player.ProcCollisionWithEntity();
				FuseEmitter->generateParticles(player.GetPosition(), 0.4f);
				scoreSystem->InitializeAcceleration();
				soundSystem->PlaySoundEffect(SOUND_BOMB);

				if (playerAlive)
				{
					//create effect at player position
					return true;
				}
				else
					return false;
			}
		}

		//Collision with 
		//Collision with Bomb
		for (int i = 0; i < Bombs.size(); i++)
		{
			XMFLOAT3 MinP = XMFLOAT3(player.GetPosition());
			XMFLOAT3 MaxP = XMFLOAT3(player.GetPosition());
			MinP.x -= 0.5f;
			MinP.y -= 0.5f;
			MinP.z -= 0.5f;
			MaxP.x += 0.5f;
			MaxP.y += 0.5f;
			MaxP.z += 0.5f;

			BoxCollider collider = Bombs[i].GetBoxCollider();
			//if (nsCollision::CheckCubeSphereCollision(Sidebars[i].GetBoxCollider(), player.GetPosition(), 1.0f))

			if (nsCollision::CheckCubeCubeCollision(MinP, MaxP, collider.MinPoint, collider.MaxPoint))
			{
				bool playerAlive = player.ProcCollisionWithEntity();
				FuseEmitter->generateParticles(player.GetPosition(), 0.4f);
				scoreSystem->InitializeAcceleration();
				soundSystem->PlaySoundEffect(SOUND_BOMB);
				Bombs.clear();

				if (playerAlive)
				{
					//create effect at player position
					return true;
				}
				else
				{
					return false;
				}
			}
		}

		Bombs.clear();

		return true;
	}

	//오브젝트가 바깥에 나갔는지 검사한다. 바깥에 나갔으면 삭제한다.
	//현재 해당 오브젝트 : ball

	void CheckObjectOut()
	{
		//ball
		for (int i = 0; i < UpperBalls.size();)
		{
			XMFLOAT3 Pos=UpperBalls[i].GetPosition();
			if (Pos.x <= -10.0f || Pos.x >= 10.0f || Pos.y <= 0.0f || Pos.z >= 10.0f | Pos.z <= -10.0f)
			{
				UpperBalls.erase(UpperBalls.begin() + i);
				continue;
			}

			i++;
		}

		for (int i = 0; i < UnderBalls.size();)
		{
			XMFLOAT3 Pos = UnderBalls[i].GetPosition();
			if (Pos.x <= -10.0f || Pos.x >= 10.0f || Pos.y <= 0.0f || Pos.z >= 10.0f | Pos.z <= -10.0f)
			{
				UnderBalls.erase(UnderBalls.begin() + i);
				continue;
			}

			i++;
		}

		for (int i = 0; i <FallBalls.size();)
		{
			XMFLOAT3 Pos = FallBalls[i].GetPosition();
			if (Pos.x <= -10.0f || Pos.x >= 10.0f || Pos.y <= 0.0f || Pos.z >= 10.0f | Pos.z <= -10.0f)
			{
				FallBalls.erase(FallBalls.begin() + i);
				SparkEmitter->generateParticles(Pos, 0.1f);
				continue;
			}

			i++;
		}
	}

	void RenderGUI()
	{
		if (!GamePlaying)
		{
			BG.Render(MenuGUIShader);
			return;
		}

		HealthBar.Render(MenuGUIShader);
		StaminaBar.Render(MenuGUIShader);
		for (int i = 0; i < ImageGUIs.size(); i++)
		{
			ImageGUIs[i].Render(MenuGUIShader);
		}

		ProgressPointer.Render(MenuGUIShader);
		txtScore->Render(fontShader);

		FadeBlack.Render(MenuGUIShader);
	}

	void RenderBoundaryBoxes()
	{
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				FarBoxes[i][j]->Render(BoxShader, *cam, lightPosition);
				LeftBoxes[i][j]->Render(BoxShader, *cam, lightPosition);
				RightBoxes[i][j]->Render(BoxShader, *cam, lightPosition);
			}
		}
	}

	void RenderTiles()
	{
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				tiles[i][j]->Render(TileShader, *cam, lightPosition);
			}
		}
	}

	void RenderGameObjects()
	{
		for (int i = 0; i < SideBallCautionMarks.size(); i++)
			SideBallCautionMarks[i].Render(SideBallCautionShader, *cam, lightPosition);

		for (int i = 0; i < SideBarCautionMarks.size(); i++)
			SideBarCautionMarks[i].Render(SideBarCautionShader, *cam, lightPosition);

		for (int i = 0; i < FloorBarCautionMarks.size(); i++)
		{
			FloorBarCautionMarks[i].Render(FloorBarCautionShader, *cam, lightPosition);
		}

		for (int i = 0; i < FloorBombCautionMarks.size(); i++)
		{
			FloorBombCautionMarks[i].Render(FloorBombCautionShader, *cam, lightPosition);
		}

		for (int i = 0; i < FallCautionMarks.size(); i++)
		{
			FallCautionMarks[i].Render(FallCautionShader, *cam, lightPosition);
		}


		for (int i = 0; i < UpperBalls.size(); i++)
			UpperBalls[i].Render(UpperBallShader, *cam, lightPosition);

		for (int i = 0; i < UnderBalls.size(); i++)
			UnderBalls[i].Render(UnderBallShader, *cam, lightPosition);

		for (int i = 0; i < FallBalls.size(); i++)
			FallBalls[i].Render(FallBallShader, *cam, lightPosition);

		for (int i = 0; i < Sidebars.size(); i++)
		{
			Sidebars[i].Render(BarShader, *cam, lightPosition);
		}

		model->Render(modelShader, lightPosition, &player, *cam);
	}

	void LoadGameData()
	{
		int NumOfKeys=ObstacleData.size();
		vector<string> TickKeys=ObstacleData.getMemberNames();

		for (int keyIdx = 0; keyIdx < NumOfKeys; keyIdx++)
		{
			int NumOfEvents = ObstacleData[TickKeys[keyIdx]].size();

			for (int eventIdx = 0; eventIdx < NumOfEvents; eventIdx++)
			{
				int type = ObstacleData[TickKeys[keyIdx]][eventIdx].get("type", "").asInt();
				if (type == OBSTACLE_SIDE) {

					int tick = atoi(&((TickKeys[keyIdx])[0]));
					int subtype=ObstacleData[TickKeys[keyIdx]][eventIdx].get("subtype", "").asInt();
					int side=ObstacleData[TickKeys[keyIdx]][eventIdx].get("side", "").asInt();
					int place=ObstacleData[TickKeys[keyIdx]][eventIdx].get("place", "").asInt();

					SideEvents.push_back(SideEvent(tick, subtype, side, place));
					CautionSideEvents.push_back(CautionSideEvent(tick - CAUTION_SIGN_TICK_LENGTH, subtype, side, place));

				}
				else if (type == OBSTACLE_FLOOR) {

					int tick = atoi(&((TickKeys[keyIdx])[0]));
					int size = ObstacleData[TickKeys[keyIdx]][eventIdx].get("size", "").asInt();
					int x= ObstacleData[TickKeys[keyIdx]][eventIdx].get("x", "").asInt();
					int y = ObstacleData[TickKeys[keyIdx]][eventIdx].get("y", "").asInt();

					FloorEvents.push_back(FloorEvent(tick, size, x, y));
					CautionFloorEvents.push_back(CautionFloorEvent(tick-CAUTION_SIGN_TICK_LENGTH, size, x, y));
				}
				else if (type == OBSTACLE_FALL) {

					int tick = atoi(&((TickKeys[keyIdx])[0]));
					//int size = ObstacleData[TickKeys[keyIdx]][eventIdx].get("size", "").asInt();
					int x = ObstacleData[TickKeys[keyIdx]][eventIdx].get("x", "").asInt();
					int y = ObstacleData[TickKeys[keyIdx]][eventIdx].get("y", "").asInt();
					srand(time(NULL));
					int genIdx = rand() % 4;
					FallEvents.push_back(FallEvent(tick,x, y, genIdx));
					CautionFallEvents.push_back(CautionFallEvent(tick-CAUTION_SIGN_TICK_LENGTH, x, y,genIdx));
				}
			}
		}

		sort(SideEvents.begin(), SideEvents.end());
		sort(FloorEvents.begin(), FloorEvents.end());
		sort(FallEvents.begin(), FallEvents.end());
		sort(CautionSideEvents.begin(), CautionSideEvents.end());
		sort(CautionFloorEvents.begin(), CautionFloorEvents.end());
		sort(CautionFallEvents.begin(), CautionFallEvents.end());
	}
public:
	InGameScene(SoundSystem* soundSystem)
	{
		this->soundSystem = soundSystem;
		soundSystem->StopMusic();
		player = Player();
		KeyPressed = true;
		GamePlaying = false;
		MusicPlay = false;
		GameTimeSum = -3.0f;
		pressTimeSum = 0.0f;
		TextureAnimationFactor = 0.0f;
		StartWaitTime = 0.0f;
		BPM = Global::GetCurrentMusic()->GetFloatBPM();
		scoreSystem =new ScoreSystem(BPM);

		MenuGUIShader = new GUIShader(L"GUIShaderFile.hlsl");
		BG = GUI(L"Textures/black.png", XMFLOAT2(0, 0), XMFLOAT2(1, 1));
		BG.LoadSRV(Global::GetCurrentMusic()->GetBGImageSRV(), true);
		BG.Render(MenuGUIShader);

		GameUndergroundImageGUI= GUI(L"Textures/Theme/bg3.jpg", XMFLOAT2(0, 0), XMFLOAT2(1, 1));
		GameUndergroundImageGUI.SetAlpha(0.6f);

		ObstacleData = note_obstacle_parse(Global::GetCurrentMusic()->GetGameDataPath());
		//LoadGameData();

		SparkTexture = new ParticleTexture(L"Textures/Ingame/spark.png", 1);
		SparkEmitter = new ParticleEmitter(SparkTexture, XMFLOAT3(0, 1, 0), 240, 8.0f, 0.1f, 1.0f, XMFLOAT3(0.4f, 0.4f, 0.4f));
		SparkEmitter->setDirection(XMFLOAT3(0, 1, 0), 0.56f);
		SparkEmitter->setLifelength(0.08f, 0.05f);
		SparkEmitter->setScale(XMFLOAT3(1.8f, 1.8f, 1.8f), 0.1);
		SparkEmitter->setSpeedError(0.2f);
		SparkEmitter->randomizeRotation();

		FuseTexture = new ParticleTexture(L"Textures/Ingame/Fuse.png", 1);
		FuseEmitter = new ParticleEmitter(FuseTexture, XMFLOAT3(0, 1, 0), 180, 8.0f, 0.1f, 1.0f, XMFLOAT3(0.4f, 0.4f, 0.4f));
		FuseEmitter->setDirection(XMFLOAT3(0, 1, 0), 0.66f);
		FuseEmitter->setLifelength(0.2f, 0.01f);
		FuseEmitter->setScale(XMFLOAT3(0.65f, 0.65f, 0.65f), 0.1);
		FuseEmitter->setSpeedError(0.2f);
		FuseEmitter->randomizeRotation();

		BombTexture = new ParticleTexture(L"Textures/Ingame/NeonTriangle.png", 1);
		BombEmitter = new ParticleEmitter(BombTexture, XMFLOAT3(0, 1, 0), 180, 8.0f, 0.1f, 1.0f, XMFLOAT3(0.4f, 0.4f, 0.4f));
		BombEmitter->setDirection(XMFLOAT3(0, 1, 0), 0.37f);
		BombEmitter->setLifelength(0.3f, 0.3f);
		BombEmitter->setScale(XMFLOAT3(0.5f, 0.5f, 0.5f), 0.1);
		BombEmitter->setSpeedError(0.2f);
		BombEmitter->randomizeRotation();


		StarTexture = new ParticleTexture(L"Textures/particleStar.png", 1);
		StarEmitter = new ParticleEmitter(StarTexture, XMFLOAT3(0, 1, 0), 120, 8.0f, 0.1f, 1.0f, XMFLOAT3(0.4f, 0.4f, 0.4f));
		StarEmitter->setDirection(XMFLOAT3(0, 1, 0), 0.16f);
		StarEmitter->setLifelength(1.0f, 0.3f);
		StarEmitter->setScale(XMFLOAT3(0.4f, 0.4f, 0.4f), 0.1);
		StarEmitter->setSpeedError(0.1f);
		StarEmitter->randomizeRotation();

		//Load Boundary Boxes
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				FarBoxes[i][j] = new BoundaryBox(XMFLOAT3(j-4, i, 4), XMFLOAT3(1, 1, 1));
				LeftBoxes[i][j] = new BoundaryBox(XMFLOAT3(-5, i, -j + 4), XMFLOAT3(1, 1, 1));
				RightBoxes[i][j] = new BoundaryBox(XMFLOAT3(4, i, -j + 4), XMFLOAT3(1, 1, 1));
			}
		}

		//Load Tiles
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				tiles[i][j] = new BoundaryBox(XMFLOAT3(i - 5, -1, j - 4), XMFLOAT3(1, 1, 1));
			}
		}

		FadeAlpha = 1.0f;
		FadeScreen = FADE_IN;

		nextScene = SceneStatus::SELECT_MUSIC;
		fontShader = new FontShader(L"FontShaderFile.hlsl", L"Textures/FontAtlas.png", L"Textures/FontAtlas.metrics");
		txtScore->SetCharacterGap(0.0f);
		cam->LookAt(XMFLOAT3(-0.5, 10, -13.5f), XMFLOAT3(-0.5, 0, 0), XMFLOAT3(0, 1, 0));
		cam->SetLens(MathHelper::Pi/6.0f, 640.0f / 480.0f, 0.001f, 1000.0f);
		cam->SetPosition(XMFLOAT3(-0.5f, 10.8f, -13.5f));
		cam->Walk(-3.0f);
		cam->UpdateViewMatrix();
		lightPosition = XMFLOAT3(0, 10 , -10);
		//LoadImages

		UpperBallShader = new EntityShader(L"EntityShaderFile.hlsl", ENTITY_UPPERSPHERE);
		UnderBallShader = new EntityShader(L"EntityShaderFile.hlsl", ENTITY_UNDERSPHERE);
		FallBallShader = new EntityShader(L"EntityShaderFile.hlsl", ENTITY_FALLSPHERE);
		BoxShader = new EntityShader(L"EntityShaderFile.hlsl", ENTITY_BOX);
		TileShader = new EntityShader(L"EntityShaderFile.hlsl",ENTITY_FLOOR);
		BarShader = new EntityShader(L"EntityShaderFile.hlsl", ENTITY_SIDEBAR);
		starShader = new ParticleShader(L"ParticleShaderFile.hlsl");
		fontShader = new FontShader(L"FontShaderFile.hlsl", L"Textures/FontAtlas.png");
		modelShader = new ModelShader(L"ModelShaderFile.hlsl");

		SideBallCautionShader = new EntityShader(L"CautionShader.hlsl", ENTITY_CAUTIONMARK_SIDE_MAGIC_CIRCLE);
		SideBarCautionShader = new EntityShader(L"CautionShader.hlsl", ENTITY_CAUTIONMARK_SIDE_BAR);
		FloorBarCautionShader = new EntityShader(L"CautionShader.hlsl", ENTITY_CAUTIONMARK_FLOORBAR);
		FloorBombCautionShader = new EntityShader(L"CautionShader.hlsl", ENTITY_CAUTIONMARK_BOMB);
		FallCautionShader = new EntityShader(L"CautionShader.hlsl", ENTITY_CAUTIONMARK_FALL);
		//PlayerShader = new EntityShader(L"EntityShaderFile.hlsl", "Models/A/dragon.obj", L"Models/A/skin.png");
		txtScore = new Text(XMFLOAT2(0.8f, 0.73f), XMFLOAT2(0.06f, 0.06f), "0");
		txtScore->SetAlign(TEXT_ALIGN_RIGHT);
		txtScore->SetCharacterGap(0.08f);

		FadeBlack = GUI(L"Textures/black.png", XMFLOAT2(0.0f, 0.0f), XMFLOAT2(10.0f, 10.0f));
		FadeBlack.SetAlpha(FadeAlpha);
		//LoadSounds

		//LoadShaders

		//LoadGUIs
		ImageGUIs.push_back(GUI(L"Textures/Ingame/HP_text.png",
			XMFLOAT2(-0.7f, 0.9f), XMFLOAT2(0.2f, 0.14f)));
		ImageGUIs[0].SetAlpha(0.4f);
		ImageGUIs.push_back(GUI(L"Textures/Ingame/Stamina_text.png",
			XMFLOAT2(-0.7f, 0.72f), XMFLOAT2(0.2f, 0.14f)));
		ImageGUIs.push_back(GUI(L"Textures/Ingame/ScoreFrame.png",
			XMFLOAT2(0.5f, 0.7f), XMFLOAT2(0.6f, 0.16f)));
		ImageGUIs.push_back(GUI(L"Textures/Ingame/GageFrame.png",
			XMFLOAT2(-0.7f, 0.9f), XMFLOAT2(0.45f, 0.09f)));
		ImageGUIs.push_back(GUI(L"Textures/Ingame/GageFrame.png",
			XMFLOAT2(-0.7f, 0.72f), XMFLOAT2(0.45f, 0.09f)));
		ImageGUIs.push_back(GUI(L"Textures/Ingame/HorizontalLine.png",
			XMFLOAT2(0.5f, 0.9f), XMFLOAT2(0.4f, 0.03f)));
		ImageGUIs[1].SetAlpha(0.4f);
		ImageGUIs[2].SetAlpha(0.8f);
		HealthBar = GUI(L"Textures/Ingame/Healthbar.jpg", XMFLOAT2(-0.7f, 0.9f), XMFLOAT2(0.375f, 0.06f));
		HealthBar.SetAlpha(0.8f);
		StaminaBar = GUI(L"Textures/Ingame/StaminaBar.jpg", XMFLOAT2(-0.7f, 0.64f), XMFLOAT2(0.375f, 0.06f));
		StaminaBar.SetAlpha(0.8f);
		ProgressPointer = GUI(L"Textures/Ingame/ProgressBarCircle.png",
			XMFLOAT2(PROGRESS_POINTER_BEGIN_POSITION, 0.9f), XMFLOAT2(0.04f, 0.04f));

		model = new Model("Models/A/note.obj");

		SongTotalLength = 100000000;

		//AutogenEvents();

		LoadGameData();
	}

	void AutogenEvents()
	{
		for (int i = 0; i < 17; i++)
		{
			int subtype = SIDESUBTYPE_UPPER;
			int side = SIDE_TOP;
			int place = 3;
			SideEvents.push_back(SideEvent(i  + 4, subtype, side, place));
			CautionSideEvents.push_back(CautionSideEvent(i +4 - CAUTION_SIGN_TICK_LENGTH, subtype, side, place));
		}

	/*	for (int i = 0; i < 325; i++)
		{
			int x = rand() % 8;
			int y = rand() % 8;
			int genIdx = rand() % 4;
			FallEvents.push_back(FallEvent(i * 4 + 4, x, y, genIdx));
			CautionFallEvents.push_back(CautionFallEvent(i * 4 + 4-CAUTION_SIGN_TICK_LENGTH, x, y, genIdx));
		}*/

		//for (int i = 0; i < 350; i++)
		//{
		//	int x = rand() % 8;
		//	int y = rand() % 8;
		//	int size = rand() % 2;
		//	FloorEvents.push_back(FloorEvent(i * 4 + 4, size, x, y));
		//	CautionFloorEvents.push_back(CautionFloorEvent(i * 4 + 4 - CAUTION_SIGN_TICK_LENGTH, size,x, y));
		//}
	}

	SceneStatus Update(float dt)
	{
		//게임이 끝났는지 검사한다.
		if (GameTimeSum >= 2.0f+(float)SongTotalLength / 1000.0f)
		{
			//결과 데이터를 글로벌 클래스에 저장한다.
			//점수, 충돌횟수, 성공 전달.
			Global::SetScore(scoreSystem->GetScore());
			Global::SetMusicClear(true);

			return SceneStatus::RESULT;
		}


		if (!GamePlaying)
		{
			StartWaitTime += dt;

			if (StartWaitTime >= 3.0f)
			{
				GamePlaying = true;
			}
			else
				return SceneStatus::INGAME;
		}

		GameTimeSum += dt;
		CheckObjectLives();
		if (!MusicPlay && GameTimeSum>=0.0f)
		{
			MusicPlay = true;
			soundSystem->PlayMusic(Global::GetCurrentMusic()->GetSoundPath().c_str(), false);
			SongTotalLength=soundSystem->getMusicTotalLength();
		}


			UpdateEvents();

		//Collision Detection and processing
		//플레이어의 체력이 0보다 작거나 같으면 false를 리턴한다.
		player.Update(dt);
		UpdateGameObjects(dt);
		bool PlayerAlive=ProcessCollision();

		if (!PlayerAlive)
		{
			//점수와 실패를 글로벌 클래스에 전달한다.
			Global::SetScore(scoreSystem->GetScore());
			Global::SetMusicClear(false);

			//Result 창을 보여준다.
			return SceneStatus::RESULT;
		}

		soundSystem->Update();

		//UpdateParticle

		if(player.GetMovingStatus()==PlayerMovingStatus::JUMP|| player.GetMovingStatus()==PlayerMovingStatus::DASH)
		StarEmitter->generateParticles(player.GetPosition(), dt);

		ParticleManager::update(*cam, dt);

		UpdateGUI(dt);

		UpdateScore(dt);

			

		if (KeyPressed)
		{
			pressTimeSum += dt;

			if (pressTimeSum >= FADEOUT_TIME_DIFFERENCE)
			{
				pressTimeSum = 0.0f;
				KeyPressed = false;
			}
		}
		else
		{
			//0x8000과 and 연산을 해주면, 바로 현재 시점에 눌렸는지를 체크해준다.
			//0x8000과 and 연산을 하지 않으면, 누적된 esc입력값을 받아서, 과거에 눌렸었는지까지 체크한다.
			if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			{
				soundSystem->PlaySoundEffect(SOUND_PREV_SCENE);
				
				if(MusicPlay)
				soundSystem->StopMusic();

				KeyPressed = true;
				FadeScreen = FADE_OUT;
				nextScene = SceneStatus::SELECT_MUSIC;
			}
		}


		if (FadeAlpha == 1.0f)
		{
			ParticleManager::clean();
			return nextScene;
		}

		return SceneStatus::INGAME;
	}

	virtual void Render()
	{
		GameUndergroundImageGUI.Render(MenuGUIShader);
		//RenderBoxes
		RenderBoundaryBoxes();
		RenderTiles();

		ParticleManager::render(starShader, cam);
		RenderGameObjects();
		//Render GUI
		RenderGUI();

	}

	~InGameScene()
	{
		delete cam;
		delete MenuGUIShader;
		delete scoreSystem;
	}
	
};