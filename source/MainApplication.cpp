

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

	//하단에 초기화 문장을 기술함. 위에 기술하면 안됨.
	RenderState::initAllStates();
	ParticleManager::initialize();


	//Main으로 씬을 연결.
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
	//다음 씬을 받아온다.
	SceneStatus retStatus=currentSceneStatus;

	if(CurrentScene!=NULL)
	retStatus=CurrentScene->Update(dt);

	//현재 씬과 다음 씬이 동일하면 리턴한다.
	if (retStatus == currentSceneStatus) return;
	else //현재 씬과 다음 씬이 다를 경우
	{
		Scene* tmpNewScene = NULL;
		//새로운 씬을 생성하여 출력한다.
		switch (retStatus)
		{
		case SceneStatus::MAIN:
			//moveToMain
			tmpNewScene = new MainScene(soundSystem);

			break;
		case SceneStatus::SELECT_MUSIC:
			//moveToSelectMusic
			tmpNewScene = new SelectMusicScene(soundSystem);
			//music data를 select music 씬에서 참조 가능하도록 설정한다.
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

		//필요한 처리가 끝나면 현재의 화면을 삭제하고
		delete CurrentScene;
		//새로운 씬을 설정한다.
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