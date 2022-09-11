#include "element.h"
#include "window.h"
#include "mesh.h"

Element::~Element()
{
    for ( int i = 0; i < Meshes.size(); ++i )
        delete Meshes[ i ];
}

void Element::Render()
{
    for ( int i = 0; i < Meshes.size(); ++i )
        Meshes[ i ]->Render();
}

void UIElement::Render()
{
    container->shader.SetShaderValue( "UI", true );
    Element::Render();
}

void GameElement::Render()
{
    container->shader.SetShaderValue( "UI", false );
    Element::Render();
}