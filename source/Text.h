#pragma once
#include"d3dApp.h"
#include"Utils.hpp"
#include"globalDeviceContext.h"
#include"GeometryGenerator.h"
#include"Camera.h"
#include"FontShader.h"
#include"RenderState.h"
#include <algorithm>
#include <vector>
#define BLING_TIME 0.4f
#define TEXT_ALIGN_LEFT 0
#define TEXT_ALIGN_RIGHT 1
class Text
{
private:
        XMFLOAT2 Position;
        XMFLOAT2 Scale;
        XMFLOAT3 Rotation;

        bool Fade;
        float FadeTimeDifference;
        float FadeTimeSum;
        float FadeFactor;
        float Alpha;
        string text;
        float characterGap;
        int TextAlign;
public:
        Text() {}

        Text(XMFLOAT2 Position, XMFLOAT2 Scale, string text)
        {
                        this->Position = Position;
                this->Scale = Scale;
                this->Fade = false;
                this->FadeFactor = 0;
                this->FadeTimeDifference = 0.4f;
                this->FadeTimeSum = 0;
                this->Rotation = XMFLOAT3(0, 0, 0);
                this->Alpha = 1.0f;
                this->text = text;
                TextAlign = TEXT_ALIGN_LEFT;
                characterGap = 0.0f;
        }

        void Update(float dt)
        {
                if (Fade)
                {
                        FadeTimeSum += dt;

                        if (FadeTimeSum >= FadeTimeDifference)
                        {
                                FadeTimeSum -= FadeTimeDifference;
                        }

                        FadeFactor = (FadeTimeSum / FadeTimeDifference);
                }
                else
                {
                        FadeTimeSum = 0;
                        FadeFactor = 0;
                }
        }

        void Render(FontShader* shader)
        {
                if (shader == nullptr || text.empty())
                {
                        Global::finishRender();
                        return;
                }

                Global::Context()->RSSetState(RenderState::NoCullRS);
                Global::Context()->IASetInputLayout(shader->InputLayout());
                Global::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

                UINT stride = sizeof(XMFLOAT2);
                UINT offset = 0;
                ID3D11Buffer* pVB = shader->VB();
                Global::Context()->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);

                shader->LoadFadeFactor(FadeFactor);
                shader->LoadAlpha(Alpha);

                const float baseWidth = std::max(shader->CellWidth(), 1.0f);
                const float baseHeight = std::max(shader->CellHeight(), 1.0f);
                const float lineAdvance = std::max(shader->LineHeight(), baseHeight) / baseHeight;

                auto computeLineWidth = [&](const std::string& line)
                {
                        float widthSum = 0.0f;
                        for (size_t idx = 0; idx < line.size(); ++idx)
                        {
                                const FontGlyph& glyph = shader->Glyph(line[idx]);
                                widthSum += Scale.x * (glyph.xAdvance / baseWidth);
                                if (idx + 1 < line.size())
                                {
                                        widthSum += characterGap;
                                }
                        }
                        return widthSum;
                };

                std::vector<std::string> lines;
                lines.reserve(4);
                std::string currentLine;
                for (char ch : text)
                {
                        if (ch == '\n')
                        {
                                lines.push_back(currentLine);
                                currentLine.clear();
                        }
                        else
                        {
                                currentLine.push_back(ch);
                        }
                }
                lines.push_back(currentLine);

                float cursorY = 0.0f;
                for (const std::string& line : lines)
                {
                        float cursorX = 0.0f;
                        float alignOffset = 0.0f;
                        if (TextAlign == TEXT_ALIGN_RIGHT)
                        {
                                alignOffset = -computeLineWidth(line);
                        }

                        for (size_t idx = 0; idx < line.size(); ++idx)
                        {
                                const FontGlyph& glyph = shader->Glyph(line[idx]);

                                const float widthFactor = (glyph.width > 0.0f) ? glyph.width / baseWidth : 0.0f;
                                const float heightFactor = (glyph.height > 0.0f) ? glyph.height / baseHeight : 0.0f;
                                const float offsetX = glyph.xOffset / baseWidth;
                                const float offsetY = glyph.yOffset / baseHeight;

                                shader->ApplyGlyph(glyph);

                                if (line[idx] != ' ' && widthFactor > 0.0f && heightFactor > 0.0f)
                                {
                                        XMMATRIX posMatrix = XMMatrixTranslation(alignOffset + Position.x + cursorX + Scale.x * offsetX,
                                                Position.y + cursorY - Scale.y * offsetY,
                                                0);
                                        XMMATRIX rotMatrix = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
                                        XMMATRIX scaleMatrix = XMMatrixScaling(Scale.x * widthFactor, Scale.y * heightFactor, 0);
                                        XMMATRIX worldMatrix = scaleMatrix * rotMatrix * posMatrix;
                                        shader->LoadWorldMatrix(worldMatrix);
                                        shader->getTech()->GetPassByIndex(0)->Apply(0, Global::Context());
                                        Global::Context()->Draw(6, 0);
                                }

                                cursorX += Scale.x * (glyph.xAdvance / baseWidth);
                                cursorX += characterGap;
                        }

                        cursorY -= Scale.y * lineAdvance;
                }

                Global::finishRender();
        }

        void SetScale(XMFLOAT2 scale)
        {
                this->Scale = scale;
        }

        void fade(bool OnOff)
        {
                Fade = OnOff;
        }

        void SetText(string txt)
        {
                text = txt;
        }

        void SetAlign(int align)
        {
                if(align==TEXT_ALIGN_LEFT || align==TEXT_ALIGN_RIGHT)
                TextAlign = align;
        }

        float getFadeFactor()
        {
                return FadeFactor;
        }

        void UpdateRotate(float xAngle, float yAngle, float zAngle)
        {
                Rotation.x += xAngle;
                Rotation.y += yAngle;
                Rotation.z += zAngle;
        }

        void movePosition(float xDelta, float yDelta)
        {
                Position.x += xDelta;
                Position.y += yDelta;
        }

        void SetPosition(float x, float y)
        {
                Position.x = x;
                Position.y = y;
        }

        void SetAlpha(float alpha)
        {
                this->Alpha = alpha;
        }

        void SetCharacterGap(float gap)
        {
                characterGap = gap;
        }

        ~Text()
        {

        }
};
