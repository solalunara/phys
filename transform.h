#ifndef TRANSFORM_H
#define TRANSFORM_H

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

struct Transform
{
    Transform( glm::vec3 pos, glm::quat rot, glm::vec3 scl ) : pos( pos ), rot( rot ), scl( scl ) {}
    glm::vec3 pos;
    glm::quat rot;
    glm::vec3 scl;

    Transform *parent;

    glm::mat4 GetMatrix()
    {
        if ( !parent )
            return glm::translate( glm::mat4( 1 ), pos ) * glm::mat4_cast( rot ) * glm::scale( glm::mat4( 1 ), scl );
        else
            return parent->GetMatrix() * glm::translate( glm::mat4( 1 ), pos ) * glm::mat4_cast( rot ) * glm::scale( glm::mat4( 1 ), scl );
    }
    glm::mat4 GetInverseMatrix()
    {
        return glm::inverse( GetMatrix() );
    }

    glm::vec3 GetAbsOrigin()
    {
        glm::vec3 scl;
        glm::quat rot;
        glm::vec3 pos;
        glm::vec3 skew;
        glm::vec4 persp;
        glm::decompose( GetMatrix(), scl, rot, pos, skew, persp );
        return pos;
    }
    glm::quat GetAbsRot()
    {
        glm::vec3 scl;
        glm::quat rot;
        glm::vec3 pos;
        glm::vec3 skew;
        glm::vec4 persp;
        glm::decompose( GetMatrix(), scl, rot, pos, skew, persp );
        return rot;
    }

    void SetAbsOrigin( glm::vec3 pt )
    {
        if ( !parent )
            pos = pt;
        else
            pos = parent->GetInverseMatrix() * glm::vec4( pos, 1 );
    }
    void SetAbsRot( glm::quat rt )
    {
        if ( !parent )
            rot = rt;
        else
        {
            glm::vec3 scl;
            glm::quat rot_parent;
            glm::vec3 pos;
            glm::vec3 skew;
            glm::vec4 persp;
            glm::decompose( parent->GetMatrix(), scl, rot_parent, pos, skew, persp );
            rot = glm::inverse( rot_parent ) * rt;
        }
    }

    glm::vec3 LocalToWorldDirection( glm::vec3 v )
    {
        return GetMatrix() * glm::vec4( v, 0 );
    }
    glm::vec3 LocalToWorldPoint( glm::vec3 p )
    {
        return GetMatrix() * glm::vec4( p, 1 );
    }
    glm::vec2 LocalToWorldPoint( glm::vec2 p )
    {
        return GetMatrix() * glm::vec4( p, 0, 1 );
    }
    glm::vec3 WorldToLocalDirection( glm::vec3 v )
    {
        return GetInverseMatrix() * glm::vec4( v, 0 );
    }
    glm::vec3 WorldToLocalPoint( glm::vec3 p )
    {
        return GetInverseMatrix() * glm::vec4( p, 1 );
    }
    glm::vec2 WorldToLocalPoint( glm::vec2 p )
    {
        return GetInverseMatrix() * glm::vec4( p, 0, 1 );
    }
};

#endif