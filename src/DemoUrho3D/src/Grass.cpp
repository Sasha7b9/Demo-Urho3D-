#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/StaticModelGroup.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>

#include "Grass.h"
#include "GlobalVars.h"

using namespace Urho3D;

Grass::Grass(Context *context) : 
    LogicComponent(context)
{
    float timeStart = gTime->GetElapsedTime();

    float fullSize = 100.0f;
    float step = 1.5f;

    int sizeBlock = 10;
    int numZones = (int)fullSize / sizeBlock;

    float startX = -fullSize / 2.0f + step / 2.0f;
    float startZ = startX;

    Node *node = gScene->CreateChild("Grass");

    for(int i = 0; i < numZones; i++)
    {
        for(int j = 0; j < numZones; j++)
        {
            ZoneGrass *zone = new ZoneGrass(node, {startX + i * (fullSize / numZones), 0.0f, startZ + j * (fullSize / numZones)}, fullSize / numZones, step);
            zones.Push(zone);
        }
    }

    URHO3D_LOGINFOF("time %f", gTime->GetElapsedTime() - timeStart);
}

void Grass::RegisterObject(Context *context)
{
    context->RegisterFactory<Grass>();
}

void Grass::Update(float timeStep)
{
    for(ZoneGrass* zone : zones)
    {
        zone->Update();
    }
}

ZoneGrass::ZoneGrass(Node *node_, const Vector3& position, float size, float step)
{
    float minX = -size / 2.0f + step / 2.0f;
    float minZ = -size / 2.0f + step / 2.0f;

    int numGrass = (int)(size / step);

    int allGrass = (int)(numGrass * numGrass * (CalculateAverageNormal(position, size) + 1.0f) * 2.0f);

    Vector<Vector<int>> blocks;
    blocks.Resize(Distance_Size);

    for(int i = 0; i < allGrass; i++)
    {
        blocks[0].Push(i);
    }
    
    for(int i = 0; i < allGrass; i++)
    {
        int index1 = Random(allGrass);
        int index2 = Random(allGrass);
        int temp = blocks[0][index1];
        blocks[0][index1] = blocks[0][index2];
        blocks[0][index2] = temp;
    }

    float k = 1.25f;

    Vector<int> positions;
    positions.Resize(allGrass);
    for(int i = 0; i < allGrass; i++)
    {
        positions[i] = 0;
    }

    for(int i = 1; i < Distance_Size; i++)
    {
        int num = (int)(blocks[i - 1].Size() / k);

        for(int j = 0; j < num; j++)
        {
            if(blocks[0].Size() == 0)
            {
                break;
            }
            int value = blocks[i - 1].Back();
            blocks[i].Push(value);
            blocks[i - 1].Pop();
            positions[value] = i;
        }
    }

    for(int i = 0; i < Distance_Size; i++)
    {
        Node* nodeGroup = gScene->CreateChild("GrassZone");
        StaticModelGroup* group = nodeGroup->CreateComponent<StaticModelGroup>();
        group->SetModel(gCache->GetResource<Model>("Models/Plane.mdl"));
        group->SetMaterial(gCache->GetResource<Material>("Materials/Grass.xml"));
        group->SetCastShadows(false);

        zones.Push(nodeGroup);
        zones[i]->SetPosition(position);
    }

    int numBlock = 0;

    for(int i = 0; i < allGrass; i++)
    {
        float scale = Random(0.75f, 1.25f);

        Node *nodeZone = zones[positions[numBlock++]];

        Node* objectNode = nodeZone->CreateChild("Grass");
        Vector3 position(Random(minX, minX + size), 0.0f, Random(minZ, minZ + size));
        position.y_ = gTerrain->GetHeight(position + nodeZone->GetPosition()) + 0.5f * scale;
        objectNode->SetPosition(position);
        objectNode->SetRotation(Quaternion(Random(-180.0f, 180.0f), Vector3::UP) * Quaternion(90.0f, Vector3::LEFT));
        objectNode->SetScale(scale);
        nodeZone->GetComponent<StaticModelGroup>()->AddInstanceNode(objectNode);
    }
}

Node* ZoneGrass::CreateHalfZone(Node *node)
{
    Node *nodeHalf = gScene->CreateChild("ZoneGrass");
    nodeHalf->SetPosition(node->GetPosition());

    Vector<SharedPtr<Node>> nodes = node->GetChildren();

    int numHalf = (int)(nodes.Size() / 1.5f);

    unsigned size = nodes.Size();

    for(int i = 0; i < numHalf; i++)
    {
        SharedPtr<Node> node = nodes[Random((int)size--)];
        nodes.Remove(node);
        nodeHalf->AddChild(node);
    }

    Vector<SharedPtr<Node>> nodesAfter = nodeHalf->GetChildren();

    URHO3D_LOGINFOF("%d", nodesAfter.Size());

    return nodeHalf;
}

void ZoneGrass::Update()
{
    float distance = (zones[0]->GetPosition() - gNodeJack->GetPosition()).Length();

    int num = (int)distance / 50;

    Distance newDist = Distance_0;

    if(num < Distance_Size - 1)
    {
        newDist = (Distance)num;
    }
    else
    {
        newDist = (Distance)(Distance_Size - 1);
    }

    if(dist != newDist)
    {
        dist = newDist;
        SwitchState();
    }
}

void ZoneGrass::SwitchState()
{
    for(int i = 0; i < Distance_Size; i++)
    {
        zones[i]->SetDeepEnabled(i < dist ? false : true);
    }
}

float ZoneGrass::CalculateAverageNormal(const Vector3& position, float size)
{
    float step = 1.0f;

    Vector3 sumNormals = Vector3::ZERO;

    float minX = position.x_ - size / 2.0f;
    float minZ = position.z_ - size / 2.0f;

    float maxX = minX + size;
    float maxZ = minZ + size;

    for(float x = minX; x < maxX; x += step)
    {
        for(float z = minZ; z < maxZ; z += step)
        {
            sumNormals += gTerrain->GetNormal({x, 0.0f, z});
        }
    }

    Vector3 aveNormal = sumNormals;
    aveNormal.Normalize();

    aveNormal.y_ = 0.0f;

    return aveNormal.Length();
}
