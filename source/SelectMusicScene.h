#pragma once
#include"Scene.h"
#include"GUI.h"
#include"GUIShader.h"
#include"BackgroundShader.h"
#include"Triangle.h"
#include"Music.h"
#include"SoundSystem.h"
#include"Text.h"
#include <deque>
#include <memory>
#include <utility>
#include <wrl/client.h>
#define MENU_SELECT_MUSIC 0
#define MENU_OPTION 1
#define MENU_EXIT 2

#define GRID_VELOCITY (0.15f)
#define BG_HORIZONTAL_VELOCITY (-0.3f)
#define BG_VERTICAL_VELOCITY (-0.05f)

#define PRESS_TIME_DIFFERENCE (4.0f)
#define MUSIC_SCROLL_TIME_DIFFERENCE (0.14f)

#define FADE_VALUE_SLOW (0.3f)
#define FADE_IN 0
#define FADE_OUT 1
#define NO_FADE 2

struct MusicItem
{
	GUI MusicItemBox;
	Music* pMusic;
	Text txtTitle;
	Text txtArtist;
	Text txtBPM;
	MusicItem(GUI gui, Music* pSong)
	{
		MusicItemBox = gui;
		pMusic = pSong;
		XMFLOAT2 guiPos = gui.GetPosition();

		if (pSong != NULL)
		{
			txtTitle = Text(XMFLOAT2(guiPos.x + 0.1f, guiPos.y + 0.5f), XMFLOAT2(0.05f, 0.05f), pMusic->GetTitle());
			txtArtist = Text(XMFLOAT2(guiPos.x + 0.1f, guiPos.y + 0.2f), XMFLOAT2(0.1f, 0.1f), pMusic->GetArtist());
			txtBPM = Text(XMFLOAT2(-0.65f, 0.4f), XMFLOAT2(0.07f, 0.07f), pMusic->GetstrBPM());
			txtTitle.SetCharacterGap(0.0f);
			txtArtist.SetCharacterGap(0.0f);
			txtBPM.SetCharacterGap(0.0f);
		}
	}
};

class SelectMusicScene :public Scene
{
private:

	SoundSystem* soundSystem;
	int KeyDirection;
	int CurrentSelectMusic;
	float scrollAnimationFactor;
	int scrollMoveFactor;
	std::unique_ptr<BackgroundShader> backgroundShader;
	std::unique_ptr<FontShader> fontShader;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV_BG;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV_grid;
	XMFLOAT2 uvOffsetBG;
	XMFLOAT2 uvOffsetGrid;

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
	vector<GUI> ImageGUIs;
	std::deque<std::unique_ptr<MusicItem>> MusicItems;
	bool MusicScrollMove; //사용자가 상하 방향키를 눌렀을 때
	GUI HealthBar;
	GUI StaminaBar;

	GUI spinCircle1;
	GUI spinCircle2;
	GUI spinCircle3;

	std::unique_ptr<GUI> introImage;

	GUI FadeBlack;

	std::unique_ptr<GUIShader> MenuGUIShader;
	vector<Triangle> triangles;
	std::unique_ptr<TriangleShader> triangleShader;

	float FadeAlpha;
	int FadeScreen;
	bool MusicPlay;
	float MusicPlayDelayTimeSum;


	SceneStatus nextScene;
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

		if (KeyDirection != 0)
		{
			scrollAnimationFactor += dt*7.0f;

			if (scrollAnimationFactor >= 1.0f)
			{
				scrollAnimationFactor = 1.0f;
			}
		}

		//음악 스크롤러 부분 업데이트
		for (int i = 0; i < MusicItems.size(); i++)
		{
			float yPos;
			
			if (KeyDirection==-1)
				yPos = i + scrollAnimationFactor*KeyDirection;
			else if(KeyDirection==1)
				yPos = i + scrollAnimationFactor*KeyDirection - 2.0f;
			else
				yPos = i-1;

			MusicItems[i]->MusicItemBox.SetPosition(0.55f, 0.3f*yPos - 1.8f);
			XMFLOAT2 itemPos = MusicItems[i]->MusicItemBox.GetPosition();
			MusicItems[i]->txtTitle.SetPosition(itemPos.x-0.25f, itemPos.y + 0.01f);
			MusicItems[i]->txtArtist.SetPosition(itemPos.x - 0.15f, itemPos.y - 0.065f);
		}


	//	0.3f*i - 1.8f
	}

	void UpdateBG(float dt)
	{
		uvOffsetGrid.y += GRID_VELOCITY*dt;

		uvOffsetBG.x += dt*BG_HORIZONTAL_VELOCITY;
		uvOffsetBG.y += dt*BG_VERTICAL_VELOCITY;

		spinCircle1.UpdateRotate(dt*MathHelper::Pi*1.2f, dt*MathHelper::Pi / 4.0f, 0);
		spinCircle1.Update(dt);
		spinCircle2.UpdateRotate(dt*MathHelper::Pi*1.0f, dt*MathHelper::Pi*1.0f, 0);
		spinCircle2.Update(dt);
		spinCircle3.UpdateRotate(dt*MathHelper::Pi*0.8f, dt*MathHelper::Pi*0.8f, 0);
		spinCircle3.Update(dt);


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

	void RenderGUI()
	{
		spinCircle1.Render(MenuGUIShader.get());
		spinCircle2.Render(MenuGUIShader.get());
		spinCircle3.Render(MenuGUIShader.get());


		for (int i = 0; i < MusicItems.size(); i++)
		{
			MusicItems[i]->MusicItemBox.Render(MenuGUIShader.get());
		}

		for (int i = 0; i < MusicItems.size(); i++)
		{
			MusicItems[i]->txtTitle.Render(fontShader.get());
			MusicItems[i]->txtArtist.Render(fontShader.get());
			
			if (i == 7)
				MusicItems[i]->txtBPM.Render(fontShader.get());
		}

		for (int i = 0; i < ImageGUIs.size(); i++)
		{
			ImageGUIs[i].Render(MenuGUIShader.get());
		}

		introImage->Render(MenuGUIShader.get());

		FadeBlack.Render(MenuGUIShader.get());
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

	void RenderTriangles()
	{
		for (int i = 0; i < triangles.size(); i++)
			triangles[i].Render(*cam, triangleShader.get());
	}
public:
	SelectMusicScene(SoundSystem* soundSystem)
	{
		this->soundSystem = soundSystem;
		soundSystem->StopMusic();
		KeyPressed = true;
		MusicScrollMove = false;
		MusicPlay = false;
		scrollAnimationFactor = 1.0f;
		pressTimeSum = 0.0f;
		KeyDirection = 0;
		MusicPlayDelayTimeSum = 0.0f;

		FadeAlpha=1.0f;
		FadeScreen=FADE_IN;
		uvOffsetBG = XMFLOAT2(0, 0);
		uvOffsetGrid = XMFLOAT2(0, 0);
		introImage = nullptr;

		nextScene = SceneStatus::SELECT_MUSIC;
		fontShader = std::make_unique<FontShader>(L"FontShaderFile.hlsl", L"Textures/FontAtlas.png", L"Textures/FontAtlas.metrics");
		cam = std::make_unique<Camera>();
		cam->LookAt(XMFLOAT3(0, 0, -10), XMFLOAT3(0, 0, 0), XMFLOAT3(0, 1, 0));
		cam->UpdateViewMatrix();

		backgroundShader = std::make_unique<BackgroundShader>(L"MainBGShader.hlsl");
		particleTexture = std::make_unique<ParticleTexture>(L"Textures/streak.jpg", 1);
		streakParticleEmitter = std::make_unique<ParticleEmitter>(particleTexture.get(), XMFLOAT3(0, -1, 0), 19, 10.6f, 0.1, 1, XMFLOAT3(0.2f, 3.0f, 0));
		streakParticleEmitter->setDirection(XMFLOAT3(0, 1, 0), 0.0f);
		streakParticleEmitter->setScale(XMFLOAT3(0.2f, 3.0f, 0), 0.2);
		particleShader = std::make_unique<ParticleShader>(L"ParticleShaderFile.hlsl");
		fontShader = std::make_unique<FontShader>(L"FontShaderFile.hlsl", L"Textures/FontAtlas.png");
		//LoadImages
		HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), L"Textures/Theme/bg.jpg", 0, 0, SRV_BG.ReleaseAndGetAddressOf(), 0));
		HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), L"Textures/Theme/grid.jpg", 0, 0, SRV_grid.ReleaseAndGetAddressOf(), 0));

		for (int i = 0; i < 70; i++)
			triangles.push_back(Triangle(-2.0f));



		ImageGUIs.push_back(GUI(L"Textures/Theme/SELECTGUI/ScreenSelectCourse banner frame.png",
			XMFLOAT2(-0.5f, 0.2f), XMFLOAT2(0.4f, 0.24f)));
		ImageGUIs.push_back(GUI(L"Textures/Theme/SELECTGUI/pantsu.png",
			XMFLOAT2(0.0f, -0.88f), XMFLOAT2(1.0f, 0.12f)));
		ImageGUIs.push_back(GUI(L"Textures/Theme/SELECTGUI/shirt.png",
			XMFLOAT2(0.0f, 0.8f), XMFLOAT2(1.0f, 0.20f)));
		ImageGUIs.push_back(GUI(L"Textures/Theme/SELECTGUI/imgSelectMusic.png",
			XMFLOAT2(0.0f, 0.81f), XMFLOAT2(1.0f, 0.20f)));
		ImageGUIs.push_back(GUI(L"Textures/Theme/SELECTGUI/MusicWheel highlight.png",
			XMFLOAT2(0.5f, 0.0f), XMFLOAT2(0.55f, 0.60f)));
		ImageGUIs.push_back(GUI(L"Textures/Theme/SELECTGUI/rave.png",
			XMFLOAT2(-0.014f, 0.1f), XMFLOAT2(0.138f, 0.2f)));

		spinCircle1 = GUI(L"Textures/Theme/ring.png", XMFLOAT2(-0.3f, 0), XMFLOAT2(0.8f, 1.2f));
		spinCircle1.UpdateRotate(MathHelper::Pi / 4.0f, MathHelper::Pi / 4.0f, 0);
		spinCircle2 = GUI(L"Textures/Theme/ring.png", XMFLOAT2(-0.3f, 0), XMFLOAT2(0.8f, 1.2f));
		spinCircle2.UpdateRotate(MathHelper::Pi / 4.0f, -MathHelper::Pi / 4.0f, 0);
		spinCircle3 = GUI(L"Textures/Theme/ring.png", XMFLOAT2(-0.3f, 0), XMFLOAT2(0.8f, 1.2f));

		vector<Music>* pMusicList = Global::GetMusicList();
		int numMusics = (*pMusicList).size();

		for (int i = 0; i < 12; i++)
		{
			if (numMusics != 0)
			{
				auto item = std::make_unique<MusicItem>(GUI(L"Textures/Theme/SELECTGUI/MusicItem.png", XMFLOAT2(0.5f, 0.3f*i - 1.8f), XMFLOAT2(0.6f, 0.15f)), &((*pMusicList)[(Global::GetCurrentMusicIndex() - 7 + 7 * Global::GetNumOfSongs() + i) % Global::GetNumOfSongs()]));
				item->MusicItemBox.SetAlpha(0.8f);
				MusicItems.push_back(std::move(item));
			}
			else
			{
				auto item = std::make_unique<MusicItem>(GUI(L"Textures/Theme/SELECTGUI/MusicItem.png", XMFLOAT2(0.5f, 0.3f*i - 1.8f), XMFLOAT2(0.6f, 0.15f)), NULL);
				item->MusicItemBox.SetAlpha(0.8f);
				MusicItems.push_back(std::move(item));
			}
		}

		introImage = std::make_unique<GUI>(L"Textures/black.png", XMFLOAT2(-0.51f, 0.19f), XMFLOAT2(0.38f, 0.14f));
		if (numMusics != 0)
			introImage->LoadSRV(MusicItems[7]->pMusic->GetIntroImage(), true);

		MenuGUIShader = std::make_unique<GUIShader>(L"GUIShaderFile.hlsl");

		FadeBlack = GUI(L"Textures/black.png", XMFLOAT2(0.0f, 0.0f), XMFLOAT2(10.0f, 10.0f));
		FadeBlack.SetAlpha(FadeAlpha);
		//LoadSounds

		//LoadShaders
		triangleShader = std::make_unique<TriangleShader>(L"TriangleShaderFile.hlsl");

		//Initialize Animation Factors
		scaleTimeSum = 1.0f;
	}

	SceneStatus Update(float dt)
	{
		soundSystem->Update();
		streakParticleEmitter->generateParticles(XMFLOAT3(MathHelper::RandF()*8.0f - 4.0f, -3.7f, 0), dt);
		ParticleManager::update(*cam, dt);
		UpdateBG(dt);
		UpdateGUI(dt);

		if (!MusicPlay)
		{
			MusicPlayDelayTimeSum += dt*0.5f;

			if (MusicPlayDelayTimeSum >= 0.2f)
			{
				MusicPlayDelayTimeSum = 0.0f;
				MusicPlay = true;
				soundSystem->PlayMusic(Global::GetCurrentMusic()->GetIntroSoundPath().c_str(), true);
			}
		}

		if (KeyPressed)
		{
			pressTimeSum += dt;

			if (pressTimeSum >= MUSIC_SCROLL_TIME_DIFFERENCE)
			{
				pressTimeSum = 0.0f;
				KeyPressed = false;
				KeyDirection = 0;
				introImage->LoadSRV(Global::GetCurrentMusic()->GetIntroImage(), false);
				scrollAnimationFactor = 0.0f;
			}
		}
		else
		{
			//0x8000과 and 연산을 해주면, 바로 현재 시점에 눌렸는지를 체크해준다.
			//0x8000과 and 연산을 하지 않으면, 누적된 esc입력값을 받아서, 과거에 눌렸었는지까지 체크한다.
			if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			{
				soundSystem->PlaySoundEffect(SOUND_PREV_SCENE);
				soundSystem->StopMusic();
				KeyPressed = true;
				FadeScreen = FADE_OUT;
				nextScene=SceneStatus::MAIN;
			}

			//Enter 키가 눌리면
			if (GetAsyncKeyState(VK_RETURN)&0x8000)
			{
				soundSystem->PlaySoundEffect(SOUND_SELECT);
				soundSystem->StopMusic();
				MusicPlay = false;
				//현재 선택한 곡을 플레이하는, 플레이 창으로 이동한다.
				KeyPressed = true;
				FadeScreen = FADE_OUT;
				nextScene=SceneStatus::INGAME;
			}
			
			if (GetAsyncKeyState(VK_UP) &0x8000)
			{
				soundSystem->PlaySoundEffect(SOUND_MUSIC_SCROLL);

				if(MusicPlay)
				soundSystem->StopMusic();

				MusicPlay = false;

				MusicPlayDelayTimeSum = 0.0f;
				KeyPressed = true;
				Global::UpWheelMusicIndex();
				KeyDirection = 1;

				std::unique_ptr<MusicItem> tmpItem;

				//KeyDirection이 1이면 위에 있는 것을 아래로 가져오게 된다. 따라서, 맨 아래에 있는 것을 맨 위에 넣고 정보를 갱신해줘야 한다.
				//KeyDirection이 -1이면 아래 있는 것을 위로 가져오게 된다.
					tmpItem = std::move(MusicItems.back());
					int currentMusicIndex = Global::GetCurrentMusicIndex();
					int frontMusicIndex = ((currentMusicIndex - 7) + 7 * Global::GetNumOfSongs()) % Global::GetNumOfSongs();
					tmpItem->pMusic = &(Global::GetMusicList()->at(frontMusicIndex));
					tmpItem->txtArtist.SetText(tmpItem->pMusic->GetArtist());
					tmpItem->txtTitle.SetText(tmpItem->pMusic->GetTitle());
					tmpItem->txtBPM.SetText(tmpItem->pMusic->GetstrBPM());

					MusicItems.pop_back();
					MusicItems.push_front(std::move(tmpItem));

				}

			if (GetAsyncKeyState(VK_DOWN) & 0x8000)
			{
				soundSystem->PlaySoundEffect(SOUND_MUSIC_SCROLL);

				if (MusicPlay)
					soundSystem->StopMusic();

				MusicPlay = false;

				MusicPlayDelayTimeSum = 0.0f;
				KeyPressed = true;
				Global::DownWheelMusicIndex();
				KeyDirection = -1;

				std::unique_ptr<MusicItem> tmpItem;

				tmpItem = std::move(MusicItems.front());
				int currentMusicIndex = Global::GetCurrentMusicIndex();
				int backMusicIndex= (currentMusicIndex + 4) % Global::GetNumOfSongs();
				tmpItem->pMusic = &(Global::GetMusicList()->at(backMusicIndex));
				tmpItem->txtArtist.SetText(tmpItem->pMusic->GetArtist());
				tmpItem->txtTitle.SetText(tmpItem->pMusic->GetTitle());
				tmpItem->txtBPM.SetText(tmpItem->pMusic->GetstrBPM());

				MusicItems.pop_front();
				MusicItems.push_back(std::move(tmpItem));
			}
			}
		


		if (FadeAlpha == 1.0f)
		{
			ParticleManager::clean();
			return nextScene;
		}

		return SceneStatus::SELECT_MUSIC;
	}

	virtual void Render()
	{
		//Render BG
		RenderBackground();
		//Render Streak
		ParticleManager::render(particleShader.get(), cam.get());


		//Render Triangle
		RenderTriangles();
		//Render GUI
		RenderGUI();
	}


	 ~SelectMusicScene() override = default;
};