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
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/RigidBody.h>


#include "Turret.h"
#include "Bullet.h"
#include "lSprite.h"
#include "GlobalVars.h"
#include "DeadObject.h"
#include "CustomLogic.h"

float Turret::timeUpdate = 0.0f;
float Turret::timeFromPrevBegin = 0.0f;

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

    nodeBoneTower_ = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("Bone1")->node_;

    nodeBeacon_ = nodeBoneTower_->CreateChild("Beacon");
    Light *light = nodeBeacon_->CreateComponent<Light>();
    light->SetLightType(LIGHT_POINT);
    light->SetCastShadows(false);
    light->SetRange(0.33f);
    light->SetBrightness(300.0f);
    light->SetColor(Color::RED);

    Vector3 position = {0.0f, 2.5f, -0.075f};

    nodeBeacon_->SetPosition(position);

    nodeLightL_ = nodeBoneTower_->CreateChild("LightL");
    light = nodeLightL_->CreateComponent<Light>();
    light->SetLightType(LIGHT_POINT);
    light->SetCastShadows(true);
    light->SetRange(0.5f);
    light->SetBrightness(10.0f);

    position = {2.4f, 0.7f, -1.1f};
    nodeLightL_->SetPosition(position);

    nodeLightR_ = nodeBoneTower_->CreateChild("LightR");
    light = nodeLightR_->CreateComponent<Light>();
    light->SetLightMask(LIGHT_POINT);
    light->SetCastShadows(true);
    light->SetRange(0.5f);
    light->SetBrightness(10.0f);

    position = {-2.4f, 0.7f, -1.1f};
    nodeLightR_->SetPosition(position);

    soundSource_ = node_->CreateComponent<SoundSource3D>();
    soundSource_->SetDistanceAttenuation(0.0f, detectDistance * 1.2f, 1.0f);
    soundSource_->SetSoundType(SOUND_EFFECT);

    ResourceCache *cache = gCache;

    sound_ = cache->GetResource<Sound>("Models/Turret/Turret.wav");
    sound_->SetLooped(true);

    boneGunR_ = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("GunR");
    nodeBoneGunR_ = boneGunR_->node_;

    nodeFireR_ = nodeBoneGunR_->CreateChild("Fire");
    float scale = 0.4f;
    nodeFireR_->SetScale(scale);
    nodeFireR_->SetPosition({0.0f, -3.5f, 0.0f});
    float angle = 90.0f;
    nodeFireR_->RotateAround(Vector3::ZERO, Quaternion(angle, Vector3::LEFT));
    StaticModel *fireModel = nodeFireR_->CreateComponent<StaticModel>();
    fireModel->SetModel(cache->GetResource<Model>("Models/Turret/Fire/Fire.mdl"));
    Material *material = cache->GetResource<Material>("Models/Turret/Fire/AutogunSplash.xml");
    Color color = material->GetShaderParameter("MatDiffColor").GetColor();
    color.a_ = 0.75f;
    material->SetShaderParameter("MatDiffColor", Variant(color));
    material->SetShadowCullMode(CULL_NONE);
    fireModel->SetMaterial(material);
    nodeFireR_->SetEnabled(false);

    light = nodeFireR_->CreateComponent<Light>();
    light->SetLightMask(LIGHT_POINT);
    light->SetCastShadows(true);
    light->SetRange(2.0f);
    light->SetBrightness(1.0f);

    boneGunL_ = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("GunL");
    nodeBoneGurL_ = boneGunL_->node_;

    nodeFireL_ = nodeBoneGurL_->CreateChild("Fire");
    nodeFireL_->SetScale(scale);
    nodeFireL_->SetPosition({0.0f, -3.5f, 0.0f});
    nodeFireL_->RotateAround(Vector3::ZERO, Quaternion(angle, Vector3::LEFT));
    fireModel = nodeFireL_->CreateComponent<StaticModel>();
    fireModel->SetModel(cache->GetResource<Model>("Models/Turret/Fire/Fire.mdl"));
    fireModel->SetMaterial(material);
    nodeFireL_->SetEnabled(false);

    light = nodeFireL_->CreateComponent<Light>();
    light->SetLightMask(LIGHT_POINT);
    light->SetCastShadows(true);
    light->SetRange(2.0f);
    light->SetBrightness(1.0f);
    light->SetColor(Color(1.0f, 1.0f, 0.5f));

    modelUInode_ = node_->CreateChild("UI");
    StaticModel *modelUI = modelUInode_->CreateComponent<StaticModel>();
    modelUI->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
    materialGUI_ = cache->GetResource<Material>("Models/Turret/GUI/GUI.xml")->Clone();
    modelUI->SetMaterial(materialGUI_);
    modelUInode_->SetPosition({0.0f, 4.0f, 0.0f});
    modelUInode_->SetScale({6.0f, 1.0f, 0.25f});
    modelUInode_->SetRotation(Quaternion(90.0f, Vector3::LEFT));
    modelUInode_->SetEnabled(false);
    
    materialGUI_->SetShadowCullMode(CULL_NONE);

    sprite_ = new lSprite(context_);
    sprite_->SetSize(200, 5);

    DrawHealth();



    nodeBoneGunsGradient_ = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("Bone2")->node_;
    nodeBoneGunsGradient_->SetRotation(Quaternion(0.5f, Vector3::RIGHT));
}

void Turret::DrawHealth()
{
    sprite_->Clear(Color::GRAY);
    sprite_->FillRectangle(0, 0, (int)(200.0f * health_ / 100.0f), 5, Color::RED);
    materialGUI_->SetTexture(TU_DIFFUSE, sprite_->GetTexture());
}

void Turret::Update(float timeStep)
{
    if(!isDead)
    {
        if((gTime->GetElapsedTime() - timeFromPrevBegin) >= 1.0f)
        {
            //URHO3D_LOGINFOF("Time update = %f", timeUpdate);
            timeUpdate = 0.0f;
            timeFromPrevBegin = gTime->GetElapsedTime();
        }

        //float timeEnter = gTime->GetElapsedTime();

        float speed = 10.0f;

        float maxAngle = speed * timeStep;

        if(health_ != 100.0f)
        {
            Vector3 posCamera = gCameraNode->GetPosition();
            Vector3 position = node_->GetPosition();
            float angle = NormalizeAngle(Atan2(posCamera.x_ - position.x_, posCamera.z_ - position.z_) + 180.0f);
            modelUInode_->SetRotation(Quaternion(angle, Vector3::UP) * Quaternion(90.0f, Vector3::LEFT));
        }

        gunsEnabled_ = false;

        distanceToJack = GetDistance(gNodeJack);

        if(distanceToJack > detectDistance)
        {
            //RotateToDefault(maxAngle);
            beaconEnabled_ = false;

            if(soundSource_->IsPlaying())
            {
                soundSource_->Stop();
            }
        }
        else
        {
            RotateToTarget(gNodeJack, maxAngle, timeStep);
            beaconEnabled_ = true;
            GradientToTarget(timeStep);
        }

        UpdateLights();

        //timeUpdate += (gTime->GetElapsedTime() - timeEnter);
    }
}

void Turret::GradientToTarget(float timeStep)
{
    static const float speedRotate = 10.0f;
    float maxAngle = speedRotate * timeStep;

    Vector3 positionJack = gNodeJack->GetPosition();

    Vector3 dirToTarget = positionJack - node_->GetPosition();
    dirToTarget.Normalize();
    float angleToTarget = Asin(dirToTarget.y_);

    Quaternion rotTurret = nodeBoneGunsGradient_->GetWorldRotation();

    float angleTurret = rotTurret.PitchAngle();

    float dAngle = angleToTarget - angleTurret;
    if (Abs(dAngle) > maxAngle)
    {
        dAngle = maxAngle * Sign(dAngle);
    }

    float angle = angleTurret + dAngle;

    nodeBoneGunsGradient_->SetRotation(Quaternion(angle, Vector3::RIGHT));
}

void Turret::AnimateGun(Bone *bone, float timeStep)
{
    rotateGun_ += timeStep * 360.0f * 2.0f;
    bone->node_->SetRotation(Quaternion(90.0f, Vector3::RIGHT) * Quaternion(rotateGun_, Vector3::UP));

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
    worldRotationDefault_ = skeleton.GetBone("Bone1")->node_->GetWorldRotation().YawAngle();
}

void Turret::Logging()
{
    float angle = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("Bone1")->node_->GetWorldRotation().YawAngle();
    URHO3D_LOGINFOF("%f", angle);
}

void Turret::RotateToDefault(float maxAngle)
{
    float worldRotation = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("Bone1")->node_->GetWorldRotation().YawAngle();

    float angle = AngularDifference(worldRotationDefault_, worldRotation);

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
    Vector3 position = node_->GetPosition();
    Vector3 positionTarget = node->GetPosition();

    float angle = NormalizeAngle(Atan2(positionTarget.x_ - position.x_, positionTarget.z_ - position.z_) + 180.0f);

    float delta = NormalizeAngle(angle - nodeBoneTower_->GetWorldRotation().YawAngle());

    if (Abs(delta) < (detectDistance - distanceToJack) / detectDistance * 45.0f)
    {
        AnimateGun(boneGunR_, timeStep);
        AnimateGun(boneGunL_, timeStep);

        if (!soundSource_->IsPlaying())
        {
            soundSource_->Play(sound_);
        }

        float currentTime = gTime->GetElapsedTime();

        if(currentTime >= timePrevShot_ + 1.0f / rateOfFire_)
        {
            timePrevShot_ = currentTime;

            SharedPtr<Node> nodeBullet1(GetScene()->CreateChild("Bullet"));
            SharedPtr<Bullet> bullet1(nodeBullet1->CreateComponent<Bullet>());

            bullet1->Shot(nodeBoneGurL_->GetWorldPosition(), -nodeBoneGurL_->GetParent()->GetWorldDirection(), detectDistance);

            SharedPtr<Node> nodeBullet2(GetScene()->CreateChild("Bullet"));
            SharedPtr<Bullet> bullet2(nodeBullet2->CreateComponent<Bullet>());

            bullet2->Shot(nodeBoneGunR_->GetWorldPosition(), -nodeBoneGunR_->GetParent()->GetWorldDirection(), detectDistance);
        }

        gunsEnabled_ = true;

        if (Abs(delta) < 1.0f)
        {
            return;
        }
    }
    else
    {
        if (soundSource_->IsPlaying())
        {
            soundSource_->Stop();
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

    nodeBoneTower_->SetWorldRotation(Quaternion(nodeBoneTower_->GetWorldRotation().YawAngle() + delta * sign, Vector3::UP));
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
    float time = gTime->GetElapsedTime();

    nodeBeacon_->SetEnabled(beaconEnabled_);
    nodeLightL_->SetEnabled(gunsEnabled_);
    nodeLightR_->SetEnabled(gunsEnabled_);
    if (gunsEnabled_)
    {
        nodeLightL_->GetComponent<Light>()->SetColor(Color(Random(1.0f), 0.0f, 0.0f));
        nodeLightR_->GetComponent<Light>()->SetColor(Color(Random(1.0f), 0.0f, 0.0f));
    }
    else
    {
        nodeFireL_->SetEnabled(false);
        nodeFireR_->SetEnabled(false);
    }
}

void Turret::HandleShot(StringHash eventType, VariantMap& eventData)
{
    if(!isDead)
    {
        if(health_ > 0.0f)
        {
            health_ -= 5.0f;
            DrawHealth();
            modelUInode_->SetEnabled(true);
        }
        else
        {
            modelUInode_->SetEnabled(false);
            
            RunDetail(nodeBoneTower_, {0.0f, 1.0f, 0.0f});

            RunDetail(nodeBoneGunsGradient_, {0.0f, 2.0f, 0.0f});

            RunDetail(nodeBoneGunR_, {1.0f, 1.0f, 1.0f});

            RunDetail(nodeBoneGurL_, {-1.0f, 1.0f, -1.0f});

            gunsEnabled_ = false;

            Update(0.0001f);

            soundSource_->Stop();

            isDead = true;

#ifdef DX11
            /*
            SharedPtr<Node> node(GetScene()->CreateChild("Boom"));
            StaticModel *model = node->CreateComponent<StaticModel>();
            model->SetModel(gCache->GetResource<Model>("Effects/Explosion/Models/Icosphere.mdl"));
            model->SetMaterial(gCache->GetResource<Material>("Effects/Explosion/Materials/BoomMaterial.xml"));
            node->CreateComponent<CustomLogic>();
            node->SetScale(10.0f);
            */
#endif
        }
    }
}

void Turret::RunDetail(Node *node, const Vector3& deltaPos)
{
    Vector3 scale = node->GetWorldScale();
    Vector3 position = node->GetWorldPosition();
    GetScene()->AddChild(node);

    node->SetWorldScale(scale);
    position += deltaPos;
    node->SetWorldPosition(position);

    RigidBody *body = node->CreateComponent<RigidBody>();
    body->SetCollisionLayer(2);
    body->SetMass(1.0f);
    CollisionShape *shape = node->CreateComponent<CollisionShape>();
    shape->SetBox(Vector3::ONE);

    body->SetLinearVelocity({Random(-5.0f, 5.0f), Random(3.0f, 5.0f), Random(-5.0f, 5.0f)});
    float angular = 5.0f;
    body->SetAngularVelocity(Vector3(Random(angular), Random(angular), Random(angular)));

    SharedPtr<DeadObject> deadObject(node->CreateComponent<DeadObject>());
}
