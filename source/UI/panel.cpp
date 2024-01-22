#include "UI/panel.h"
#include "text.h"
#include "GlobalTexture.h"


UIPanelTitlebar::UIPanelTitlebar( const char *PanelTitle, vec3 pos, float scale, GlobalTexture *background, Window *container ) :
    UIElement( container, new Transform( pos, glm::identity<quat>(), vec3( scale ) ) )
{
    _PanelTitle = new char[ strlen( PanelTitle ) + 1 ];
    strcpy( _PanelTitle, PanelTitle );
    //TODO: scale will need to be adjusted
    AddElement( new UIText( PanelTitle, NULL, pos, scale, vec3( 1 ), container ) );
    
    float len = strlen( PanelTitle );
    int lines = 1;
    for ( int i = 0; i < len; ++i )
        if ( PanelTitle[ i ] == '\n' ) ++lines;
    vec3 backscale = vec3( 1.5f * len * scale / lines / background->width * 200.f, 1.5f * scale * lines / background->height * 200.f, 1 );
    AddElement( new UIElement( container, new Transform( pos, glm::identity<quat>(), backscale ) ) );
}