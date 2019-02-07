#pragma once

#include "component.h"

class SimpleRotate : public Component
{
public:
	SimpleRotate(RenderObj* pObj);
	~SimpleRotate();

	void LoadProperties(const rapidjson::Value& properties) override;
	void Update(float deltaTime) override;
private:
	float mSpeed;
};