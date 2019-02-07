#include "stdafx.h"
#include "skinnedObj.h"
#include "game.h"


SkinnedObj::SkinnedObj(Game *pGame, const Mesh *mesh)
    : RenderObj(pGame, mesh)
{
	for (int i = 0; i < Skeleton::MAX_SKELETON_BONES; ++i)
		mSkinnedData.matrixPalette[i] = Matrix4::Identity;
	mSkinnedBuffer = mGame->GetGraphics()->CreateGraphicsBuffer(&mSkinnedData, sizeof(mSkinnedData), D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

SkinnedObj::~SkinnedObj()
{
	mSkinnedBuffer->Release();
}

void SkinnedObj::Draw()
{
    mGame->GetGraphics()->UploadBuffer(mSkinnedBuffer, &mSkinnedData, sizeof(mSkinnedData));
    mGame->GetGraphics()->GetDeviceContext()->VSSetConstantBuffers(Graphics::CONSTANT_BUFFER_SKINNING, 1, &mSkinnedBuffer);
	RenderObj::Draw();
}

