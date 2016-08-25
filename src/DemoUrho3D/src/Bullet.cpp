#include <Urho3D/Core/Context.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Graphics/ParticleEffect.h>

#include "Bullet.h"


Bullet::Bullet(Context* context) :
    LogicComponent(context)
{
    SetUpdateEventMask(USE_UPDATE);
}

void Bullet::RegisterObject(Context* context)
{
    context->RegisterFactory<Bullet>();
}

void Bullet::Start()
{
    Node *nodeSmode = node_->CreateChild("Flame");
    ParticleEmitter *emitter = nodeSmode->CreateComponent<ParticleEmitter>();
    emitter->SetEffect(GetSubsystem<ResourceCache>()->GetResource<ParticleEffect>("Models/Turret/Particle/Fire.xml"));
    node_->SetEnabled(false);
}

void Bullet::Update(float timeStep)
{
    traveledDistance += absSpeed * timeStep;

    node_->SetPosition(node_->GetPosition() + timeStep * vectSpeed);

    if(traveledDistance >= distance_)
    {
        GetScene()->RemoveChild(node_);
    }
}

void Bullet::Shot(const Vector3& start, const Vector3& direction, float distance)
{
    distance_ = distance;
    node_->SetWorldPosition(start);
    vectSpeed = direction / direction.Length() * absSpeed;
    node_->SetEnabled(true);
}
