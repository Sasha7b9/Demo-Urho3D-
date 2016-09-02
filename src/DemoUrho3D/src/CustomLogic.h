#pragma once

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Graphics/Material.h>

using namespace Urho3D;

class CustomLogic : public LogicComponent
{
	URHO3D_OBJECT(CustomLogic, LogicComponent);
public:
	

	CustomLogic(Context* context);
	virtual void Start();
	void Update(float timeStep);
	void FixedUpdate(float timeStep);

	static void RegisterObject(Context* context);

	SharedPtr<Material> mat;



};