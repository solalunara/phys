#ifndef PANEL_H
#define PANEL_H

#pragma once

#include "element.h"
#include <cstring>
#include <glm/glm.hpp>

struct Text;
struct GlobalTexture;

struct UIPanelTitlebar :
    public UIElement
{
    UIPanelTitlebar( const char *PanelTitle, vec3 pos, float scale, GlobalTexture *background, Window *container );

    // const char *const -> a constant pointer to a constant character
    // neccesary to make title read-only and to make the variable itself read-only
    const char *const PanelTitle = _PanelTitle;

private:
    char *_PanelTitle;
};
struct UIPanelBody :
    public UIElement
{

};

/*
struct UIPanel :
    public UIElement
{
    UIPanel( const char *title, float scale,  )
}
*/

#endif