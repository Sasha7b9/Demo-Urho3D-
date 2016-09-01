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

    if(body->GetLinearVelocity() == Vector3::ZERO && body->GetAngularVelocity() == Vector3::ZERO)
    {
        node_->Remove();
        //GetScene()->RemoveChild(node_);
    }
}
