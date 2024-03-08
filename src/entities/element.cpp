#include "element.h"
#include "render/mesh.h"
#include "physics/physics.h"
#include "maths/function.h"
#include "physics/collide.h"


Element::Element( Window *container, Transform *transform, vector<Renderable *> Elements ) :
    Renderable( container, transform, Elements )
{
}

//do not call base-class deconstructors: https://stackoverflow.com/questions/677620/do-i-need-to-explicitly-call-the-base-virtual-destructor
Element::~Element()
{
    delete phys_obj;
    delete collide;
}

void Element::Render()
{
    Renderable::Render();

    if ( phys_obj )
    {
        phys_obj->FrameUpdate();

        //only if phys_obj since otherwise we can't resolve a collision
        if ( collide )
        {
            for ( int i = 0; i < container->Elements.size(); ++i ) 
            {
                Element *child_i = dynamic_cast<Element *>( container->Elements[ i ] );
                if ( child_i && this != child_i && child_i->collide )
                {
                    IntersectionData id = collide->GetIntersection( child_i->collide );
                    if ( id.Intersection )
                        collide->ResolveIntersection( child_i->collide, id );
                }
            }
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