#include <Urho3D/Core/Context.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>

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
}

void Bullet::Update(float timeStep)
{
    float currentTime = GetSubsystem<Time>()->GetElapsedTime();

    if (currentTime > timeShot + timeLive)
    {
        GetScene()->RemoveChild(node_);
    }
    else
    {
        Color color = material->GetShaderParameter("MatDiffColor").GetColor();

        color.a_ = ((timeShot + timeLive) - currentTime) / timeLive * 0.2f;

        material->SetShaderParameter("MatDiffColor", Variant(color));
    }
}

void Bullet::Shot(const Vector3& start, const Vector3& direction, float distance)
{
    timeLive = Random(0.1f) + 0.1f;

    ResourceCache* cache = GetSubsystem<ResourceCache>();

    material = cache->GetResource<Material>("Models/Turret/Bullet/Bullet.xml");

    float scale = 0.2f;

    Node* node1 = node_->CreateChild();
    StaticModel *model = node1->CreateComponent<StaticModel>();
    model->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
    model->SetMaterial(material);

    Node* node11 = node1->CreateChild();
    model = node11->CreateComponent<StaticModel>();
    model->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
    model->SetMaterial(material);
    node11->RotateAround(Vector3::ZERO, Quaternion(180.0f, Vector3::BACK));

    float maxDeltaShift = 0.3f;

    Vector3 position = start + direction / direction.Length() * distance / 2 + Vector3(Random(maxDeltaShift) - maxDeltaShift / 2.0f, Random(maxDeltaShift) - maxDeltaShift / 2.0f, Random(maxDeltaShift) - maxDeltaShift / 2.0f);

    node1->SetScale({scale, 1.0f, distance});
    node1->SetWorldPosition(position);
    node1->SetWorldDirection(direction);

    Node *node2 = node_->CreateChild();
    model = node2->CreateComponent<StaticModel>();
    model->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
    model->SetMaterial(material);
    node2->RotateAround({0.0f, 0.0f, 0.0f}, Quaternion(90.0f, Vector3::UP));
    node2->SetScale({scale, 1.0f, distance});
    node2->SetWorldPosition(position);
    node2->SetWorldDirection(direction);
    node2->RotateAround(Vector3::ZERO, Quaternion(90.0f, Vector3::BACK));

    Node* node21 = node2->CreateChild();
    model = node21->CreateComponent<StaticModel>();
    model->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
    model->SetMaterial(material);
    node21->RotateAround(Vector3::ZERO, Quaternion(180.0f, Vector3::BACK));

    timeShot = GetSubsystem<Time>()->GetElapsedTime();
}
