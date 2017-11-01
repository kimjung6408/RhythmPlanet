#pragma once
#include"Scene.h"
#include"GUI.h"
#include"GUIShader.h"
#include"BackgroundShader.h"
#include"Triangle.h"
#include"Music.h"
#include"SoundSystem.h"
#include"Text.h"
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
			txtTitle.SetCharacterGap(0.05f);
			txtArtist.SetCharacterGap(0.09f);
			txtBPM.SetCharacterGap(0.09f);
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
	BackgroundShader* backgroundShader;
	FontShader* fontShader;
	ID3D11ShaderResourceView* SRV_BG;
	ID3D11ShaderResourceView* SRV_grid;
	XMFLOAT2 uvOffsetBG;
	XMFLOAT2 uvOffsetGrid;

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
	vector<GUI> ImageGUIs;
	deque<MusicItem*> MusicItems;
	bool MusicScrollMove; //����ڰ� ���� ����Ű�� ������ ��
	GUI HealthBar;
	GUI StaminaBar;

	GUI spinCircle1;
	GUI spinCircle2;
	GUI spinCircle3;

	GUI* introImage;

	GUI FadeBlack;

	GUIShader* MenuGUIShader;
	vector<Triangle> triangles;
	TriangleShader* triangleShader;

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

		//���� ��ũ�ѷ� �κ� ������Ʈ
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


		//�����÷ο�, ����÷ο� ����.
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
		spinCircle1.Render(MenuGUIShader);
		spinCircle2.Render(MenuGUIShader);
		spinCircle3.Render(MenuGUIShader);


		for (int i = 0; i < MusicItems.size(); i++)
		{
			MusicItems[i]->MusicItemBox.Render(MenuGUIShader);
		}

		for (int i = 0; i < MusicItems.size(); i++)
		{
			MusicItems[i]->txtTitle.Render(fontShader);
			MusicItems[i]->txtArtist.Render(fontShader);
			
			if (i == 7)
				MusicItems[i]->txtBPM.Render(fontShader);
		}

		for (int i = 0; i < ImageGUIs.size(); i++)
		{
			ImageGUIs[i].Render(MenuGUIShader);
		}

		introImage->Render(MenuGUIShader);

		FadeBlack.Render(MenuGUIShader);
	}

	void RenderBackground()
	{
		//Render Background
		Global::Context()->IASetInputLayout(backgroundShader->InputLayout());
		Global::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//VertexBuffer�� �����Ѵ�.
		UINT stride = sizeof(XMFLOAT2);
		UINT offset = 0;
		ID3D11Buffer* mVB = backgroundShader->VB();
		Global::Context()->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);

		//Context�� shader�� technique pass�� �����Ѵ�.
		ID3DX11EffectTechnique* tech = backgroundShader->getTech();
		tech->GetPassByIndex(0)->Apply(0, Global::Context());

		//�������� factor���� �ε��Ѵ�.
		backgroundShader->LoadBG(SRV_BG);
		backgroundShader->LoadGridImage(SRV_grid);
		backgroundShader->Load_uvOffsetBG(uvOffsetBG);
		backgroundShader->Load_uvOffsetGrid(uvOffsetGrid);


		Global::Context()->Draw(6, 0);

		Global::finishRender();
	}

	void RenderTriangles()
	{
		for (int i = 0; i < triangles.size(); i++)
			triangles[i].Render(*cam, triangleShader);
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
		introImage = NULL;

		nextScene = SceneStatus::SELECT_MUSIC;
		//streak, triangle��� ���� ī�޶�.
		cam = new Camera();
		cam->LookAt(XMFLOAT3(0, 0, -10), XMFLOAT3(0, 0, 0), XMFLOAT3(0, 1, 0));
		cam->UpdateViewMatrix();

		backgroundShader = new BackgroundShader(L"MainBGShader.hlsl");
		particleTexture = new ParticleTexture(L"Textures/streak.jpg", 1);
		streakParticleEmitter = new ParticleEmitter(particleTexture, XMFLOAT3(0, -1, 0), 19, 10.6f, 0.1, 1, XMFLOAT3(0.2f, 3.0f, 0));
		streakParticleEmitter->setDirection(XMFLOAT3(0, 1, 0), 0.0f);
		streakParticleEmitter->setScale(XMFLOAT3(0.2f, 3.0f, 0), 0.2);
		particleShader = new ParticleShader(L"ParticleShaderFile.hlsl");
		fontShader = new FontShader(L"FontShaderFile.hlsl", L"Textures/FontAtlas.png");
		//LoadImages
		HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), L"Textures/Theme/bg.jpg", 0, 0, &SRV_BG, 0));
		HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), L"Textures/Theme/grid.jpg", 0, 0, &SRV_grid, 0));

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
				MusicItem* item =new MusicItem(GUI(L"Textures/Theme/SELECTGUI/MusicItem.png", XMFLOAT2(0.5f, 0.3f*i - 1.8f), XMFLOAT2(0.6f, 0.15f)), &((*pMusicList)[(Global::GetCurrentMusicIndex() - 7 + 7 * Global::GetNumOfSongs() + i) % Global::GetNumOfSongs()]));
				item->MusicItemBox.SetAlpha(0.8f);
				MusicItems.push_back(item);
			}
			else
			{
				MusicItem* item = new MusicItem(GUI(L"Textures/Theme/SELECTGUI/MusicItem.png", XMFLOAT2(0.5f, 0.3f*i - 1.8f), XMFLOAT2(0.6f, 0.15f)), NULL);
				item->MusicItemBox.SetAlpha(0.8f);
				MusicItems.push_back(item);
			}
		}

		introImage = new GUI(L"Textures/black.png", XMFLOAT2(-0.51f, 0.19f), XMFLOAT2(0.38f, 0.14f));
		if (numMusics != 0)
			introImage->LoadSRV(MusicItems[7]->pMusic->GetIntroImage(), true);

		MenuGUIShader = new GUIShader(L"GUIShaderFile.hlsl");

		FadeBlack = GUI(L"Textures/black.png", XMFLOAT2(0.0f, 0.0f), XMFLOAT2(10.0f, 10.0f));
		FadeBlack.SetAlpha(FadeAlpha);
		//LoadSounds

		//LoadShaders
		triangleShader = new TriangleShader(L"TriangleShaderFile.hlsl");

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
			//0x8000�� and ������ ���ָ�, �ٷ� ���� ������ ���ȴ����� üũ���ش�.
			//0x8000�� and ������ ���� ������, ������ esc�Է°��� �޾Ƽ�, ���ſ� ���Ⱦ��������� üũ�Ѵ�.
			if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			{
				soundSystem->PlaySoundEffect(SOUND_PREV_SCENE);
				soundSystem->StopMusic();
				KeyPressed = true;
				FadeScreen = FADE_OUT;
				nextScene=SceneStatus::MAIN;
			}

			//Enter Ű�� ������
			if (GetAsyncKeyState(VK_RETURN)&0x8000)
			{
				soundSystem->PlaySoundEffect(SOUND_SELECT);
				soundSystem->StopMusic();
				MusicPlay = false;
				//���� ������ ���� �÷����ϴ�, �÷��� â���� �̵��Ѵ�.
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

				MusicItem* tmpItem;

				//KeyDirection�� 1�̸� ���� �ִ� ���� �Ʒ��� �������� �ȴ�. ����, �� �Ʒ��� �ִ� ���� �� ���� �ְ� ������ ��������� �Ѵ�.
				//KeyDirection�� -1�̸� �Ʒ� �ִ� ���� ���� �������� �ȴ�.
					tmpItem = MusicItems.back();
					int currentMusicIndex = Global::GetCurrentMusicIndex();
					int frontMusicIndex = ((currentMusicIndex - 7) + 7 * Global::GetNumOfSongs()) % Global::GetNumOfSongs();
					tmpItem->pMusic = &(Global::GetMusicList()->at(frontMusicIndex));
					tmpItem->txtArtist.SetText(tmpItem->pMusic->GetArtist());
					tmpItem->txtTitle.SetText(tmpItem->pMusic->GetTitle());
					tmpItem->txtBPM.SetText(tmpItem->pMusic->GetstrBPM());

					MusicItems.pop_back();
					MusicItems.push_front(tmpItem);

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

				MusicItem* tmpItem;

				tmpItem = MusicItems.front();
				int currentMusicIndex = Global::GetCurrentMusicIndex();
				int backMusicIndex= (currentMusicIndex + 4) % Global::GetNumOfSongs();
				tmpItem->pMusic = &(Global::GetMusicList()->at(backMusicIndex));
				tmpItem->txtArtist.SetText(tmpItem->pMusic->GetArtist());
				tmpItem->txtTitle.SetText(tmpItem->pMusic->GetTitle());
				tmpItem->txtBPM.SetText(tmpItem->pMusic->GetstrBPM());

				MusicItems.pop_front();
				MusicItems.push_back(tmpItem);
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
		ParticleManager::render(particleShader, cam);


		//Render Triangle
		RenderTriangles();
		//Render GUI
		RenderGUI();
	}


	 ~SelectMusicScene()
	{
		delete backgroundShader;
		ReleaseCOM(SRV_BG);
		ReleaseCOM(SRV_grid);
		delete streakParticleEmitter;
		delete particleShader;
		delete particleTexture;
		delete introImage;
		delete cam;
		delete MenuGUIShader;

		delete triangleShader;

		for (int i = 0; i < 12; i++)
			delete MusicItems[i];
	}
};