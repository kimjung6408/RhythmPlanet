#include "OptionScene.h"

#include "globalDeviceContext.h"

OptionScene::OptionScene(SoundSystem* soundSystem)
    : CurrentSelectMenu(MENU_SELECT_MUSIC), nextScene(SceneStatus::OPTION),
      backgroundShader(std::make_unique<BackgroundShader>(L"MainBGShader.hlsl")),
      uvOffsetBG(0.0f, 0.0f), uvOffsetGrid(0.0f, 0.0f),
      MenuNormalScale(0.5f, 0.2f), MenuCurrentScale(0.6f, 0.25f),
      cam(std::make_unique<Camera>()), KeyPressed(false), pressTimeSum(0.0f),
      scaleTimeSum(1.0f), FadeAlpha(1.0f), FadeScreen(FADE_IN), logoAlpha(0.4f),
      FadeBlack(GUI(L"Textures/black.png", XMFLOAT2(0, 0), XMFLOAT2(10.0f, 10.0f))),
      MenuGUIShader(std::make_unique<GUIShader>(L"GUIShaderFile.hlsl")),
      soundSystem(soundSystem) {
    this->soundSystem->StopMusic();
    this->soundSystem->PlayMusic("Sounds/Music/Option.mp3", true);

    cam->LookAt(XMFLOAT3(0, 0, -10), XMFLOAT3(0, 0, 0), XMFLOAT3(0, 1, 0));
    cam->UpdateViewMatrix();

    particleTexture = std::make_unique<ParticleTexture>(L"Textures/streak.jpg", 1);
    streakParticleEmitter = std::make_unique<ParticleEmitter>(
        particleTexture.get(), XMFLOAT3(0, -1, 0), 19, 10.6f, 0.1f, 1,
        XMFLOAT3(0.2f, 3.0f, 0));
    streakParticleEmitter->setDirection(XMFLOAT3(0, 1, 0), 0.0f);
    streakParticleEmitter->setScale(XMFLOAT3(0.2f, 3.0f, 0), 0.2f);
    particleShader = std::make_unique<ParticleShader>(L"ParticleShaderFile.hlsl");

    HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), L"Textures/Theme/bg.jpg", nullptr, nullptr,
                                              SRV_BG.ReleaseAndGetAddressOf(), nullptr));
    HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), L"Textures/Theme/grid.jpg", nullptr, nullptr,
                                              SRV_grid.ReleaseAndGetAddressOf(), nullptr));

    FadeBlack.SetAlpha(FadeAlpha);
    ImageMenuGUIs.push_back(GUI(L"Textures/Theme/OptionPage.png", XMFLOAT2(0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f)));
}

OptionScene::~OptionScene() = default;

void OptionScene::UpdateGUI(float dt) {
    for (auto& gui : ImageMenuGUIs) {
        if (scaleTimeSum >= 2.0f) {
            scaleTimeSum -= 1.0f;
        }
        gui.Update(dt);
    }

    logoAlpha -= dt * 1.0f;

    switch (FadeScreen) {
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

    if (FadeAlpha == 0.0f) {
        FadeScreen = NO_FADE;
    }
}

void OptionScene::UpdateBG(float dt) {
    uvOffsetGrid.y += GRID_VELOCITY * dt;

    uvOffsetBG.x += dt * BG_HORIZONTAL_VELOCITY;
    uvOffsetBG.y += dt * BG_VERTICAL_VELOCITY;

    if (uvOffsetGrid.y >= 1.0f) {
        uvOffsetGrid.y -= 1.0f;
    }

    if (uvOffsetBG.x <= -1.0f) {
        uvOffsetBG.x += 1.0f;
    }

    if (uvOffsetBG.y <= -1.0f) {
        uvOffsetBG.y += 1.0f;
    }
}

void OptionScene::RenderBackground() {
    Global::Context()->IASetInputLayout(backgroundShader->InputLayout());
    Global::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = sizeof(XMFLOAT2);
    UINT offset = 0;
    ID3D11Buffer* mVB = backgroundShader->VB();
    Global::Context()->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);

    ID3DX11EffectTechnique* tech = backgroundShader->getTech();
    tech->GetPassByIndex(0)->Apply(0, Global::Context());

    backgroundShader->LoadBG(SRV_BG.Get());
    backgroundShader->LoadGridImage(SRV_grid.Get());
    backgroundShader->Load_uvOffsetBG(uvOffsetBG);
    backgroundShader->Load_uvOffsetGrid(uvOffsetGrid);

    Global::Context()->Draw(6, 0);

    Global::finishRender();
}

void OptionScene::RenderGUI() {
    for (auto& gui : ImageMenuGUIs) {
        gui.Render(MenuGUIShader.get());
    }

    FadeBlack.Render(MenuGUIShader.get());
}

SceneStatus OptionScene::Update(float dt) {
    soundSystem->Update();
    streakParticleEmitter->generateParticles(XMFLOAT3(MathHelper::RandF() * 8.0f - 4.0f, -3.7f, 0), dt);
    ParticleManager::update(*cam, dt);
    UpdateBG(dt);
    UpdateGUI(dt);

    if (KeyPressed) {
        pressTimeSum += dt;

        if (FadeAlpha == 1.0f) {
            soundSystem->StopMusic();
            return nextScene;
        }

        if (pressTimeSum >= PRESS_TIME_DIFFERENCE) {
            pressTimeSum = 0.0f;
            KeyPressed = false;
        }
    } else {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            soundSystem->PlaySoundEffect(SOUND_PREV_SCENE);
            soundSystem->StopMusic();
            KeyPressed = true;
            FadeScreen = FADE_OUT;
            nextScene = SceneStatus::MAIN;
        } else if (GetAsyncKeyState(VK_RETURN)) {
            soundSystem->PlaySoundEffect(SOUND_SELECT);
            KeyPressed = true;

            FadeScreen = FADE_OUT;
            nextScene = SceneStatus::MAIN;

        } else if ((GetAsyncKeyState(VK_UP) & 0x8000) || (GetAsyncKeyState(VK_LEFT) & 0x8000)) {
            soundSystem->PlaySoundEffect(SOUND_MAIN_SCROLL);
            KeyPressed = true;
        } else if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState(VK_RIGHT)) {
            soundSystem->PlaySoundEffect(SOUND_MAIN_SCROLL);
            KeyPressed = true;
        }
    }

    return SceneStatus::OPTION;
}

void OptionScene::Render() {
    RenderBackground();
    ParticleManager::render(particleShader.get(), cam.get());
    RenderGUI();
}

