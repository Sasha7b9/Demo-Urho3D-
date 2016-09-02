#include <Urho3D/Core/Context.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>

#include "DeadObject.h"

DeadObject::DeadObject(Context *context) :
    LogicComponent(context)
{

}

void DeadObject::RegisterObject(Context *context)
{
    context->RegisterFactory<DeadObject>();
}

void DeadObject::Update(float timeStep)
{
    RigidBody *body = node_->GetComponent<RigidBody>();

    if(body->GetLinearVelocity().Length() < 0.1f && body->GetAngularVelocity().Length() < 0.1f)
    {
        Vector3 position = node_->GetPosition();
        position.y_ = -1000.0f;
        node_->SetPosition(position);
        node_->SetEnabled(false);
    }
}
