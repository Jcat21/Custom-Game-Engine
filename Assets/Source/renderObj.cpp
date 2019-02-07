#include "stdafx.h"
#include "renderObj.h"
#include "component.h"
#include "game.h"
#include "mesh.h"
#include "Shader.h"
#include "texture.h"
#include "vertexBuffer.h"


RenderObj::RenderObj(Game* pGame, const Mesh* pMesh)
	: mGame(pGame)
	, mMesh(pMesh)
{
	mObjectBuffer = mGame->GetGraphics()->CreateGraphicsBuffer(&mObjectData, sizeof(mObjectData), D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
	mObjectData.modelToWorld = Matrix4::CreateRotationZ(Math::ToDegrees(45.0f));
}

RenderObj::~RenderObj()
{
	mObjectBuffer->Release();
}

void RenderObj::Draw()
{
	if (nullptr != mMesh)
	{
		mGame->GetGraphics()->UploadBuffer(mObjectBuffer, &mObjectData, sizeof(mObjectData));
		mGame->GetGraphics()->GetDeviceContext()->VSSetConstantBuffers(Graphics::CONSTANT_BUFFER_RENDEROBJ, 1, &mObjectBuffer);
		mMesh->Draw();
	}
}

void RenderObj::Update(float deltaTime)
{
    for (auto comp : mComponents)
        comp->Update(deltaTime);
}