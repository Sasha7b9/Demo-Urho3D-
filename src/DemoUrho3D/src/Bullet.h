#pragma once

#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;


class Bullet : public LogicComponent
{
    URHO3D_OBJECT(Bullet, LogicComponent);

public:
    /// Construct.
    Bullet(Context* context);

    /// Register object factory and attributes.
    static void RegisterObject(Context* context);

    /// Handle startup. Called by LogicComponent base class.
    virtual void Start();

    /// Handle update. Called by LogicComponent base class.
    virtual void Update(float timeStep);

    ///
    void Shot(const Vector3& start, const Vector3& direction, float distance);

private:
    const float absSpeed = 10.0f;
    Vector3 vectSpeed = Vector3::ZERO;
    float traveledDistance = 0.0f;
    float distance_ = 0.0f;
};