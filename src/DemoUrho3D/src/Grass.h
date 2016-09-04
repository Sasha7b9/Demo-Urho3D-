#pragma once

#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;

class ZoneGrass
{
public:
    ZoneGrass(Node *node, const Vector3& position, float size, float step);
    void Update();
private:
    void SwitchState();

    enum Distance
    {
        Distance_0,
        Distance_1,
        Distance_2,
        Distance_3,
        Distance_4,
        Distance_5,
        Distance_6,
        Distance_7,
        Distance_8,
        Distance_Size
    };

    Node* CreateHalfZone(Node *node);
    float CalculateAverageNormal(const Vector3& position, float size);

    PODVector<Node*> zones;
    Distance dist = Distance_0;
};

class Grass : public LogicComponent
{
    URHO3D_OBJECT(Grass, LogicComponent);

public:
    Grass(Context *context);

    static void RegisterObject(Context *context);

    virtual void Update(float timeStep);

private:
    PODVector<ZoneGrass*> zones;
};