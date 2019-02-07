#include "stdafx.h"
#include "texture.h"
#include "game.h"
#include "Graphics.h"
#include "DirectXTK\Inc\DDSTextureLoader.h"
#include "DirectXTK\Inc\WICTextureLoader.h"
#include <string>

Texture::Texture(Graphics *graphics)
    : mGraphics(graphics)
    , mResource(nullptr)
    , mView(nullptr)
    , mWidth(0)
    , mHeight(0)
{
}

Texture::~Texture()
{
    Free();
}

void Texture::Free()
{
    if (nullptr != mView)
    {
        mView->Release();
        mView = nullptr;
    }
    if (nullptr != mResource)
    {
        mResource->Release();
        mResource = nullptr;
    }
    mWidth = 0;
    mHeight = 0;
}

bool Texture::Load(const WCHAR* fileName)
{
    Free();     // in case there was already a texture loaded here, release it

    ID3D11Device *pDev = mGraphics->GetDevice();

    std::wstring fileStr(fileName);
    std::wstring extension = fileStr.substr(fileStr.find_last_of('.'));
    HRESULT hr = -1;
    if (extension == L".dds" || extension == L".DDS")
        hr = DirectX::CreateDDSTextureFromFile(pDev, fileName, &mResource, &mView);
    else
        hr = DirectX::CreateWICTextureFromFile(pDev, fileName, &mResource, &mView);
    DbgAssert(hr == S_OK, "Problem Creating Texture From File");
    if (S_OK != hr)
        return false;

    CD3D11_TEXTURE2D_DESC textureDesc;
    ((ID3D11Texture2D*)mResource)->GetDesc(&textureDesc);
    mWidth = textureDesc.Width;
    mHeight = textureDesc.Height;

    return true;
}

#if 1	// TODO Lab 04c
Texture* Texture::StaticLoad(const WCHAR* fileName, Game* pGame)
{
    Texture* pTex = new Texture(pGame->GetGraphics());
    if (false == pTex->Load(fileName))
    {
        delete pTex;
        return nullptr;
    }
    return pTex;
}
#endif

#if 1	// TODO Lab 03k
void Texture::SetActive(int slot) const
{
	mGraphics->SetActiveTexture(slot, mView);
}
#endif