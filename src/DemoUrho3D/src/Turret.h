#pragma once

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Audio/Sound.h>

using namespace Urho3D;

class lSprite;

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
    void UpdateLights();

    ///
    void DrawHealth();

    ///
    void GradientToTarget(float timeStep);

    ///
    void HandleShot(StringHash eventType, VariantMap& eventData);

    float dirOnTarget = 0.0f;
    float worldRotationDefault = 0.0f;
    float rotateGun = 0.0f;
    bool beaconEnabled = false;
    bool gunsEnabled = false;

    float timePrevShot = 0.0f;
    float rateOfFire = 15.0f;
    const float detectDistance = 20.0f;

    float health_ = 100.0f;

    Sound *sound = nullptr;

    SharedPtr<lSprite> sprite = nullptr;
    SharedPtr<Material> materialGUI = nullptr;
    Node *modelUInode = nullptr;
};
