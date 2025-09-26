#pragma once

#include "d3dApp.h"
#include "globalDeviceContext.h"
#include "ShaderProgram.h"

#include <algorithm>
#include <cctype>
#include <codecvt>
#include <fstream>
#include <cstdlib>
#include <locale>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

struct FontGlyph
{
    XMFLOAT2 uvOffset;
    XMFLOAT2 uvScale;
    float width;
    float height;
    float xOffset;
    float yOffset;
    float xAdvance;
};

class FontShader : public ShaderProgram
{
private:
    ID3D11InputLayout* mInputLayout;
    ID3DX11EffectTechnique* mTech;
    ID3D11Buffer* mVB;
    ID3DX11EffectShaderResourceVariable* mTexture;
    ID3DX11EffectVariable* mFadeFactor;
    ID3DX11EffectVariable* mAlpha;
    ID3DX11EffectVariable* mUVOffset;
    ID3DX11EffectVariable* mUVScale;
    ID3DX11EffectMatrixVariable* mWorld;

    ID3D11ShaderResourceView* SRV;

    float mTextureWidth;
    float mTextureHeight;
    float mCellWidth;
    float mCellHeight;
    float mLineHeight;
    float mBaseline;
    float mTracking;

    std::unordered_map<int, FontGlyph> mGlyphs;
    FontGlyph mFallbackGlyph;

private:
    struct GlyphOverride
    {
        bool hasWidth = false;
        bool hasHeight = false;
        bool hasXOffset = false;
        bool hasYOffset = false;
        bool hasAdvance = false;
        bool hasPosition = false;

        float width = 0.0f;
        float height = 0.0f;
        float xOffset = 0.0f;
        float yOffset = 0.0f;
        float advance = 0.0f;
        float x = 0.0f;
        float y = 0.0f;
    };

private:
    void initialiseEffectBindings()
    {
        mTech = Shader()->GetTechniqueByName("FontTech");
        mTexture = Shader()->GetVariableByName("Image")->AsShaderResource();
        mWorld = Shader()->GetVariableByName("matWorld")->AsMatrix();
        mFadeFactor = Shader()->GetVariableByName("fadeFactor");
        mAlpha = Shader()->GetVariableByName("Alpha");
        mUVOffset = Shader()->GetVariableByName("UVOffset");
        mUVScale = Shader()->GetVariableByName("UVScale");

        mTexture->SetResource(SRV);
    }

    void createInputLayout()
    {
        D3D11_INPUT_ELEMENT_DESC layoutDesc[1] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };

        D3DX11_PASS_DESC passDesc;
        mTech->GetPassByIndex(0)->GetDesc(&passDesc);
        HR(Global::Device()->CreateInputLayout(layoutDesc, 1, passDesc.pIAInputSignature,
            passDesc.IAInputSignatureSize, &mInputLayout));
    }

    std::string toNarrow(const std::wstring& wide) const
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.to_bytes(wide);
    }

    static std::string trim(const std::string& value)
    {
        const auto begin = std::find_if_not(value.begin(), value.end(), [](unsigned char ch) { return std::isspace(ch); });
        const auto end = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char ch) { return std::isspace(ch); }).base();
        if (begin >= end)
        {
            return std::string();
        }
        return std::string(begin, end);
    }

    static bool parseKeyValue(const std::string& token, std::string& key, std::string& value)
    {
        const auto pos = token.find('=');
        if (pos == std::string::npos)
        {
            return false;
        }

        key = token.substr(0, pos);
        value = token.substr(pos + 1);
        return true;
    }

    void applyGeneralValue(const std::string& key, const std::string& value)
    {
        if (key == "textureWidth")
        {
            mTextureWidth = static_cast<float>(atof(value.c_str()));
        }
        else if (key == "textureHeight")
        {
            mTextureHeight = static_cast<float>(atof(value.c_str()));
        }
        else if (key == "cellWidth")
        {
            mCellWidth = static_cast<float>(atof(value.c_str()));
        }
        else if (key == "cellHeight")
        {
            mCellHeight = static_cast<float>(atof(value.c_str()));
        }
        else if (key == "lineHeight")
        {
            mLineHeight = static_cast<float>(atof(value.c_str()));
        }
        else if (key == "baseline")
        {
            mBaseline = static_cast<float>(atof(value.c_str()));
        }
        else if (key == "tracking")
        {
            mTracking = static_cast<float>(atof(value.c_str()));
        }
    }

    void buildDefaultGlyphs()
    {
        mGlyphs.clear();

        const float effectiveTextureWidth = (mTextureWidth > 0.0f) ? mTextureWidth : mCellWidth * 16.0f;
        const float effectiveTextureHeight = (mTextureHeight > 0.0f) ? mTextureHeight : mCellHeight * 16.0f;
        const float uvWidth = (effectiveTextureWidth > 0.0f) ? mCellWidth / effectiveTextureWidth : 0.0f;
        const float uvHeight = (effectiveTextureHeight > 0.0f) ? mCellHeight / effectiveTextureHeight : 0.0f;

        for (int code = 0; code < 256; ++code)
        {
            const int column = code % 16;
            const int row = code / 16;

            FontGlyph glyph;
            glyph.width = mCellWidth;
            glyph.height = mCellHeight;
            glyph.xOffset = 0.0f;
            glyph.yOffset = 0.0f;
            glyph.xAdvance = mCellWidth + mTracking;
            glyph.uvOffset = XMFLOAT2(column * uvWidth, row * uvHeight);
            glyph.uvScale = XMFLOAT2(uvWidth, uvHeight);

            mGlyphs.emplace(code, glyph);
        }

        const int fallbackCode = static_cast<int>('?');
        auto it = mGlyphs.find(fallbackCode);
        mFallbackGlyph = (it != mGlyphs.end()) ? it->second : FontGlyph{};
    }

    void applyOverrides(const std::unordered_map<int, GlyphOverride>& overrides)
    {
        const float effectiveTextureWidth = (mTextureWidth > 0.0f) ? mTextureWidth : 1.0f;
        const float effectiveTextureHeight = (mTextureHeight > 0.0f) ? mTextureHeight : 1.0f;

        for (const auto& pair : overrides)
        {
            const int code = pair.first;
            const GlyphOverride& data = pair.second;
            FontGlyph& glyph = mGlyphs[code];

            if (data.hasPosition)
            {
                glyph.uvOffset = XMFLOAT2(data.x / effectiveTextureWidth, data.y / effectiveTextureHeight);
            }

            if (data.hasWidth)
            {
                glyph.width = data.width;
                glyph.uvScale.x = (effectiveTextureWidth > 0.0f) ? data.width / effectiveTextureWidth : glyph.uvScale.x;
            }

            if (data.hasHeight)
            {
                glyph.height = data.height;
                glyph.uvScale.y = (effectiveTextureHeight > 0.0f) ? data.height / effectiveTextureHeight : glyph.uvScale.y;
            }

            if (data.hasXOffset)
            {
                glyph.xOffset = data.xOffset;
            }

            if (data.hasYOffset)
            {
                glyph.yOffset = data.yOffset;
            }

            if (data.hasAdvance)
            {
                glyph.xAdvance = data.advance;
            }
            else if (data.hasWidth)
            {
                glyph.xAdvance = data.width + mTracking;
            }
        }
    }

    bool loadFontMetrics(LPCWSTR metricsPath)
    {
        if (metricsPath == nullptr)
        {
            buildDefaultGlyphs();
            return false;
        }

        std::ifstream input(toNarrow(metricsPath));
        if (!input.is_open())
        {
            buildDefaultGlyphs();
            return false;
        }

        std::unordered_map<std::string, std::string> generalValues;
        std::unordered_map<int, GlyphOverride> overrides;

        std::string line;
        while (std::getline(input, line))
        {
            const auto commentPos = line.find('#');
            if (commentPos != std::string::npos)
            {
                line = line.substr(0, commentPos);
            }

            line = trim(line);
            if (line.empty())
            {
                continue;
            }

            std::istringstream stream(line);
            std::string firstToken;
            stream >> firstToken;

            if (firstToken == "char")
            {
                int id = -1;
                GlyphOverride overrideData;
                std::string token;

                while (stream >> token)
                {
                    std::string key;
                    std::string value;
                    if (parseKeyValue(token, key, value))
                    {
                        if (key == "id")
                        {
                            id = atoi(value.c_str());
                        }
                        else if (key == "width")
                        {
                            overrideData.hasWidth = true;
                            overrideData.width = static_cast<float>(atof(value.c_str()));
                        }
                        else if (key == "height")
                        {
                            overrideData.hasHeight = true;
                            overrideData.height = static_cast<float>(atof(value.c_str()));
                        }
                        else if (key == "xoffset")
                        {
                            overrideData.hasXOffset = true;
                            overrideData.xOffset = static_cast<float>(atof(value.c_str()));
                        }
                        else if (key == "yoffset")
                        {
                            overrideData.hasYOffset = true;
                            overrideData.yOffset = static_cast<float>(atof(value.c_str()));
                        }
                        else if (key == "xadvance")
                        {
                            overrideData.hasAdvance = true;
                            overrideData.advance = static_cast<float>(atof(value.c_str()));
                        }
                        else if (key == "x")
                        {
                            overrideData.hasPosition = true;
                            overrideData.x = static_cast<float>(atof(value.c_str()));
                        }
                        else if (key == "y")
                        {
                            overrideData.hasPosition = true;
                            overrideData.y = static_cast<float>(atof(value.c_str()));
                        }
                    }
                    else if (id < 0)
                    {
                        id = atoi(token.c_str());
                    }
                }

                if (id >= 0)
                {
                    overrides[id] = overrideData;
                }
            }
            else
            {
                std::string token = firstToken;
                std::string key;
                std::string value;
                if (parseKeyValue(token, key, value))
                {
                    generalValues[key] = value;
                }

                while (stream >> token)
                {
                    if (parseKeyValue(token, key, value))
                    {
                        generalValues[key] = value;
                    }
                }
            }
        }

        for (const auto& pair : generalValues)
        {
            applyGeneralValue(pair.first, pair.second);
        }

        buildDefaultGlyphs();
        applyOverrides(overrides);
        return true;
    }

public:
    FontShader(LPCWSTR shaderFilePath, LPCWSTR fontAtlasPath, LPCWSTR metricsPath)
        : ShaderProgram(shaderFilePath),
        mInputLayout(nullptr),
        mTech(nullptr),
        mVB(nullptr),
        mTexture(nullptr),
        mFadeFactor(nullptr),
        mAlpha(nullptr),
        mUVOffset(nullptr),
        mUVScale(nullptr),
        mWorld(nullptr),
        SRV(nullptr),
        mTextureWidth(0.0f),
        mTextureHeight(0.0f),
        mCellWidth(64.0f),
        mCellHeight(64.0f),
        mLineHeight(64.0f),
        mBaseline(52.0f),
        mTracking(0.0f)
    {
        HR(D3DX11CreateShaderResourceViewFromFileW(Global::Device(), fontAtlasPath, 0, 0, &SRV, 0));

        std::vector<XMFLOAT2> vertices =
        {
            XMFLOAT2(-1.0f, 1.0f),
            XMFLOAT2(-1.0f, -1.0f),
            XMFLOAT2(1.0f, 1.0f),
            XMFLOAT2(1.0f, 1.0f),
            XMFLOAT2(-1.0f, -1.0f),
            XMFLOAT2(1.0f, -1.0f)
        };

        mVB = nsCreator::createVertexBuffer(vertices);

        loadFontMetrics(metricsPath);

        initialiseEffectBindings();
        createInputLayout();
    }

    ~FontShader()
    {
        ReleaseCOM(mInputLayout);
        ReleaseCOM(mVB);
        ReleaseCOM(SRV);
    }

    ID3DX11EffectTechnique* getTech()
    {
        return mTech;
    }

    ID3D11InputLayout* InputLayout()
    {
        return mInputLayout;
    }

    ID3D11Buffer* VB()
    {
        return mVB;
    }

    void LoadWorldMatrix(XMMATRIX& world)
    {
        mWorld->SetMatrix(reinterpret_cast<float*>(&world));
    }

    void LoadFadeFactor(float FadeFactor)
    {
        mFadeFactor->SetRawValue(&FadeFactor, 0, sizeof(float));
    }

    void LoadAlpha(float Alpha)
    {
        mAlpha->SetRawValue(&Alpha, 0, sizeof(float));
    }

    void ApplyGlyph(const FontGlyph& glyph)
    {
        mUVOffset->SetRawValue(&glyph.uvOffset, 0, sizeof(XMFLOAT2));
        mUVScale->SetRawValue(&glyph.uvScale, 0, sizeof(XMFLOAT2));
    }

    void ApplyGlyph(char character)
    {
        const FontGlyph& glyph = Glyph(character);
        ApplyGlyph(glyph);
    }

    const FontGlyph& Glyph(char character) const
    {
        const int key = static_cast<unsigned char>(character);
        auto it = mGlyphs.find(key);
        if (it != mGlyphs.end())
        {
            return it->second;
        }

        return mFallbackGlyph;
    }

    float CellWidth() const { return mCellWidth; }
    float CellHeight() const { return mCellHeight; }
    float LineHeight() const { return mLineHeight; }
    float Baseline() const { return mBaseline; }
    float Tracking() const { return mTracking; }
};

