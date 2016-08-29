#include <Urho3D/UI/UI.h>

#include "GUI.h"


GUI::GUI(Context *context) : 
    UIElement(context)
{
    sprite = new lSprite(context);
    sprite->SetSize(200, 20);
    sprite->SetPosition(GetSubsystem<UI>()->GetRoot()->GetWidth() / 2 - sprite->GetWidth() / 2, 10);
    //AddChild(sprite);
}

void GUI::Init(UIElement* root)
{
    GetSubsystem<UI>()->GetRoot()->AddChild(sprite);
    SetVisible(true); 
}

void GUI::DrawHealth(float health)
{
    sprite->Clear(Color::GRAY);
    int width = sprite->GetWidth() * health / 100.0f;
    sprite->FillRectangle(0, 0, width, sprite->GetHeight(), Color::RED);
}
