#pragma once
#include"Scene.h"
#include"GUI.h"
#include"GUIShader.h"
#include"BackgroundShader.h"
#include"SoundSystem.h"
#include"FontShader.h"
#include"Text.h"
#include"ParticleManager.h"
#include"ParticleEmitter.h"
#include <memory>
#include <wrl/client.h>

#define GRID_VELOCITY (0.15f)
#define BG_HORIZONTAL_VELOCITY (-0.3f)
#define BG_VERTICAL_VELOCITY (-0.05f)

#define PRESS_TIME_DIFFERENCE (0.3f)
#define MENU_SCALE_ANIMATION_SLOW (2.0f)

#define FADE_VALUE_SLOW (0.3f)
#define FADE_IN 0
#define FADE_OUT 1
#define NO_FADE 2

class ResultScene :public Scene
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

	//FadeAlpha-> 0 : 화면이 까맣지 않음 1:화면이 까맣슴.
	float FadeAlpha;
	int FadeScreen;

	vector<GUI> ImageGUIs;

	GUI FadeBlack;
	GUI SongBnImage;

	std::unique_ptr<GUIShader> MenuGUIShader;

	SoundSystem* soundSystem;
	FMOD_SOUND* backgroundMusic;

	std::unique_ptr<FontShader> fontShader;
	Text SCOREstring;
	Text score;
	Text SongTitleString;
private:
	void UpdateGUI(float dt)
	{

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
		backgroundShader->LoadBG(SRV_BG.Get());
		backgroundShader->LoadGridImage(SRV_grid.Get());
		backgroundShader->Load_uvOffsetBG(uvOffsetBG);
		backgroundShader->Load_uvOffsetGrid(uvOffsetGrid);


		Global::Context()->Draw(6, 0);

		Global::finishRender();
	}

	void RenderGUI()
	{

		for (int i = 0; i < ImageGUIs.size(); i++)
			ImageGUIs[i].Render(MenuGUIShader.get());

		SCOREstring.Render(fontShader.get());
		score.Render(fontShader.get());
		SongTitleString.Render(fontShader.get());
		SongBnImage.Render(MenuGUIShader.get());

		FadeBlack.Render(MenuGUIShader.get());
	}
public:
	ResultScene(SoundSystem* soundSystem)
	{
		this->soundSystem = soundSystem;
		soundSystem->StopMusic();
		KeyPressed = false;
		nextScene = SceneStatus::RESULT;
		pressTimeSum = 0.0f;
		FadeAlpha = 1.0f;

		fontShader = std::make_unique<FontShader>(L"FontShaderFile.hlsl", L"Textures/FontAtlas.png", L"Textures/FontAtlas.metrics");
		SCOREstring.SetCharacterGap(0.0f);
		score.SetCharacterGap(0.0f);
		SongTitleString.SetCharacterGap(0.0f);

		backgroundShader = std::make_unique<BackgroundShader>(L"MainBGShader.hlsl");
		particleTexture = std::make_unique<ParticleTexture>(L"Textures/streak.jpg", 1);
		streakParticleEmitter = std::make_unique<ParticleEmitter>(particleTexture.get(), XMFLOAT3(0, -1, 0), 19, 10.6f, 0.1, 1, XMFLOAT3(0.2f, 3.0f, 0));
		streakParticleEmitter->setDirection(XMFLOAT3(0, 1, 0), 0.0f);
		streakParticleEmitter->setScale(XMFLOAT3(0.2f, 3.0f, 0), 0.2);
		particleShader = std::make_unique<ParticleShader>(L"ParticleShaderFile.hlsl");
		//LoadImages
		HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), L"Textures/Theme/bg.jpg", 0, 0, SRV_BG.ReleaseAndGetAddressOf(), 0));
		HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), L"Textures/Theme/grid.jpg", 0, 0, SRV_grid.ReleaseAndGetAddressOf(), 0));

		//LoadGUI
		FadeBlack = GUI(L"Textures/black.png", XMFLOAT2(0, 0), XMFLOAT2(10.0f, 10.0f));
		FadeBlack.SetAlpha(FadeAlpha);
		FadeScreen = FADE_IN;

		MenuGUIShader = std::make_unique<GUIShader>(L"GUIShaderFile.hlsl");
		fontShader = std::make_unique<FontShader>(L"FontShaderFile.hlsl", L"Textures/FontAtlas.png");

		SCOREstring = Text(XMFLOAT2(0.0f, 0.27f), XMFLOAT2(0.05f, 0.05f), "SCORE");
		SCOREstring.SetAlign(TEXT_ALIGN_LEFT);
		SCOREstring.SetCharacterGap(0.1f);

		score = Text(XMFLOAT2(0.84f, 0.05f), XMFLOAT2(0.08f, 0.08f), "0");
		score.SetAlign(TEXT_ALIGN_RIGHT);
		score.SetCharacterGap(0.1f);

		SongTitleString = Text(XMFLOAT2(-0.9f, 0.5f), XMFLOAT2(0.07f, 0.07f), "Title - ");
		SongTitleString.SetAlign(TEXT_ALIGN_LEFT);
		SongTitleString.SetCharacterGap(0.09f);

		stringstream ss;
		ss << Global::GetScore();
		string str;
		ss >> str;
		score.SetText(str);

		SongTitleString.SetText("Song - "+Global::GetCurrentMusic()->GetTitle());
		//Initialize Animation Factors
		uvOffsetBG = XMFLOAT2(0, 0);
		uvOffsetGrid = XMFLOAT2(0, 0);

		ImageGUIs.push_back(GUI(L"Textures/Result/ResultBG.png", XMFLOAT2(0, 0), XMFLOAT2(1.0f, 0.8f)));
		ImageGUIs[0].SetAlpha(0.5f);
		ImageGUIs.push_back(GUI(L"Textures/Result/pantsu.png", XMFLOAT2(0, -0.88), XMFLOAT2(1.0f, 0.12f)));
		ImageGUIs.push_back(GUI(L"Textures/Result/pantsu.png", XMFLOAT2(0, 0.88), XMFLOAT2(1.0f, 0.12f)));
		ImageGUIs[2].SetRotation(XMFLOAT3(0, 0, MathHelper::Pi));
		ImageGUIs.push_back(GUI(L"Textures/Result/ScoreFrame.png", XMFLOAT2(0.55f, 0.1f), XMFLOAT2(0.8f, 0.25f)));
		ImageGUIs.push_back(GUI(L"Textures/Result/ImageFrame.png", XMFLOAT2(-0.5f, 0.1f), XMFLOAT2(0.4f, 0.3f)));

		if(Global::IsClear())
			ImageGUIs.push_back(GUI(L"Textures/Result/Clear.png", XMFLOAT2(0.7f, 0.26f), XMFLOAT2(0.25f, 0.1f)));
		else
			ImageGUIs.push_back(GUI(L"Textures/Result/Fail.png", XMFLOAT2(0.7f, 0.26f), XMFLOAT2(0.35f, 0.25f)));
		SongBnImage = (GUI(L"Textures/black.png", XMFLOAT2(-0.5f, 0.1f), XMFLOAT2(0.373f, 0.25f)));
		SongBnImage.LoadSRV(Global::GetCurrentMusic()->GetIntroImage(), true);

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
				KeyPressed = true;
				FadeScreen = FADE_OUT;
				nextScene = SceneStatus::SELECT_MUSIC;
			}
			else if (GetAsyncKeyState(VK_RETURN))
			{
				soundSystem->PlaySoundEffect(SOUND_SELECT);
				KeyPressed = true;

				FadeScreen = FADE_OUT;
				nextScene = SELECT_MUSIC;

			}
		}

		return SceneStatus::RESULT;
	}

	virtual void Render()
	{
		//Render BG
		RenderBackground();
		//Render Streak
		ParticleManager::render(particleShader.get(), cam.get());
		//Render GUI
		RenderGUI();
	}

	~ResultScene() override = default;
};