#pragma once
#include "animation.h"
#include "renderObj.h"

class SkinnedObj : public RenderObj
{
public:
	struct SkinningConstants
	{
		Matrix4 matrixPalette[Skeleton::MAX_SKELETON_BONES];
	};

	SkinnedObj(Game *pGame, const Mesh *mesh);
    virtual ~SkinnedObj();

	void Draw() override;

	SkinningConstants mSkinnedData;
    
protected:
	ID3D11Buffer* mSkinnedBuffer;
};