#ifndef PANEL_H
#define PANEL_H

#pragma once

#include "element.h"
#include <cstring>

struct Text;

struct UIPanelTitlebar :
    public UIElement
{
    UIPanelTitlebar( const char *PanelTitle, float x, float y, float scale, float depth, Window *container );

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