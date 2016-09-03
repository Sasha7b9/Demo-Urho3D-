#pragma once

#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;

class Grass : public LogicComponent
{
    URHO3D_OBJECT(Grass, LogicComponent);

public:
    Grass(Context *context);

    static void RegisterObject(Context *context);
};