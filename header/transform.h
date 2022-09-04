#ifndef TRANSFORM_H
#define TRANSFORM_H

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::quat;
using glm::mat4;

#include <vector>
using std::vector;

struct Transform
{
    Transform( vec3 pos, quat rot, vec3 scl ) : pos( pos ), rot( rot ), scl( scl ), parent( NULL ) {}
    vec3 pos;
    quat rot;
    vec3 scl;

    mat4 GetMatrix()
    {
        return ( parent ? parent->GetMatrix() : glm::identity<mat4>() ) * glm::translate( mat4( 1 ), pos ) * glm::mat4_cast( rot ) * glm::scale( mat4( 1 ), scl );
    }
    mat4 GetInverseMatrix()
    {
        return glm::inverse( GetMatrix() );
    }

    vec3 GetAbsOrigin()
    {
        vec3 scl;
        quat rot;
        vec3 pos;
        vec3 skew;
        vec4 persp;
        glm::decompose( GetMatrix(), scl, rot, pos, skew, persp );
        return pos;
    }
    quat GetAbsRot()
    {
        vec3 scl;
        quat rot;
        vec3 pos;
        vec3 skew;
        vec4 persp;
        glm::decompose( GetMatrix(), scl, rot, pos, skew, persp );
        return rot;
    }

    void SetAbsOrigin( vec3 pt )
    {
        if ( !parent )
            pos = pt;
        else
            pos = parent->GetInverseMatrix() * vec4( pos, 1 );
    }
    void SetAbsRot( quat rt )
    {
        if ( !parent )
            rot = rt;
        else
        {
            vec3 scl;
            quat rot_parent;
            vec3 pos;
            vec3 skew;
            vec4 persp;
            glm::decompose( parent->GetMatrix(), scl, rot_parent, pos, skew, persp );
            rot = glm::inverse( rot_parent ) * rt;
        }
    }

    vec3 LocalToWorldDirection( vec3 v )
    {
        return GetMatrix() * vec4( v, 0 );
    }
    vec3 LocalToWorldPoint( vec3 p )
    {
        return GetMatrix() * vec4( p, 1 );
    }
    vec2 LocalToWorldPoint( vec2 p )
    {
        return GetMatrix() * vec4( p, 0, 1 );
    }
    vec3 WorldToLocalDirection( vec3 v )
    {
        return GetInverseMatrix() * vec4( v, 0 );
    }
    vec3 WorldToLocalPoint( vec3 p )
    {
        return GetInverseMatrix() * vec4( p, 1 );
    }
    vec2 WorldToLocalPoint( vec2 p )
    {
        return GetInverseMatrix() * vec4( p, 0, 1 );
    }

    void SetParent( Transform *parent )
    {
        if ( this->parent )
        {
            int i;
            for ( i = 0; i < this->parent->children.size(); ++i )
                if ( this->parent->children[ i ] == this ) break;
            this->parent->children.erase( this->parent->children.begin() + i );
        }
        if ( parent )
            parent->children.push_back( this );
        this->parent = parent;
    }
    bool HasParent() { return parent; }

    Transform( const Transform & ) = delete;
    Transform &operator =( const Transform & ) = delete;
    Transform( Transform &&other ) :
        pos( other.pos ), rot( other.rot ), scl( other.scl ), parent( other.parent ), children( vector<Transform *>() )
    {
        for ( int i = 0; i < other.children.size(); ++i )
        {
            Transform *child = other.children[ i ];
            child->parent = this;
            children.push_back( child );
        }
    }

private:
    Transform *parent = NULL;
    vector<Transform *> children = vector<Transform *>();
};

#endif