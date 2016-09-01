#pragma once


#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;


class DeadObject : public LogicComponent
{
    URHO3D_OBJECT(DeadObject, LogicComponent);

public:
    DeadObject(Context *context);

    static void RegisterObject(Context *context);

    virtual void Update(float timeStep);
};
