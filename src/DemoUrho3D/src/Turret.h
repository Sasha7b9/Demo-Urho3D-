#pragma once

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource3D.h>

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
    void RunDetail(Node *node, const Vector3& deltaPos);

    ///
    void HandleShot(StringHash eventType, VariantMap& eventData);

    float dirOnTarget_ = 0.0f;
    float worldRotationDefault_ = 0.0f;
    float rotateGun_ = 0.0f;
    bool beaconEnabled_ = false;
    bool gunsEnabled_ = false;

    float timePrevShot_ = 0.0f;
    float rateOfFire_ = 15.0f;
    const float detectDistance = 20.0f;

    float health_ = 100.0f;

    Sound *sound_ = nullptr;

    SharedPtr<lSprite> sprite_;
    SharedPtr<Material> materialGUI_;
    Node *modelUInode_ = nullptr;
    Node *nodeLightL_ = nullptr;
    Node *nodeLightR_ = nullptr;
    Node *nodeBeacon_ = nullptr;
    Node *nodeFireR_ = nullptr;
    Node *nodeFireL_ = nullptr;
    Node *nodeBoneTower_ = nullptr;
    float distanceToJack = 0.0f;
    Node *nodeBoneGunR_ = nullptr;
    Node *nodeBoneGurL_ = nullptr;
    Node *nodeBoneGunsGradient_ = nullptr;
    Bone *boneGunR_ = nullptr;
    Bone *boneGunL_ = nullptr;

    SoundSource3D *soundSource_ = nullptr;

    bool isDead = false;

    static float timeUpdate;
    static float timeFromPrevBegin;
};
