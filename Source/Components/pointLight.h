#pragma once
#include "component.h"

struct PointLightData;

class PointLight : public Component
{
public:
    PointLight(RenderObj *pObj);
    virtual ~PointLight() override;

    void LoadProperties(const rapidjson::Value& properties) override;

protected:
    PointLightData* mLight;
};