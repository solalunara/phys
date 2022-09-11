#include "element.h"
#include "window.h"
#include "mesh.h"


Element::Element( Window *container, Transform &&transform, vector<Element *> Elements ) :
    container( container ), Elements( Elements ), transform( (Transform &&)transform )
{
    for ( int i = 0; i < Elements.size(); ++i )
        if ( !Elements[ i ]->transform.HasParent() ) Elements[ i ]->transform.SetParent( &transform );
    
    container->Elements.push_back( this );
}

Element::~Element()
{
    for ( int i = 0; i < Elements.size(); ++i )
        delete Elements[ i ];
}

void Element::Render()
{
    for ( int i = 0; i < Elements.size(); ++i )
        Elements[ i ]->Render();
}

void Element::AddElement( Element *e )
{
    Elements.push_back( e );
    
    //if an element is added as a child to another element, remove it from the drawing list
    for ( int i = 0; i < container->Elements.size(); ++i )
    {
        if ( container->Elements[ i ] == e )
        {
            container->Elements.erase( container->Elements.begin() + i );
            break;
        }
    }
}
void Element::RemoveElement( Element *e )
{
    for ( int i = 0; i < Elements.size(); ++i )
    {
        if ( Elements[ i ] == e )
        {
            Elements.erase( Elements.begin() + i );
            break;
        }
    }
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