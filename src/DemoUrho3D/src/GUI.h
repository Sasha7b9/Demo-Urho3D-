#pragma once

#include <Urho3D/UI/UIElement.h>

#include "lSprite.h"

using namespace Urho3D;

class GUI : public UIElement
{
    URHO3D_OBJECT(GUI, UIElement);

public:
    ///
    GUI(Context *context);
    ///
    void Init(UIElement*);
    /// 
    void DrawHealth(float health);

private:
    SharedPtr<lSprite> sprite;
};

extern GUI *gui;
