#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/Graphics/ParticleEmitter.h>

#include "Turret.h"
#include "Bullet.h"

Turret::Turret(Context* context) :
    LogicComponent(context)
{
    SetUpdateEventMask(USE_UPDATE);
}

void Turret::RegisterObject(Context* context)
{
    context->RegisterFactory<Turret>();
}

void Turret::Start()
{
    Node *lightNode = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("Bone1")->node_->CreateChild("Beacon");
    Light *light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_POINT);
    light->SetCastShadows(false);
    light->SetRange(0.33f);
    light->SetBrightness(300.0f);
    light->SetColor(Color::RED);

    Vector3 position = {0.0f, 2.5f, -0.075f};

    lightNode->SetPosition(position);

    Node *lightShot = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("Bone1")->node_->CreateChild("LightL");
    light = lightShot->CreateComponent<Light>();
    light->SetLightType(LIGHT_POINT);
    light->SetCastShadows(false);
    light->SetRange(1.0f);
    light->SetBrightness(2.0f);

    position = {2.4f, 0.7f, -1.1f};
    lightShot->SetPosition(position);

    lightShot = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("Bone1")->node_->CreateChild("LightR");
    light = lightShot->CreateComponent<Light>();
    light->SetLightMask(LIGHT_POINT);
    light->SetCastShadows(false);
    light->SetRange(1.0f);
    light->SetBrightness(2.0f);

    position = {-2.4f, 0.7f, -1.1f};
    lightShot->SetPosition(position);

    /*
    ResourceCache *cache = GetSubsystem<ResourceCache>();

    Node *flameNode = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("Bone1")->node_->CreateChild("FlameL");
    flameNode->SetPosition({1.5f, 1.0f, -3.5f});
    flameNode->SetRotation(Quaternion(90.0f, Vector3::LEFT));
    ParticleEmitter *emitter = flameNode->CreateComponent<ParticleEmitter>();
    emitter->SetEffect(cache->GetResource<ParticleEffect>("Models/Turret/Particle/Fire.xml"));

    flameNode = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("Bone1")->node_->CreateChild("FlameR");
    flameNode->SetPosition({-1.5f, 1.0f, -3.5f});
    flameNode->SetRotation(Quaternion(90.0f, Vector3::LEFT));
    emitter = flameNode->CreateComponent<ParticleEmitter>();
    emitter->SetEffect(cache->GetResource<ParticleEffect>("Models/Turret/Particle/Fire.xml"));
    */

    /*
    Node *cubeNode = node_->GetComponent<AnimatedModdel>()->GetSkeleton().GetBone("Bone1")->node_->CreateChild("");
    StaticModel *cube = cubeNode->CreateComponent<StaticModel>();
    cube->SetModel(GetSubsystem<ResourceCache>()->GetResource<Model>("Models/Box.mdl"));
    cubeNode->SetScale(0.25f);
    cubeNode->SetPosition(position);
    */
}

void Turret::Update(float timeStep)
{
    float speed = 10.0f;

    float maxAngle = speed * timeStep;

    Node *nodeJack = GetScene()->GetChild("Jack");

    gunsEnabled = false;

    if (GetDistance(nodeJack) > detectDistance)
    {
        RotateToDefault(maxAngle);
        beaconEnabled = false;
    }
    else
    {
        RotateToTarget(nodeJack, maxAngle, timeStep);
        beaconEnabled = true;
    }

    UpdateLights();
}

void Turret::AnimateGun(Bone *bone, float timeStep)
{
    rotateGun += timeStep * 360.0f * 2.0f;
    bone->node_->SetRotation(Quaternion(90.0f, Vector3::RIGHT) * Quaternion(rotateGun, Vector3::UP));
}

void Turret::SetRotate(float angle)
{
    Skeleton& skeleton = node_->GetComponent<AnimatedModel>()->GetSkeleton();

    skeleton.GetBone("MasterBone")->node_->SetRotation(Quaternion(angle, Vector3::UP));
    worldRotationDefault = skeleton.GetBone("Bone1")->node_->GetWorldRotation().YawAngle();
}

void Turret::Logging()
{
    float angle = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("Bone1")->node_->GetWorldRotation().YawAngle();
    URHO3D_LOGINFOF("%f", angle);
}

void Turret::RotateToDefault(float maxAngle)
{
    float worldRotation = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("Bone1")->node_->GetWorldRotation().YawAngle();

    float angle = AngularDifference(worldRotationDefault, worldRotation);

    if (fabs(angle) > maxAngle)
    {
        if (angle < 0)
        {
            angle = worldRotation - maxAngle;
        }
        else
        {
            angle = worldRotation + maxAngle;
        }
    }
}

void Turret::RotateToTarget(Node *node, float maxAngle, float timeStep)
{
    Bone *bone = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("Bone1");

    Vector3 position = node_->GetPosition();
    Vector3 positionTarget = node->GetPosition();

    float angle = NormalizeAngle(Atan2(positionTarget.x_ - position.x_, positionTarget.z_ - position.z_) + 180.0f);

    float delta = NormalizeAngle(angle - bone->node_->GetWorldRotation().YawAngle());

    if (Abs(delta) < 25.0f)
    {
        Bone *bone1 = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("GunR");
        Bone *bone2 = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("GunL");

        AnimateGun(node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("GunR"), timeStep);
        AnimateGun(node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("GunL"), timeStep);

        float currentTime = GetSubsystem<Time>()->GetElapsedTime();

        if(currentTime >= timePrevShot + 1.0f / rateOfFire)
        {
            timePrevShot = currentTime;

            SharedPtr<Node> nodeBullet1(GetScene()->CreateChild("Bullet"));
            SharedPtr<Bullet> bullet1(nodeBullet1->CreateComponent<Bullet>());

            bullet1->Shot(bone1->node_->GetWorldPosition(), -bone1->node_->GetParent()->GetWorldDirection(), detectDistance);

            SharedPtr<Node> nodeBullet2(GetScene()->CreateChild("Bullet"));
            SharedPtr<Bullet> bullet2(nodeBullet2->CreateComponent<Bullet>());

            bullet2->Shot(bone2->node_->GetWorldPosition(), -bone2->node_->GetParent()->GetWorldDirection(), detectDistance);
        }

        gunsEnabled = true;

        if (Abs(delta) < 1.0f)
        {
            return;
        }
    }

    float sign = 1.0f;

    if (delta != 0.0f)
    {
        sign = delta / Abs(delta);
    }

    if (Abs(delta) > maxAngle)
    {
        delta = maxAngle;
    }

    bone->node_->SetWorldRotation(Quaternion(bone->node_->GetWorldRotation().YawAngle() + delta * sign, Vector3::UP));
}

float Turret::GetDistance(Node *node)
{
    Vector3 position = node_->GetPosition();
    Vector3 positionJack = node->GetPosition();

    float deltaZ = positionJack.z_ - position.z_;
    float deltaX = positionJack.x_ - position.x_;

    return sqrt(deltaX * deltaX + deltaZ * deltaZ);
}


float Turret::GetDirection(Node *node)
{
    Vector3 position = node_->GetPosition();
    Vector3 positionJack = node->GetPosition();

    float deltaZ = positionJack.z_ - position.z_;
    float deltaX = positionJack.x_ - position.x_;

    float tangens = deltaX / deltaZ;

    float angle = Atan(tangens);

    if (deltaZ > 0.0f)
    {
        angle += 180.0f;
    }

    return angle;
}

float Turret::AngularDifference(float angle1, float angle2)
{
    angle1 = ConvertAngle(angle1);
    angle2 = ConvertAngle(angle2);

    float diff = fabs(angle1 - angle2);

    if (diff > 180.0f)
    {
        diff = 360.0f - diff;
    }

    return diff * (angle1 < angle2 ? 1.0f : -1.0f);
}

float Turret::ConvertAngle(float angle)
{
    if (angle < 0.0f)
    {
        angle = 360 + angle;
    }
    return angle;
}
 
float Turret::NormalizeAngle(float angle)
{
    if (angle < -180.0f)
    {
        return angle + 360.0f;
    }
    if (angle > 180.0f)
    {
        return angle - 360.0f;
    }
    return angle;
}

void Turret::UpdateLights()
{
    Node *nodeLightL = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("Bone1")->node_->GetChild("LightL");
    Node *nodeLightR = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("Bone1")->node_->GetChild("LightR");

    float time = GetSubsystem<Time>()->GetElapsedTime();

    node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("Bone1")->node_->GetChild("Beacon")->SetEnabled(beaconEnabled);
    nodeLightL->SetEnabled(gunsEnabled);
    nodeLightR->SetEnabled(gunsEnabled);
    if (gunsEnabled)
    {
        //nodeLightL->GetComponent<Light>()->SetColor(Color(0.5f * Cos(time * 1e3) + 0.5f, 0.0f, 0.0f));
        //nodeLightR->GetComponent<Light>()->SetColor(Color(0.5f * Sin(time * 1e3) + 0.5f, 0.0f, 0.0f));
        nodeLightL->GetComponent<Light>()->SetColor(Color(Random(1.0f), 0.0f, 0.0f));
        nodeLightR->GetComponent<Light>()->SetColor(Color(Random(1.0f), 0.0f, 0.0f));
    }

    //node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("Bone1")->node_->GetChild("FlameL")->SetEnabled(gunsEnabled);
    //node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("Bone1")->node_->GetChild("FlameR")->SetEnabled(gunsEnabled);
}
