#pragma once
#include"globalDeviceContext.h"
class RenderState
{
public:
	static void initAllStates()
	{
		//
		// WireframeRS
		//
		D3D11_RASTERIZER_DESC wireframeDesc;
		ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
		wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
		wireframeDesc.CullMode = D3D11_CULL_BACK;
		wireframeDesc.FrontCounterClockwise = false;
		wireframeDesc.DepthClipEnable = true;

		HR(Global::Device()->CreateRasterizerState(&wireframeDesc, &WireframeRS));

		//
		// NoCullRS
		//
		D3D11_RASTERIZER_DESC noCullDesc;
		ZeroMemory(&noCullDesc, sizeof(D3D11_RASTERIZER_DESC));
		noCullDesc.FillMode = D3D11_FILL_SOLID;
		noCullDesc.CullMode = D3D11_CULL_NONE;
		noCullDesc.FrontCounterClockwise = false;
		noCullDesc.DepthClipEnable = true;

		HR(Global::Device()->CreateRasterizerState(&noCullDesc, &NoCullRS));

		//
		// AlphaToCoverageBS
		//

		D3D11_BLEND_DESC alphaToCoverageDesc = { 0 };
		alphaToCoverageDesc.AlphaToCoverageEnable = true;
		alphaToCoverageDesc.IndependentBlendEnable = false;
		alphaToCoverageDesc.RenderTarget[0].BlendEnable = false;
		alphaToCoverageDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HR(Global::Device()->CreateBlendState(&alphaToCoverageDesc, &AlphaToCoverageBS));

		//
		// TransparentBS
		//

		D3D11_BLEND_DESC transparentDesc = { 0 };
		transparentDesc.AlphaToCoverageEnable = false;
		transparentDesc.IndependentBlendEnable = false;

		transparentDesc.RenderTarget[0].BlendEnable = true;
		transparentDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		transparentDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		transparentDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		transparentDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		transparentDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		transparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HR(Global::Device()->CreateBlendState(&transparentDesc, &TransparentBS));

		//값 수정해야 함
		D3D11_BLEND_DESC AdditiveDesc = { 0 };
		AdditiveDesc.AlphaToCoverageEnable = false;
		AdditiveDesc.IndependentBlendEnable = false;

		AdditiveDesc.RenderTarget[0].BlendEnable = true;
		AdditiveDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		AdditiveDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		AdditiveDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		AdditiveDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		AdditiveDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		AdditiveDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		AdditiveDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HR(Global::Device()->CreateBlendState(&AdditiveDesc, &AdditiveBS));


	}
	static void deleteAllStates()
	{
		ReleaseCOM(WireframeRS);
		ReleaseCOM(NoCullRS);
		ReleaseCOM(AlphaToCoverageBS);
		ReleaseCOM(TransparentBS);
	}
	static ID3D11RasterizerState* WireframeRS;
	static ID3D11RasterizerState* NoCullRS;

	static ID3D11BlendState* AlphaToCoverageBS;
	static ID3D11BlendState* TransparentBS;
	static ID3D11BlendState* AdditiveBS;
	static ID3D11BlendState* SubtractiveBS;
};

ID3D11RasterizerState* RenderState::WireframeRS = 0;
ID3D11RasterizerState* RenderState::NoCullRS = 0;

ID3D11BlendState*      RenderState::AlphaToCoverageBS = 0;
ID3D11BlendState*      RenderState::TransparentBS = 0;
ID3D11BlendState*      RenderState::AdditiveBS = 0;
ID3D11BlendState*      RenderState::SubtractiveBS = 0;