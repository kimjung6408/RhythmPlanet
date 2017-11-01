#pragma once
#include"Scene.h"
#include"GUI.h"
#include"GUIShader.h"
#include"BackgroundShader.h"
#include"SoundSystem.h"
#include"ParticleManager.h"
#include"ParticleEmitter.h"
#include"CautionMark.h"
#include"Entity.h"
#include"EntityShader.h"
#define MENU_SELECT_MUSIC 0
#define MENU_OPTION 1
#define MENU_EXIT 2

#define GRID_VELOCITY (0.15f)
#define BG_HORIZONTAL_VELOCITY (-0.3f)
#define BG_VERTICAL_VELOCITY (-0.05f)

#define PRESS_TIME_DIFFERENCE (0.3f)
#define MENU_SCALE_ANIMATION_SLOW (2.0f)

#define FADE_VALUE_SLOW (0.3f)
#define FADE_IN 0
#define FADE_OUT 1
#define NO_FADE 2

class OptionScene :public Scene
{
private:

	int CurrentSelectMenu;
	SceneStatus nextScene;
	BackgroundShader* backgroundShader;
	ID3D11ShaderResourceView* SRV_BG;
	ID3D11ShaderResourceView* SRV_grid;
	XMFLOAT2 uvOffsetBG;
	XMFLOAT2 uvOffsetGrid;

	XMFLOAT2 MenuNormalScale;
	XMFLOAT2 MenuCurrentScale;

	ParticleEmitter* streakParticleEmitter;
	ParticleShader* particleShader;
	ParticleTexture* particleTexture;

	Camera* cam;

	//Sound* BackgroundSound;
	//Sound* ScrollSound;
	//Sound* SelectSound;

	//Sound* MainSong;
	bool KeyPressed;
	float pressTimeSum;
	float scaleTimeSum;

	//FadeAlpha-> 0 : 화면이 까맣지 않음 1:화면이 까맣슴.
	float FadeAlpha;
	int FadeScreen;

	float logoAlpha;

	vector<GUI> ImageMenuGUIs;
	vector<GUI> ImageGUIs;

	GUI FadeBlack;

	GUI OptionPageImage;
	GUIShader* MenuGUIShader;

	SoundSystem* soundSystem;
	FMOD_SOUND* backgroundMusic;
private:
	void UpdateGUI(float dt)
	{
		for (int i = 0; i < ImageMenuGUIs.size(); i++)
		{

			//오버플로우 방지.
			if (scaleTimeSum >= 2.0f)
				scaleTimeSum -= 1.0f;

			ImageMenuGUIs[i].Update(dt);
		}

		logoAlpha -= dt*1.0f;

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
	}

	void UpdateBG(float dt)
	{
		uvOffsetGrid.y += GRID_VELOCITY*dt;

		uvOffsetBG.x += dt*BG_HORIZONTAL_VELOCITY;
		uvOffsetBG.y += dt*BG_VERTICAL_VELOCITY;


		//오버플로우, 언더플로우 방지.
		if (uvOffsetGrid.y >= 1.0f)
		{
			uvOffsetGrid.y -= 1.0f;
		}

		if (uvOffsetBG.x <= -1.0f)
			uvOffsetBG.x += 1.0f;

		if (uvOffsetBG.y <= -1.0f)
			uvOffsetBG.y += 1.0f;
	}

	void RenderBackground()
	{
		//Render Background
		Global::Context()->IASetInputLayout(backgroundShader->InputLayout());
		Global::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//VertexBuffer를 설정한다.
		UINT stride = sizeof(XMFLOAT2);
		UINT offset = 0;
		ID3D11Buffer* mVB = backgroundShader->VB();
		Global::Context()->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);

		//Context에 shader의 technique pass를 연결한다.
		ID3DX11EffectTechnique* tech = backgroundShader->getTech();
		tech->GetPassByIndex(0)->Apply(0, Global::Context());

		//여러가지 factor들을 로딩한다.
		backgroundShader->LoadBG(SRV_BG);
		backgroundShader->LoadGridImage(SRV_grid);
		backgroundShader->Load_uvOffsetBG(uvOffsetBG);
		backgroundShader->Load_uvOffsetGrid(uvOffsetGrid);


		Global::Context()->Draw(6, 0);

		Global::finishRender();
	}

	void RenderGUI()
	{

		for (int i = 0; i < ImageMenuGUIs.size(); i++)
			ImageMenuGUIs[i].Render(MenuGUIShader);

		FadeBlack.Render(MenuGUIShader);
	}
public:
	OptionScene(SoundSystem* soundSystem)
	{
		this->soundSystem = soundSystem;
		soundSystem->StopMusic();
		soundSystem->PlayMusic("Sounds/Music/Option.mp3", true);
		KeyPressed = false;
		nextScene = SceneStatus::OPTION;
		pressTimeSum = 0.0f;
		FadeAlpha = 1.0f;
		logoAlpha = 0.4f;

		//streak출력 전용 카메라.
		cam = new Camera();
		cam->LookAt(XMFLOAT3(0, 0, -10), XMFLOAT3(0, 0, 0), XMFLOAT3(0, 1, 0));
		cam->UpdateViewMatrix();

		backgroundShader = new BackgroundShader(L"MainBGShader.hlsl");
		particleTexture = new ParticleTexture(L"Textures/streak.jpg", 1);
		streakParticleEmitter = new ParticleEmitter(particleTexture, XMFLOAT3(0, -1, 0), 19, 10.6f, 0.1, 1, XMFLOAT3(0.2f, 3.0f, 0));
		streakParticleEmitter->setDirection(XMFLOAT3(0, 1, 0), 0.0f);
		streakParticleEmitter->setScale(XMFLOAT3(0.2f, 3.0f, 0), 0.2);
		particleShader = new ParticleShader(L"ParticleShaderFile.hlsl");
		//LoadImages
		HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), L"Textures/Theme/bg.jpg", 0, 0, &SRV_BG, 0));
		HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), L"Textures/Theme/grid.jpg", 0, 0, &SRV_grid, 0));

		//LoadGUI
		FadeBlack = GUI(L"Textures/black.png", XMFLOAT2(0, 0), XMFLOAT2(10.0f, 10.0f));
		FadeBlack.SetAlpha(FadeAlpha);
		FadeScreen = FADE_IN;

		MenuNormalScale = XMFLOAT2(0.5f, 0.2f);
		MenuCurrentScale = XMFLOAT2(0.6f, 0.25f);
		ImageMenuGUIs.push_back(GUI(L"Textures/Theme/OptionPage.png", XMFLOAT2(0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f)));
		MenuGUIShader = new GUIShader(L"GUIShaderFile.hlsl");

		//Initialize Animation Factors
		uvOffsetBG = XMFLOAT2(0, 0);
		uvOffsetGrid = XMFLOAT2(0, 0);
		scaleTimeSum = 1.0f;

		//Initialize Variables
		CurrentSelectMenu = MENU_SELECT_MUSIC;

	}

	virtual SceneStatus Update(float dt)
	{
		soundSystem->Update();
		streakParticleEmitter->generateParticles(XMFLOAT3(MathHelper::RandF()*8.0f - 4.0f, -3.7f, 0), dt);
		ParticleManager::update(*cam, dt);
		UpdateBG(dt);
		UpdateGUI(dt);

		if (KeyPressed)
		{

			pressTimeSum += dt;

			if (FadeAlpha == 1.0f)
			{
				soundSystem->StopMusic();
				return nextScene;
			}

			if (pressTimeSum >= PRESS_TIME_DIFFERENCE)
			{
				pressTimeSum = 0.0f;
				KeyPressed = false;
			}
		}
		else
		{

			if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			{
				soundSystem->PlaySoundEffect(SOUND_PREV_SCENE);
				soundSystem->StopMusic();
				KeyPressed = true;
				FadeScreen = FADE_OUT;
				nextScene = SceneStatus::MAIN;
			}
			else if (GetAsyncKeyState(VK_RETURN))
			{
				soundSystem->PlaySoundEffect(SOUND_SELECT);
				KeyPressed = true;

				FadeScreen = FADE_OUT;
				nextScene = MAIN;

			}
			else if ((GetAsyncKeyState(VK_UP)&0x8000) || (GetAsyncKeyState(VK_LEFT)&0x8000))
			{
				soundSystem->PlaySoundEffect(SOUND_MAIN_SCROLL);
				KeyPressed = true;
			}
			else if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState(VK_RIGHT))
			{
				soundSystem->PlaySoundEffect(SOUND_MAIN_SCROLL);
				KeyPressed = true;
			}
		}

		return SceneStatus::OPTION;
	}

	virtual void Render()
	{
		//Render BG
		RenderBackground();
		//Render Streak
		ParticleManager::render(particleShader, cam);
		//Render GUI
		RenderGUI();
	}

	~OptionScene()
	{
		delete backgroundShader;
		ReleaseCOM(SRV_BG);
		ReleaseCOM(SRV_grid);
		delete streakParticleEmitter;
		delete particleShader;
		delete particleTexture;

		delete cam;
		delete MenuGUIShader;
	}
};