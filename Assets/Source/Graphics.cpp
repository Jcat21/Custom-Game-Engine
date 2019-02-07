#include "stdafx.h"
#include "Graphics.h"
#include "Shader.h"
#include "engineMath.h"

#pragma comment (lib, "d3d11.lib") 

Graphics::Graphics()
    : mScreenWidth(0)
    , mScreenHeight(0)
    , mSwapchain(nullptr)
    , mDev(nullptr)
    , mDevcon(nullptr)
	, mBackbuffer(nullptr)
	, mDefaultSampler(nullptr)
{
}

Graphics::~Graphics()
{
}

void Graphics::InitD3D(HWND hWnd, float width, float height)
{
    mScreenWidth = width;
    mScreenHeight = height;

    // create a struct to hold information about the swap chain
    DXGI_SWAP_CHAIN_DESC scd;

    // clear out the struct for use
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
    // fill the swap chain description struct
    scd.BufferCount = 1;                                    // one back buffer
    scd.BufferDesc.Width = (UINT)mScreenWidth;
    scd.BufferDesc.Height = (UINT)mScreenHeight;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
    scd.OutputWindow = hWnd;                                // the window to be used
    scd.SampleDesc.Count = 1;                               // how many multisamples
    scd.Windowed = TRUE;                                    // windowed/full-screen mode

    // create a device, device context and swap chain using the information in the scd struct
    HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        D3D11_CREATE_DEVICE_DEBUG,
        NULL,
        NULL,
        D3D11_SDK_VERSION,
        &scd,
        &mSwapchain,
        &mDev,
        NULL,
        &mDevcon);
    DbgAssert(hr == S_OK, "Something wrong with your swap chain");

    // get the address of the back buffer
    ID3D11Texture2D *pBackBuffer;
    hr = mSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    DbgAssert(hr == S_OK, "Something wrong with your back buffer");

    // use the back buffer address to create the render target
    mDev->CreateRenderTargetView(pBackBuffer, NULL, &mBackbuffer);
    pBackBuffer->Release();

	// Set the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = mScreenWidth;
	viewport.Height = mScreenHeight;
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.0f;

	mDevcon->RSSetViewports(1, &viewport);

	// Create depth stencil texture
	CreateDepthStencil((int)mScreenWidth, (int)mScreenHeight, &mDepthStencilTexture, &mDepthStencilView);
	mDepthState = CreateDepthStencilState(true, D3D11_COMPARISON_LESS_EQUAL);
//	mDepthState = CreateDepthStencilState(true, D3D11_COMPARISON_ALWAYS);

	mDevcon->OMSetRenderTargets(1, &mBackbuffer, mDepthStencilView);
	mDevcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mDevcon->OMSetDepthStencilState(mDepthState, 0);

	{   // Create the default sample state
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = mDev->CreateSamplerState(&sampDesc, &mDefaultSampler);
		DbgAssert(hr == S_OK, "Failure Creating Sampler State");
		SetActiveSampler(0, mDefaultSampler);
	}

#if 1
    {   //let's set CCW as the front face, since right hand rule is nice...
        D3D11_RASTERIZER_DESC rastDesc;
        ZeroMemory(&rastDesc, sizeof(rastDesc));
        rastDesc.FrontCounterClockwise = true;
        rastDesc.FillMode = D3D11_FILL_SOLID;
        rastDesc.CullMode = D3D11_CULL_BACK;
        ID3D11RasterizerState* pRaster;
        auto hr = mDev->CreateRasterizerState(&rastDesc, &pRaster);
        DbgAssert(hr == S_OK, "Problem Creating Rasterizer State");
        mDevcon->RSSetState(pRaster);
        pRaster->Release();
    }
#endif
}

void Graphics::BeginFrame(const Color4 &clearColor)
{
    mDevcon->ClearRenderTargetView(mBackbuffer, (const float*)&clearColor);
	mDevcon->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Graphics::EndFrame()
{
    mSwapchain->Present(1, 0);
}

void Graphics::CleanD3D()
{
	mDefaultSampler->Release();
	mDepthState->Release();
	mDepthStencilView->Release();
	mDepthStencilTexture->Release();
	mBackbuffer->Release();
	mSwapchain->Release();
	mDev->Release();
    mDevcon->Release();
}

ID3D11Buffer *Graphics::CreateGraphicsBuffer(const void *initialData, int inDataSize, D3D11_BIND_FLAG inBindFlags, D3D11_CPU_ACCESS_FLAG inCPUAccessFlags, D3D11_USAGE inUsage)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = inUsage;
	bd.ByteWidth = inDataSize;
	bd.BindFlags = inBindFlags;
	bd.CPUAccessFlags = inCPUAccessFlags;

	ID3D11Buffer* toRet = nullptr;
	HRESULT hr = mDev->CreateBuffer(&bd, nullptr, &toRet);
	DbgAssert(hr == S_OK, "Problem Creating Graphics Buffer");

	if (nullptr != initialData)
		UploadBuffer(toRet, initialData, inDataSize);

	return toRet;
}

void Graphics::UploadBuffer(ID3D11Buffer* buffer, const void* data, size_t dataSize)
{
	D3D11_MAPPED_SUBRESOURCE mapped;
	mDevcon->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, data, dataSize);
	mDevcon->Unmap(buffer, 0);
}

bool Graphics::CreateDepthStencil(int inWidth, int inHeight, ID3D11Texture2D **pDepthTexture, ID3D11DepthStencilView **pDepthView)
{
	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = inWidth;
	descDepth.Height = inHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	HRESULT hr = mDev->CreateTexture2D(&descDepth, nullptr, pDepthTexture);
	DbgAssert(hr == S_OK, "Problem Creating Depth Stencil");

	ID3D11DepthStencilView* toRet = nullptr;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = mDev->CreateDepthStencilView(*pDepthTexture, &descDSV, pDepthView);
	DbgAssert(hr == S_OK, "Problem Creating Depth Stencil");

	return true;
}

ID3D11DepthStencilState *Graphics::CreateDepthStencilState(bool inDepthTestEnable, D3D11_COMPARISON_FUNC inDepthComparisonFunction)
{
	D3D11_DEPTH_STENCIL_DESC dsDesc;

	// Depth test parameters
	dsDesc.DepthEnable = inDepthTestEnable;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = inDepthComparisonFunction;

	// Stencil test parameters
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	ID3D11DepthStencilState* toRet = nullptr;
	HRESULT hr = mDev->CreateDepthStencilState(&dsDesc, &toRet);
	DbgAssert(hr == S_OK, "Problem Creating Depth Stencil");

	return toRet;
}

void Graphics::SetActiveTexture(int slot, ID3D11ShaderResourceView* pView)
{
	mDevcon->PSSetShaderResources(slot, 1, &pView);
}

void Graphics::SetActiveSampler(int slot, ID3D11SamplerState* pSampler)
{
	mDevcon->PSSetSamplers(slot, 1, &pSampler);
}