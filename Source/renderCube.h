#pragma once
#include "renderObj.h"


class Shader;
class Texture;

class RenderCube : public RenderObj
{
public:
	RenderCube(Game *pGame, const Shader *pShader, const Texture *pTex);
	~RenderCube() override;
};