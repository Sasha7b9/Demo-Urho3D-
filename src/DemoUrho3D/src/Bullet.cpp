#include <Urho3D/Core/Context.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/IO/Log.h>

#include "Bullet.h"

float Bullet::timeStartCalc = 0.0f;
float Bullet::timeForBuild = 0.0f;

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
    if (GetSubsystem<Time>()->GetElapsedTime() > timeShot + 1.0f)
    {
        GetScene()->RemoveChild(node_);
    }
}

void Bullet::Shot(const Vector3& start, const Vector3& direction, float distance)
{
    if (timeStartCalc + 1.0f <= GetSubsystem<Time>()->GetElapsedTime())
    {
        timeStartCalc = GetSubsystem<Time>()->GetElapsedTime();
        //URHO3D_LOGINFOF("Time build %f in sec", timeForBuild);
        timeForBuild = 0.0f;
    }

    float timeEnter = GetSubsystem<Time>()->GetElapsedTime();

    float absStep = 1.0f;

    Vector3 step = direction / direction.Length() * absStep;
    node_->SetEnabled(true);

    Vector3 position = start + Random(absStep) * step;
    float traveledDistance = 0.0f;

    node_->SetWorldPosition(start);

    Node *nodeSmoke = node_->CreateChild("Smoke");
    ParticleEmitter *emitter = nodeSmoke->CreateComponent<ParticleEmitter>();
    emitter->SetEffect(GetSubsystem<ResourceCache>()->GetResource<ParticleEffect>("Models/Turret/Particle/Fire.xml"));

    while (traveledDistance <= distance)
    {
        /*
        Node *nodeSmoke = node_->CreateChild("Smoke");
        ParticleEmitter *emitter = nodeSmoke->CreateComponent<ParticleEmitter>();
        emitter->SetEffect(GetSubsystem<ResourceCache>()->GetResource<ParticleEffect>("Models/Turret/Particle/Fire.xml"));
        nodeSmoke->SetWorldPosition(position);
        */

        Node *node = node_->CreateChild("Smoke");
        node->CloneComponent(emitter);
        node->SetWorldPosition(position);

        position += step;
        traveledDistance += absStep;
        nodeSmoke->SetEnabled(true);
    }

    //URHO3D_LOGINFOF("time %f", GetSubsystem<Time>()->GetElapsedTime() - timeEnter);

    timeShot = GetSubsystem<Time>()->GetElapsedTime();

    timeForBuild += GetSubsystem<Time>()->GetElapsedTime() - timeEnter;
}
