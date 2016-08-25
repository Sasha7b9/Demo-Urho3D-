#pragma once

#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;


class Turret : public LogicComponent
{
    URHO3D_OBJECT(Turret, LogicComponent);

public:
    /// Construct.
    Turret(Context* context);

    /// Register object factory and attributes.
    static void RegisterObject(Context* context);

    /// Handle startup. Called by LogicComponent base class.
    virtual void Start();

    /// Handle update.Called by LogicComponent base class.
    virtual void Update(float timeStep);

    ///
    void SetRotate(float angle);

private:
    ///
    void RotateToDefault(float maxAngle);

    ///
    void RotateToTarget(Node *node, float maxAngle, float timeStep);

    ///
    float GetDistance(Node *node);

    ///
    float GetDirection(Node *node);

    ///
    void Logging();

    ///
    float AngularDifference(float angle1, float angle2);

    ///
    float ConvertAngle(float angle);

    /// 
    float NormalizeAngle(float angle);

    ///
    void AnimateGun(Bone *bone, float timeStep);

    ///
    void UpdateBeacon();

    float dirOnTarget = 0.0f;
    float worldRotationDefault = 0.0f;
    float rotateGun = 0.0f;
    bool beaconEnabled = false;
};
