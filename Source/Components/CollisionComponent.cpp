#include "stdafx.h"
#include "CollisionComponent.h"
#include "jsonUtil.h"
#include "renderObj.h"
#include "game.h"

CollisionComponent::CollisionComponent(RenderObj* pObj)
	: Component(pObj)
{
}

CollisionComponent::~CollisionComponent()
{
	mObj->GetGame()->mPhysics.RemoveObj(this);
}

void CollisionComponent::LoadProperties(const rapidjson::Value & properties)
{
	Component::LoadProperties(properties);
	GetVectorFromJSON(properties, "min", mAABB.mMin);
	GetVectorFromJSON(properties, "max", mAABB.mMax);
}

Physics::AABB CollisionComponent::GetAABB()
{
	Physics::AABB temp = mAABB;

	temp.mMin *= mObj->mObjectData.modelToWorld.GetScale().x;
	temp.mMax *= mObj->mObjectData.modelToWorld.GetScale().x;

	temp.mMin.x += mObj->mObjectData.modelToWorld.GetTranslation().x;
	temp.mMin.y += mObj->mObjectData.modelToWorld.GetTranslation().y;
	temp.mMin.z += mObj->mObjectData.modelToWorld.GetTranslation().z;
	temp.mMax.x += mObj->mObjectData.modelToWorld.GetTranslation().x;
	temp.mMax.y += mObj->mObjectData.modelToWorld.GetTranslation().y;
	temp.mMax.z += mObj->mObjectData.modelToWorld.GetTranslation().z;

	return temp;
}
