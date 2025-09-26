#pragma once

#include "d3dApp.h"
#include "RenderState.h"
#include "AnimationShader.h"

#include <wrl/client.h>

class Animation {
public:
    enum class AnimState { NO_LOOP, LOOP, REVERSE_LOOP, FREEZE };

    Animation(XMFLOAT2 position, XMFLOAT3 rotation, XMFLOAT2 scale,
              int numColumn, int numRow,
              float updateTimeDifference, AnimState animState, LPCWSTR imagePath);

    void setTimeInterval(float time);
    void start();

    int getRowIdx() const;
    int getColumnIdx() const;
    void setImageIndex(int row, int column);

    bool isStarted() const;

    void pause();
    void resumeLoop();
    void resumeNoLoop();
    void resumeReverseLoop();
    void SetAnimationState(AnimState state);

    bool Update(float dt);
    void Render(AnimationShader* shader);

private:
    AnimState state;
    int rowIdx;
    int columnIdx;

    int startColumn;
    int startRow;
    int maxColumn;
    int maxRow;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> imageSRV;
    float updateTimeDifference;
    int loopDirection;

    int totalCount;
    int currentCount;

    float updateTimeSum;

    XMFLOAT2 Position;
    XMFLOAT3 Rotation;
    XMFLOAT2 Scale;
    float FadeFactor;
    float Alpha;
};

