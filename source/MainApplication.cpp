

#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include"globalDeviceContext.h"
#include"Utils.hpp"
#include"Skybox.h"
#include"Box.h"
#include"Triangle.h"
#include"RenderState.h"
#include"ParticleManager.h"
#include"ParticleEmitter.h"
#include"Ball.h"
#include"Scene.h"
#include"MainScene.h"
#include"SelectMusicScene.h"
#include"ResultScene.h"
#include"OptionScene.h"
#include"Music.h"
#include"SoundSystem.h"
#include"InGameScene.h"
#include<fmod.h>

struct Vertex
{
	XMFLOAT3 Pos;
};

class Application : public D3DApp
{
public:
	Application(HINSTANCE hInstance);
	~Application();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:

private:
	SoundSystem* soundSystem;
	SceneStatus currentSceneStatus;
	Scene* CurrentScene;
	vector<Music> musics;
	POINT mLastMousePos;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Application theApp(hInstance);

	if (!theApp.Init())
		return 0;

	return theApp.Run();
}


Application::Application(HINSTANCE hInstance)
	: D3DApp(hInstance),
	 CurrentScene(0)
{
	mMainWndCaption = L"Rhythm Planet";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;
	soundSystem = new SoundSystem();
}

Application::~Application()
{
	RenderState::deleteAllStates();
}

bool Application::Init()
{
	if (!D3DApp::Init())
		return false;
	Global::InitializeDevice(md3dDevice, md3dImmediateContext);

	//�ϴܿ� �ʱ�ȭ ������ �����. ���� ����ϸ� �ȵ�.
	RenderState::initAllStates();
	ParticleManager::initialize();


	//Main���� ���� ����.
	CurrentScene = new MainScene(soundSystem);
	currentSceneStatus = SceneStatus::MAIN;
	return true;
}

void Application::OnResize()
{
	D3DApp::OnResize();
}

void Application::UpdateScene(float dt)
{
	//���� ���� �޾ƿ´�.
	SceneStatus retStatus=currentSceneStatus;

	if(CurrentScene!=NULL)
	retStatus=CurrentScene->Update(dt);

	//���� ���� ���� ���� �����ϸ� �����Ѵ�.
	if (retStatus == currentSceneStatus) return;
	else //���� ���� ���� ���� �ٸ� ���
	{
		Scene* tmpNewScene = NULL;
		//���ο� ���� �����Ͽ� ����Ѵ�.
		switch (retStatus)
		{
		case SceneStatus::MAIN:
			//moveToMain
			tmpNewScene = new MainScene(soundSystem);

			break;
		case SceneStatus::SELECT_MUSIC:
			//moveToSelectMusic
			tmpNewScene = new SelectMusicScene(soundSystem);
			//music data�� select music ������ ���� �����ϵ��� �����Ѵ�.
			//static_cast<SelectMusicScene*>(tmpNewScene)->getMusicList(&musics);
			break;
		case SceneStatus::INGAME:
			tmpNewScene = new InGameScene(soundSystem);
			//moveToIngame
			break;
		case SceneStatus::OPTION:
			tmpNewScene = new OptionScene(soundSystem);
			//moveToOption
			break;
		case SceneStatus::RESULT:
			tmpNewScene = new ResultScene(soundSystem);
			//moveToResultScreen
			break;
		case SceneStatus::ENDGAME:
			SendMessageW(mhMainWnd, WM_DESTROY, 1, NULL);
			break;
		}

		//�ʿ��� ó���� ������ ������ ȭ���� �����ϰ�
		delete CurrentScene;
		//���ο� ���� �����Ѵ�.
		CurrentScene = tmpNewScene;
		currentSceneStatus = retStatus;
	}

}

void Application::DrawScene()
{
	nsGlobalUtils::clearScreen(mRenderTargetView, mDepthStencilView);

	if(CurrentScene!=NULL)
	CurrentScene->Render();
	//Swap Buffer
	HR(mSwapChain->Present(0, 0));
}

void Application::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void Application::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void Application::OnMouseMove(WPARAM btnState, int x, int y)
{

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}