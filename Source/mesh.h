#pragma once

#include "engineMath.h"
#include "Graphics.h"

class Game;
class Shader;
class Texture;
class VertexBuffer;

class Mesh
{
public:
	Mesh(Game* pGame, const VertexBuffer* vertexBuffer, const Shader* pShader);
	~Mesh();

	void Draw() const;
	void SetTexture(int slot, const Texture* texture);

	bool Load(const WCHAR* fileName);
	static Mesh* StaticLoad(const WCHAR* fileName, Game* pGame);
    bool IsSkinned() const { return mIsSkinned; }

protected:
	Game *mGame;
	const Shader *mShader;
	const VertexBuffer *mVertexBuffer;
	const Texture *mTexture[Graphics::TEXTURE_SLOT_TOTAL];
    bool mIsSkinned;
};