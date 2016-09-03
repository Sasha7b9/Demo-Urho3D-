#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>

#include "Grass.h"
#include "GlobalVars.h"

using namespace Urho3D;

Grass::Grass(Context *context) : 
    LogicComponent(context)
{
    float step = 0.25f;

    float timeStart = gTime->GetElapsedTime();

    float size = 500;

    float fullSize = size * step;

    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            float scale = Random(0.75f, 1.25f);

            Node* objectNode = gScene->CreateChild("Box");
            Vector3 position(-fullSize / 2 + i * step + Random(-step / 2, step / 2), 0.0f, -fullSize / 2 + j * step + Random(-step / 2, step / 2));
            position.y_ = gTerrain->GetHeight(position) + 0.5f * scale;
            objectNode->SetPosition(position);
            objectNode->SetRotation(Quaternion(Random(-180.0f, 180.0f), Vector3::UP) * Quaternion(90.0f, Vector3::LEFT));
            objectNode->SetScale(scale);
            //modelGroup->AddInstanceNode(objectNode);

            StaticModel* object = objectNode->CreateComponent<StaticModel>();
            object->SetModel(gCache->GetResource<Model>("Models/Plane.mdl"));
            object->SetMaterial(gCache->GetResource<Material>("Materials/Grass.xml"));
            object->SetCastShadows(false);

        }
    }
    URHO3D_LOGINFOF("time %f", gTime->GetElapsedTime() - timeStart);
}

void Grass::RegisterObject(Context *context)
{
    context->RegisterFactory<Grass>();
}
