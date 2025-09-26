#include "Animation.h"

#include "globalDeviceContext.h"

Animation::Animation(XMFLOAT2 position, XMFLOAT3 rotation, XMFLOAT2 scale,
                     int numColumn, int numRow,
                     float updateTimeDifference, AnimState animState, LPCWSTR imagePath)
    : state(animState), rowIdx(0), columnIdx(-1), startColumn(-1), startRow(-1),
      maxColumn(numColumn), maxRow(numRow), updateTimeDifference(updateTimeDifference),
      loopDirection(1), totalCount(0), currentCount(1), updateTimeSum(0.0f),
      Position(position), Rotation(rotation), Scale(scale),
      FadeFactor(0.0f), Alpha(1.0f) {
    HR(D3DX11CreateShaderResourceViewFromFileW(
        Global::Device(), imagePath, nullptr, nullptr, imageSRV.ReleaseAndGetAddressOf(), nullptr));
}

void Animation::setTimeInterval(float time) {
    updateTimeDifference = time;
    updateTimeSum = 0;
}

void Animation::start() {
    startColumn = 0;
    startRow = 0;
    columnIdx = startColumn;
    rowIdx = startRow;
    totalCount = maxColumn * maxRow;
}

int Animation::getRowIdx() const {
    return rowIdx;
}

int Animation::getColumnIdx() const {
    return columnIdx;
}

void Animation::setImageIndex(int row, int column) {
    rowIdx = row;
    columnIdx = column;
    currentCount = (rowIdx * (maxColumn + 1) + (columnIdx + 1))
                 - (startRow * (maxColumn + 1) + (startColumn + 1)) - 1;
}

bool Animation::isStarted() const {
    return columnIdx != -1;
}

void Animation::pause() {
    state = AnimState::FREEZE;
}

void Animation::resumeLoop() {
    if (state == AnimState::FREEZE) {
        state = AnimState::LOOP;
    }
}

void Animation::resumeNoLoop() {
    if (state == AnimState::FREEZE) {
        state = AnimState::NO_LOOP;
    }
}

void Animation::resumeReverseLoop() {
    if (state == AnimState::FREEZE) {
        state = AnimState::REVERSE_LOOP;
    }
}

void Animation::SetAnimationState(AnimState newState) {
    state = newState;
}

bool Animation::Update(float dt) {
    bool isAlive = true;
    if (state == AnimState::FREEZE || columnIdx == -1 || (maxRow == 0 && maxColumn == 0) || totalCount == 1) {
        return isAlive;
    }

    updateTimeSum += dt;

    if (updateTimeSum >= updateTimeDifference) {
        if (currentCount == totalCount) {
            switch (state) {
            case AnimState::NO_LOOP:
                isAlive = false;
                break;
            case AnimState::LOOP:
                if (loopDirection == 1) {
                    columnIdx = startColumn;
                    rowIdx = startRow;
                } else {
                    columnIdx = (startColumn + totalCount - 1) % (maxColumn + 1);
                    rowIdx = startRow + (startColumn + totalCount - 1) / (maxColumn + 1);
                }
                currentCount = 1;
                break;
            case AnimState::REVERSE_LOOP:
                if (loopDirection == 1) {
                    columnIdx = (startColumn + totalCount - 2) % (maxColumn + 1);
                    rowIdx = startRow + (startColumn + totalCount - 2) / (maxColumn + 1);
                } else {
                    columnIdx = (startColumn + 1) % (maxColumn + 1);
                    rowIdx = startRow + (startColumn + 1) / (maxColumn + 1);
                }
                currentCount = 2;
                loopDirection *= -1;
                break;
            case AnimState::FREEZE:
                break;
            }
        } else {
            ++currentCount;
            if (loopDirection == 1) {
                columnIdx = (startColumn + currentCount - 1) % (maxColumn + 1);
                rowIdx = startRow + (startColumn + currentCount - 1) / (maxColumn + 1);
            } else {
                columnIdx = (startColumn + totalCount - currentCount) % (maxColumn + 1);
                rowIdx = startRow + (startColumn + totalCount - currentCount) / (maxColumn + 1);
            }
        }
        updateTimeSum -= updateTimeDifference;
    }

    return isAlive;
}

void Animation::Render(AnimationShader* shader) {
    Global::Context()->RSSetState(RenderState::NoCullRS);
    Global::Context()->IASetInputLayout(shader->InputLayout());
    Global::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = sizeof(XMFLOAT2);
    UINT offset = 0;
    ID3D11Buffer* pVB = shader->VB();
    Global::Context()->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);

    XMMATRIX posMatrix = XMMatrixTranslation(Position.x, Position.y, 0);
    XMMATRIX rotMatrix = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
    XMMATRIX scaleMatrix = XMMatrixScaling(Scale.x, Scale.y, 0);
    XMMATRIX worldMatrix = scaleMatrix * rotMatrix * posMatrix;
    shader->LoadWorldMatrix(worldMatrix);

    shader->LoadTexture(imageSRV.Get());
    shader->LoadNumRowColumn(maxRow, maxColumn);
    shader->LoadCurrentIndex(static_cast<float>(rowIdx), static_cast<float>(columnIdx));
    shader->LoadFadeFactor(FadeFactor);
    shader->LoadAlpha(Alpha);

    shader->getTech()->GetPassByIndex(0)->Apply(0, Global::Context());

    Global::Context()->Draw(6, 0);
    Global::finishRender();
}

