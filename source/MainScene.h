#pragma once
#include"Scene.h"
#include"GUI.h"
#include"GUIShader.h"
#include"BackgroundShader.h"
#include"Triangle.h"
#include"SoundSystem.h"
#include <memory>
#include <wrl/client.h>
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

class MainScene :public Scene
{
private:

	int CurrentSelectMenu;
	SceneStatus nextScene;
	std::unique_ptr<BackgroundShader> backgroundShader;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV_BG;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV_grid;
	XMFLOAT2 uvOffsetBG;
	XMFLOAT2 uvOffsetGrid;

	XMFLOAT2 MenuNormalScale;
	XMFLOAT2 MenuCurrentScale;

	std::unique_ptr<ParticleEmitter> streakParticleEmitter;
	std::unique_ptr<ParticleShader> particleShader;
	std::unique_ptr<ParticleTexture> particleTexture;

	std::unique_ptr<Camera> cam;

	//Sound* BackgroundSound;
	//Sound* ScrollSound;
	//Sound* SelectSound;
	
	//Sound* MainSong;
	bool KeyPressed;
	float pressTimeSum;
	float scaleTimeSum;

	std::unique_ptr<GUIShader> MenuGUIShader;
	std::unique_ptr<TriangleShader> triangleShader;
	int FadeScreen;

	float logoAlpha;

	vector<GUI> ImageMenuGUIs;
	vector<GUI> ImageGUIs;
	
	GUI FadeBlack;

	GUI spinCircle1;
	GUI spinCircle2;
	GUI spinCircle3;
	GUI logo;
	GUI AlphaedLogo;
	GUIShader* MenuGUIShader;
	vector<Triangle> triangles;
	TriangleShader* triangleShader;

	SoundSystem* soundSystem;
	FMOD_SOUND* backgroundMusic;
private:
	void UpdateGUI(float dt)
	{
		for (int i = 0; i < ImageMenuGUIs.size(); i++)
		{
			if (i == CurrentSelectMenu)
			{
				ImageMenuGUIs[i].SetScale(XMFLOAT2(MenuCurrentScale.x*MathHelper::Clamp(scaleTimeSum,0.7f,1.0f),
													MenuCurrentScale.y*MathHelper::Clamp(scaleTimeSum, 0.7f, 1.0f))
					);
				ImageMenuGUIs[i].fade(true);
			}
			else
			{
				ImageMenuGUIs[i].SetScale(MenuNormalScale);
				ImageMenuGUIs[i].fade(false);
			}

			scaleTimeSum += dt/MENU_SCALE_ANIMATION_SLOW;
			
			//오버플로우 방지.
			if (scaleTimeSum >= 2.0f)
				scaleTimeSum -= 1.0f;

			ImageMenuGUIs[i].Update(dt);
		}

		spinCircle1.UpdateRotate(dt*MathHelper::Pi*1.2f, dt*MathHelper::Pi/4.0f, 0);
		spinCircle1.Update(dt);
		spinCircle2.UpdateRotate(dt*MathHelper::Pi*1.0f, dt*MathHelper::Pi*1.0f, 0);
		spinCircle2.Update(dt);
		spinCircle3.UpdateRotate(dt*MathHelper::Pi*0.8f, dt*MathHelper::Pi*0.8f, 0);
		spinCircle3.Update(dt);

		logoAlpha -= dt*1.0f;

		if (logoAlpha <= 0.0f)
		{
			logoAlpha = 0.8f;
		}

		AlphaedLogo.SetAlpha(logoAlpha);
		AlphaedLogo.SetScale(XMFLOAT2(1.4f-logoAlpha*0.1f, 1.4f-logoAlpha*0.1f));
	
		switch (FadeScreen)
		{
		case NO_FADE:
			FadeAlpha = 0.0f;
			break;
		case FADE_IN:
			FadeAlpha -= dt/FADE_VALUE_SLOW;
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
		
		uvOffsetBG.x +=dt*BG_HORIZONTAL_VELOCITY;
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

		for (int i = 0; i < triangles.size(); i++)
		{
			triangles[i].Update(dt);
		}
	}

	void RenderBackground()
	{
		//Render Background
		Global::Context()->IASetInputLayout(backgroundShader->InputLayout());
		Global::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		backgroundShader->LoadBG(SRV_BG.Get());
		backgroundShader->LoadGridImage(SRV_grid.Get());
		spinCircle1.Render(MenuGUIShader.get());
		spinCircle2.Render(MenuGUIShader.get());
		spinCircle3.Render(MenuGUIShader.get());

		AlphaedLogo.Render(MenuGUIShader.get());
		logo.Render(MenuGUIShader.get());
			ImageMenuGUIs[i].Render(MenuGUIShader.get());

		FadeBlack.Render(MenuGUIShader.get());
			triangles[i].Render(*cam,triangleShader.get());
		cam = std::make_unique<Camera>();
		backgroundShader = std::make_unique<BackgroundShader>(L"MainBGShader.hlsl");
		particleTexture = std::make_unique<ParticleTexture>(L"Textures/streak.jpg", 1);
		streakParticleEmitter = std::make_unique<ParticleEmitter>(particleTexture.get(), XMFLOAT3(0, -1, 0), 19, 10.6f, 0.1, 1, XMFLOAT3(0.2f, 3.0f, 0));
		particleShader = std::make_unique<ParticleShader>(L"ParticleShaderFile.hlsl");
		HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), L"Textures/Theme/bg.jpg", 0, 0, SRV_BG.ReleaseAndGetAddressOf(), 0));
		HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), L"Textures/Theme/grid.jpg", 0, 0, SRV_grid.ReleaseAndGetAddressOf(), 0));
		MenuGUIShader = std::make_unique<GUIShader>(L"GUIShaderFile.hlsl");
		triangleShader = std::make_unique<TriangleShader>(L"TriangleShaderFile.hlsl");

	void RenderGUI()
	{
		spinCircle1.Render(MenuGUIShader);
		spinCircle2.Render(MenuGUIShader);
		spinCircle3.Render(MenuGUIShader);

		AlphaedLogo.Render(MenuGUIShader);
		logo.Render(MenuGUIShader);

		for (int i = 0; i < ImageMenuGUIs.size(); i++)
			ImageMenuGUIs[i].Render(MenuGUIShader);

		FadeBlack.Render(MenuGUIShader);
	}

	void RenderTriangles()
	{
		for (int i = 0; i < triangles.size(); i++)
			triangles[i].Render(*cam,triangleShader);
	}
public:
	MainScene(SoundSystem* soundSystem)
	{
		this->soundSystem = soundSystem;
		soundSystem->StopMusic();
		soundSystem->PlayMusic("Sounds/Music/title.ogg", true);
		KeyPressed = false;
		nextScene = SceneStatus::MAIN;
		pressTimeSum = 0.0f;
		FadeAlpha = 1.0f;
		logoAlpha = 0.4f;

		//streak출력 전용 카메라.
		cam = new Camera();
		cam->LookAt(XMFLOAT3(0, 0, -10), XMFLOAT3(0, 0, 0), XMFLOAT3(0, 1, 0));
		cam->UpdateViewMatrix();

		backgroundShader = new BackgroundShader(L"MainBGShader.hlsl");
		particleTexture = new ParticleTexture(L"Textures/streak.jpg", 1);
		streakParticleEmitter = new ParticleEmitter(particleTexture, XMFLOAT3(0, -1, 0), 19, 10.6f, 0.1, 1, XMFLOAT3(0.2f,3.0f,0));
		streakParticleEmitter->setDirection(XMFLOAT3(0, 1, 0), 0.0f);
		streakParticleEmitter->setScale(XMFLOAT3(0.2f, 3.0f, 0), 0.2);
		particleShader = new ParticleShader(L"ParticleShaderFile.hlsl");
		//LoadImages
		HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), L"Textures/Theme/bg.jpg", 0, 0, &SRV_BG, 0));
		HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), L"Textures/Theme/grid.jpg", 0, 0, &SRV_grid, 0));

		for (int i = 0; i < 70; i++)
			triangles.push_back(Triangle(0.0f));

		//LoadGUI
		FadeBlack = GUI(L"Textures/black.png", XMFLOAT2(0, 0), XMFLOAT2(10.0f, 10.0f));
		FadeBlack.SetAlpha(FadeAlpha);
		FadeScreen = FADE_IN;

		MenuNormalScale = XMFLOAT2(0.5f, 0.2f);
		MenuCurrentScale = XMFLOAT2(0.6f, 0.25f);
		spinCircle1 = GUI(L"Textures/Theme/ring.png", XMFLOAT2(0.0f, 0), XMFLOAT2(1.2f, 1.2f));
		spinCircle1.UpdateRotate(MathHelper::Pi / 4.0f, MathHelper::Pi / 4.0f, 0);
		spinCircle2 = GUI(L"Textures/Theme/ring.png", XMFLOAT2(0.0f, 0), XMFLOAT2(1.2f, 1.2f));
		spinCircle2.UpdateRotate(MathHelper::Pi / 4.0f, -MathHelper::Pi / 4.0f, 0);
		spinCircle3 = GUI(L"Textures/Theme/ring.png", XMFLOAT2(0.0f, 0), XMFLOAT2(1.2f, 1.2f));
		ImageMenuGUIs.push_back(GUI(L"Textures/Theme/imgStartGame.png", XMFLOAT2(0.0f, -0.1f), MenuNormalScale));
		ImageMenuGUIs.push_back(GUI(L"Textures/Theme/imgOption.png", XMFLOAT2(0.0f, -0.4f), MenuNormalScale));
		ImageMenuGUIs.push_back(GUI(L"Textures/Theme/imgExit.png", XMFLOAT2(0.0f, -0.7f), MenuNormalScale));
		logo = GUI(L"Textures/Theme/logo.png", XMFLOAT2(0,0.0f), XMFLOAT2(1.0f,1.0f));
		AlphaedLogo = GUI(L"Textures/Theme/logo.png", XMFLOAT2(0, -0.1f), XMFLOAT2(1.4f, 1.4f));
		MenuGUIShader = new GUIShader(L"GUIShaderFile.hlsl");

		//LoadShaders
		triangleShader = new TriangleShader(L"TriangleShaderFile.hlsl");

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
		streakParticleEmitter->generateParticles(XMFLOAT3(MathHelper::RandF()*8.0f-4.0f, -3.7f, 0), dt);
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
			//Enter 키가 눌리면
			if (GetAsyncKeyState(VK_RETURN))
			{
				soundSystem->PlaySoundEffect(SOUND_SELECT);
				KeyPressed = true;

				FadeScreen = FADE_OUT;
				switch (CurrentSelectMenu)
				{
				case MENU_SELECT_MUSIC:
					nextScene = SceneStatus::SELECT_MUSIC;
					break;
				case MENU_OPTION:
					//Play selection sound

					//return Next Scene Enum value
					nextScene = SceneStatus::OPTION;
					break;
				case MENU_EXIT:
					nextScene = SceneStatus::ENDGAME;
					break;
				}

			}
			else if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState(VK_LEFT))
			{
				soundSystem->PlaySoundEffect(SOUND_MAIN_SCROLL);
				scaleTimeSum = 0.7f;
				KeyPressed = true;
		ParticleManager::render(particleShader.get(), cam.get());
	~MainScene() override = default;
	}

	virtual void Render()
	{
		//Render BG
		RenderBackground();
		//Render Streak
		ParticleManager::render(particleShader, cam);
		//Render Triangle
		RenderTriangles();
		//Render GUI
		RenderGUI();
	}

	~MainScene()
	{
		delete backgroundShader;
		ReleaseCOM(SRV_BG);
		ReleaseCOM(SRV_grid);
		delete streakParticleEmitter;
		delete particleShader;
		delete particleTexture;

		delete cam;
		delete MenuGUIShader;
		delete triangleShader;
	}
};