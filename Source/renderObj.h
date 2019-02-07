#pragma once

#include "engineMath.h"
#include "Graphics.h"

class Component;
class Game;
class Mesh;

class RenderObj
{
public:
	struct PerObjectConstants
	{
		Matrix4 modelToWorld;
	};

	RenderObj(Game* pGame, const Mesh* pMesh);
	virtual ~RenderObj();

	virtual void Draw();
    virtual void Update(float deltaTime);
    void AddComponent(Component *pComp) { mComponents.push_back(pComp); }

    Game* GetGame() { return mGame; }

	PerObjectConstants mObjectData;

protected:
	Game* mGame;
	const Mesh* mMesh;
	ID3D11Buffer* mObjectBuffer;
    std::vector<Component*> mComponents;
};