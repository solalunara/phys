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
        {
            glm::vec3 scl_parent;
            glm::quat rot_parent;
            glm::vec3 pos_parent;
            glm::vec3 skew;
            glm::vec4 persp;
            glm::decompose( parent->GetMatrix(), scl_parent, rot_parent, pos_parent, skew, persp );
            return glm::translate( glm::mat4( 1 ), pos + pos_parent ) * glm::mat4_cast( rot * rot_parent ) * glm::scale( glm::mat4( 1 ), scl * scl_parent );
        }
    }
    glm::mat4 GetInverseMatrix()
    {
        return glm::inverse( GetMatrix() );
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