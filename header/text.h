#ifndef TEXT_H
#define TEXT_H
#pragma once

#include "element.h"

struct CharacterMesh;
struct Font;


struct UIText :
    public UIElement
{
    UIText( const char *text, Font *font, vec3 pos, float scale, vec3 color, Window *container );
};

struct GameText :
    public GameElement
{
    GameText( const char *text, Font *font, vec3 pos, float scale, vec3 color, Window *container );
};

#endif
