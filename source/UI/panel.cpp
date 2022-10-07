#include "UI/panel.h"
#include "text.h"


UIPanelTitlebar::UIPanelTitlebar( const char *PanelTitle, float x, float y, float scale, float depth, Window *container ) :
    UIElement( container, Transform( vec3( x, y, -1 ), glm::identity<quat>(), vec3( scale ) ) )
{
    _PanelTitle = new char[ strlen( PanelTitle ) + 1 ];
    strcpy( _PanelTitle, PanelTitle );
    //TODO: scale will need to be adjusted
    AddElement( new UIText( PanelTitle, x, y, scale, vec3( 1 ), container ) );
}