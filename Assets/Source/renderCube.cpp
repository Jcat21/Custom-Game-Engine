#include "stdafx.h"
#include "renderCube.h"
#include "game.h"
#include "Graphics.h"
#include "mesh.h"
#include "vertexBuffer.h"
#include "Shader.h"

struct VertexPosNormColorUV
{
	Vector3 pos;
	Vector3 norm;
	Graphics::Color4 color;
	Vector2 uv;
};

static VertexPosNormColorUV cubeVertex[] =
{
	{ Vector3(-0.5f, 0.5f, -0.5f),	Vector3(0.0f, 0.0f, -1.0f),	Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f) },
	{ Vector3(0.5f, 0.5f, -0.5f),	Vector3(0.0f, 0.0f, -1.0f),	Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f) },
	{ Vector3(0.5f, -0.5f, -0.5f),	Vector3(0.0f, 0.0f, -1.0f),	Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f) },
	{ Vector3(-0.5f, -0.5f, -0.5f),	Vector3(0.0f, 0.0f, -1.0f),	Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f), Vector2(0.0f, 0.0f) },

	{ Vector3(0.5f, 0.5f, -0.5f),	Vector3(1.0f, 0.0f, 0.0f),	Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f) },
	{ Vector3(0.5f, 0.5f, 0.5f),	Vector3(1.0f, 0.0f, 0.0f),	Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f) },
	{ Vector3(0.5f, -0.5f, 0.5f),	Vector3(1.0f, 0.0f, 0.0f),	Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f) },
	{ Vector3(0.5f, -0.5f, -0.5f),	Vector3(1.0f, 0.0f, 0.0f),	Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f), Vector2(0.0f, 0.0f) },

	{ Vector3(-0.5f, -0.5f, 0.5f),	Vector3(0.0f, 0.0f, 1.0f),	Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f) },
	{ Vector3(0.5f, -0.5f, 0.5f),	Vector3(0.0f, 0.0f, 1.0f),	Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f) },
	{ Vector3(0.5f, 0.5f, 0.5f),	Vector3(0.0f, 0.0f, 1.0f),	Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f) },
	{ Vector3(-0.5f, 0.5f, 0.5f),	Vector3(0.0f, 0.0f, 1.0f),	Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f) },

	{ Vector3(-0.5f, -0.5f, -0.5f),	Vector3(-1.0f, 0.0f, 0.0f),	Graphics::Color4(1.0f, 1.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f) },
	{ Vector3(-0.5f, -0.5f, 0.5f),	Vector3(-1.0f, 0.0f, 0.0f),	Graphics::Color4(1.0f, 1.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f) },
	{ Vector3(-0.5f, 0.5f, 0.5f),	Vector3(-1.0f, 0.0f, 0.0f),	Graphics::Color4(1.0f, 1.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f) },
	{ Vector3(-0.5f, 0.5f, -0.5f),	Vector3(-1.0f, 0.0f, 0.0f),	Graphics::Color4(1.0f, 1.0f, 0.0f, 1.0f), Vector2(0.0f, 0.0f) },

	{ Vector3(-0.5f, -0.5f, -0.5f),	Vector3(0.0f, -1.0f, 0.0f),	Graphics::Color4(0.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f) },
	{ Vector3(0.5f, -0.5f, -0.5f),	Vector3(0.0f, -1.0f, 0.0f),	Graphics::Color4(0.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f) },
	{ Vector3(0.5f, -0.5f, 0.5f),	Vector3(0.0f, -1.0f, 0.0f),	Graphics::Color4(0.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f) },
	{ Vector3(-0.5f, -0.5f, 0.5f),	Vector3(0.0f, -1.0f, 0.0f),	Graphics::Color4(0.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f) },

	{ Vector3(-0.5f, 0.5f, 0.5f),	Vector3(0.0f, 1.0f, 0.0f),	Graphics::Color4(1.0f, 0.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f) },
	{ Vector3(0.5f, 0.5f, 0.5f),	Vector3(0.0f, 1.0f, 0.0f),	Graphics::Color4(1.0f, 0.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f) },
	{ Vector3(0.5f, 0.5f, -0.5f),	Vector3(0.0f, 1.0f, 0.0f),	Graphics::Color4(1.0f, 0.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f) },
	{ Vector3(-0.5f, 0.5f, -0.5f),	Vector3(0.0f, 1.0f, 0.0f),	Graphics::Color4(1.0f, 0.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f) },
};
static uint16_t cubeIndex[] =
{
	2, 1, 0,
	3, 2, 0,

	6, 5, 4,
	7, 6, 4,

	10, 9, 8,
	11, 10, 8,

	14, 13, 12,
	15, 14, 12,

	18, 17, 16,
	19, 18, 16,

	22, 21, 20,
	23, 22, 20,
};

RenderCube::RenderCube(Game *pGame, const Shader *pShader, const Texture *pTex)
    : RenderObj(pGame, nullptr)
{
	VertexBuffer *pVertex = new VertexBuffer(mGame->GetGraphics(),
		cubeVertex, sizeof(cubeVertex) / sizeof(cubeVertex[0]), sizeof(cubeVertex[0]),
		cubeIndex, sizeof(cubeIndex) / sizeof(cubeIndex[0]), sizeof(cubeIndex[0])
		);
	Mesh* pMesh = new Mesh(pGame, pVertex, pShader);
	pMesh->SetTexture(Graphics::TEXTURE_SLOT_DIFFUSE, pTex);
	mMesh = pMesh;
}

RenderCube::~RenderCube()
{
	delete mMesh;
}
