#pragma once

#include "Scene.h"
#include "GUI.h"
#include "GUIShader.h"
#include "BackgroundShader.h"
#include "SoundSystem.h"
#include "ParticleManager.h"
#include "ParticleEmitter.h"
#include "CautionMark.h"
#include "Entity.h"
#include "EntityShader.h"

#include <memory>
#include <vector>
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

class OptionScene : public Scene {
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

    bool KeyPressed;
    float pressTimeSum;
    float scaleTimeSum;

    float FadeAlpha;
    int FadeScreen;

    float logoAlpha;

    std::vector<GUI> ImageMenuGUIs;
    std::vector<GUI> ImageGUIs;

    GUI FadeBlack;

    GUI OptionPageImage;
    std::unique_ptr<GUIShader> MenuGUIShader;

    SoundSystem* soundSystem;

private:
    void UpdateGUI(float dt);
    void UpdateBG(float dt);
    void RenderBackground();
    void RenderGUI();

public:
    explicit OptionScene(SoundSystem* soundSystem);
    ~OptionScene() override;

    SceneStatus Update(float dt) override;
    void Render() override;
};

