#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Scene/Scene.h>

#include "Turret.h"

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

}

void Turret::Update(float timeStep)
{
    float speed = 20.0f;

    float maxAngle = speed * timeStep;

    Node *nodeJack = GetScene()->GetChild("Jack");

    if (GetDistance(nodeJack) > 50.0f)
    {
        RotateToDefault(maxAngle);
    }
    else
    {
        RotateToTarget(nodeJack, maxAngle);
    }
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

void Turret::RotateToTarget(Node *node, float maxAngle)
{
    Bone *bone = node_->GetComponent<AnimatedModel>()->GetSkeleton().GetBone("Bone1");

    float angle = GetDirection(node);

    float worldRotation = bone->node_->GetWorldRotation().YawAngle();

    float diff = AngularDifference(angle, worldRotation);

    if (fabs(diff) > maxAngle)
    {
        if (diff < 0)
        {
            angle = worldRotation - maxAngle;
        }
        else
        {
            angle = worldRotation - maxAngle;
        }
    }

    bone->node_->SetWorldRotation(Quaternion(angle, Vector3::UP));
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
