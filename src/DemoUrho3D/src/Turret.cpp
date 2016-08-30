#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource3D.h>
#include <Urho3D/Audio/SoundStream.h>
#include <Urho3D/Graphics/Material.h>


#include "Turret.h"
#include "Bullet.h"
#include "lSprite.h"

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
    SubscribeToEvent(GetNode(), E_SHOT, URHO3D_HANDLER(Turret, HandleShot));

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
    light->SetCastShadows(true);
    light->SetRange(0.5f);
    light->SetBrightness(10.0f);

    position = {2.4f, 0.7f, -1.1f};
    lightShot->SetPosition(position);

    lightShot = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("Bone1")->node_->CreateChild("LightR");
    light = lightShot->CreateComponent<Light>();
    light->SetLightMask(LIGHT_POINT);
    light->SetCastShadows(true);
    light->SetRange(0.5f);
    light->SetBrightness(10.0f);

    position = {-2.4f, 0.7f, -1.1f};
    lightShot->SetPosition(position);

    SoundSource3D* soundSource = node_->CreateComponent<SoundSource3D>();
    soundSource->SetDistanceAttenuation(0.0f, detectDistance * 1.2f, 1.0f);
    soundSource->SetSoundType(SOUND_EFFECT);

    ResourceCache *cache = GetSubsystem<ResourceCache>();

    sound = cache->GetResource<Sound>("Models/Turret/Turret.wav");
    sound->SetLooped(true);

    Node *fireNode = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("GunR")->node_->CreateChild("Fire");
    float scale = 0.4f;
    fireNode->SetScale(scale);
    fireNode->SetPosition({0.0f, -3.5f, 0.0f});
    float angle = 90.0f;
    fireNode->RotateAround(Vector3::ZERO, Quaternion(angle, Vector3::LEFT));
    StaticModel *fireModel = fireNode->CreateComponent<StaticModel>();
    fireModel->SetModel(cache->GetResource<Model>("Models/Turret/Fire/Fire.mdl"));
    Material *material = cache->GetResource<Material>("Models/Turret/Fire/AutogunSplash.xml");
    Color color = material->GetShaderParameter("MatDiffColor").GetColor();
    color.a_ = 0.75f;
    material->SetShaderParameter("MatDiffColor", Variant(color));
    material->SetShadowCullMode(CULL_NONE);
    fireModel->SetMaterial(material);
    fireNode->SetEnabled(false);

    light = fireNode->CreateComponent<Light>();
    light->SetLightMask(LIGHT_POINT);
    light->SetCastShadows(true);
    light->SetRange(2.0f);
    light->SetBrightness(1.0f);

    fireNode = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("GunL")->node_->CreateChild("Fire");
    fireNode->SetScale(scale);
    fireNode->SetPosition({0.0f, -3.5f, 0.0f});
    fireNode->RotateAround(Vector3::ZERO, Quaternion(angle, Vector3::LEFT));
    fireModel = fireNode->CreateComponent<StaticModel>();
    fireModel->SetModel(cache->GetResource<Model>("Models/Turret/Fire/Fire.mdl"));
    fireModel->SetMaterial(material);
    fireNode->SetEnabled(false);

    light = fireNode->CreateComponent<Light>();
    light->SetLightMask(LIGHT_POINT);
    light->SetCastShadows(true);
    light->SetRange(2.0f);
    light->SetBrightness(1.0f);
    light->SetColor(Color(1.0f, 1.0f, 0.5f));

    modelUInode = node_->CreateChild("UI");
    StaticModel *modelUI = modelUInode->CreateComponent<StaticModel>();
    modelUI->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
    materialGUI = cache->GetResource<Material>("Models/Turret/GUI/GUI.xml")->Clone();
    modelUI->SetMaterial(materialGUI);
    modelUInode->SetPosition({0.0f, 4.0f, 0.0f});
    modelUInode->SetScale({6.0f, 1.0f, 0.25f});
    modelUInode->SetRotation(Quaternion(90.0f, Vector3::LEFT));
    
    materialGUI->SetShadowCullMode(CULL_NONE);

    sprite = new lSprite(context_);
    sprite->SetSize(200, 5);

    DrawHealth();

    Bone *bone = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("Bone2");
    bone->node_->SetRotation(Quaternion(0.5f, Vector3::RIGHT));
}

void Turret::DrawHealth()
{
    sprite->Clear(Color::GRAY);
    sprite->FillRectangle(0, 0, (int)(200.0f * health_ / 100.0f), 5, Color::RED);


    materialGUI->SetTexture(TU_DIFFUSE, sprite->GetTexture());
}

void Turret::Update(float timeStep)
{
    float speed = 10.0f;

    float maxAngle = speed * timeStep;

    Node *nodeJack = GetScene()->GetChild("Jack");

    float angle = NormalizeAngle(Atan2(GetScene()->GetChild("CameraNode")->GetPosition().x_ - node_->GetPosition().x_, GetScene()->GetChild("CameraNode")->GetPosition().z_ - node_->GetPosition().z_) + 180.0f);

    modelUInode->SetRotation(Quaternion(angle, Vector3::UP) * Quaternion(90.0f, Vector3::LEFT));

    gunsEnabled = false;

    if (GetDistance(nodeJack) > detectDistance)
    {
        RotateToDefault(maxAngle);
        beaconEnabled = false;

        SoundSource3D *soundSource = node_->GetComponent<SoundSource3D>();
        if (soundSource->IsPlaying())
        {
            soundSource->Stop();
        }
    }
    else
    {
        RotateToTarget(nodeJack, maxAngle, timeStep);
        beaconEnabled = true;
        GradientToTarget(timeStep);
    }

    UpdateLights();
}

void Turret::GradientToTarget(float timeStep)
{
    static const float speedRotate = 100.0f;
    float maxAngle = speedRotate * timeStep;

    Node *nodeJack = GetScene()->GetChild("Jack");
    Vector3 positionJack = nodeJack->GetPosition();

    Vector3 dirToTarget = positionJack - node_->GetPosition();
    dirToTarget.Normalize();
    float angleToTarget = Asin(dirToTarget.y_);

    Bone *bone = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("Bone2");

    Vector3 dirTurret = bone->node_->GetWorldDirection();
    float angleTurret = Asin(dirTurret.y_);

    float dAngle = angleToTarget - angleTurret;
    if (Abs(dAngle) > maxAngle)
    {
        dAngle = maxAngle * Sign(dAngle);
    }

    float angle = angleTurret + dAngle;

    bone->node_->SetRotation(Quaternion(angle, Vector3::RIGHT));
}

void Turret::AnimateGun(Bone *bone, float timeStep)
{
    rotateGun += timeStep * 360.0f * 2.0f;
    bone->node_->SetRotation(Quaternion(90.0f, Vector3::RIGHT) * Quaternion(rotateGun, Vector3::UP));

    Node *nodeFire = bone->node_->GetChild("Fire");

    nodeFire->SetEnabled(true);

    nodeFire->SetScale(Random(0.2f, 0.4f));
    nodeFire->RotateAround(Vector3::ZERO, Quaternion(Random(-180.0f, 180.0f), Vector3::BACK));

    Light *light = nodeFire->GetComponent<Light>();
    light->SetRange(Random(1.0f, 2.0f));
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

    if (Abs(delta) < (detectDistance - GetDistance(GetScene()->GetChild("Jack"))) / detectDistance * 45.0f)
    {
        Bone *bone1 = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("GunR");
        Bone *bone2 = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("GunL");

        AnimateGun(node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("GunR"), timeStep);
        AnimateGun(node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("GunL"), timeStep);

        SoundSource3D *soundSource = node_->GetComponent<SoundSource3D>();
        if (!soundSource->IsPlaying())
        {
            soundSource->Play(sound);
        }

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
    else
    {
        SoundSource3D *soundSource = node_->GetComponent<SoundSource3D>();
        if (soundSource->IsPlaying())
        {
            soundSource->Stop();
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
    else
    {
        node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("GunL")->node_->GetChild("Fire")->SetEnabled(false);
        node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("GunR")->node_->GetChild("Fire")->SetEnabled(false);
    }

    //node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("Bone1")->node_->GetChild("FlameL")->SetEnabled(gunsEnabled);
    //node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("Bone1")->node_->GetChild("FlameR")->SetEnabled(gunsEnabled);
}

void Turret::HandleShot(StringHash eventType, VariantMap& eventData)
{
    using namespace Shot;

    //Node *node = static_cast<Node*>(eventData[P_NODE].GetPtr());

    //if(node == node_)
    {
        health_ -= 0.5f;
        DrawHealth();
    }
}
