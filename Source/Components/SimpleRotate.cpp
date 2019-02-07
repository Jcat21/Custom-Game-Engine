#include "stdafx.h"
#include "SimpleRotate.h"
#include "jsonUtil.h"
#include "renderObj.h"
#include "engineMath.h"

SimpleRotate::SimpleRotate(RenderObj * pObj)
	: Component(pObj)
	, mSpeed(0.0f)
{
}

SimpleRotate::~SimpleRotate()
{
}

void SimpleRotate::LoadProperties(const rapidjson::Value & properties)
{
	Component::LoadProperties(properties);
	GetFloatFromJSON(properties, "speed", mSpeed);
}

void SimpleRotate::Update(float deltaTime)
{
	mObj->mObjectData.modelToWorld *= Matrix4::CreateRotationZ(Math::ToRadians(mSpeed));
}
