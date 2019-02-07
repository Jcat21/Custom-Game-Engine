#pragma once
#include "component.h"
#include "Physics.h"

class CollisionComponent : public Component
{
public:
	CollisionComponent(RenderObj* pObj);
	~CollisionComponent();
	void LoadProperties(const rapidjson::Value& properties) override;

	Physics::AABB GetAABB();
private:
	Physics::AABB mAABB;
};