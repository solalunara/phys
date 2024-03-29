#include "renderable.h"
#include "mesh.h"
#include "window.h"


Renderable::Renderable( Window *container, Transform *transform, vector<Renderable *> Elements ) :
    container( container ), Elements( Elements ), transform( transform )
{
    for ( int i = 0; i < Elements.size(); ++i )
        Elements[ i ]->transform->SetParent( transform );
    
    container->Elements.push_back( this );
}

Renderable::~Renderable()
{
    delete transform;
    for ( int i = 0; i < Elements.size(); ++i )
        delete Elements[ i ];
    for ( int i = 0; i < container->Elements.size(); ++i )
    {
        if ( container->Elements[ i ] == this )
        {
            container->Elements.erase( container->Elements.begin() + i );
            break;
        }
    }
}

void Renderable::Render()
{
    for ( int i = 0; i < Elements.size(); ++i )
        Elements[ i ]->Render();
}

void Renderable::AddChild( Renderable *e )
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

    e->transform->SetParent( this->transform );
}
void Renderable::RemoveChild( Renderable *e )
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
vector<vec3> Renderable::GetNormals()
{
    vector<vec3> normals;
    normals.reserve( Elements.size() );
    for ( int i = 0; i < Elements.size(); ++i )
    {
        if ( Elements[ i ]->IsMesh() )
            normals.push_back( static_cast<Mesh *>( Elements[ i ] )->GetNormal() );
        else
        {
            vector<vec3> childnormals = Elements[ i ]->GetNormals();
            for ( int j = 0; j < childnormals.size(); ++j )
                normals.push_back( childnormals[ j ] );
        }
    }
    normals.shrink_to_fit();
    return normals;
}
vector<vec3> Renderable::GetVertices( bool local )
{
    vector<vec3> verts;
    verts.reserve( 4 * Elements.size() );
    for ( int i = 0; i < Elements.size(); ++i )
    {
        if ( Elements[ i ]->IsMesh() )
        {
            vector<vec3> meshverts = static_cast<Mesh *>( Elements[ i ] )->GetVertices();
            for ( int j = 0; j < meshverts.size(); ++j )
                verts.push_back( meshverts[ j ] );
        }
        else
        {
            vector<vec3> childverts = Elements[ i ]->GetVertices();
            for ( int j = 0; j < childverts.size(); ++j )
                verts.push_back( childverts[ j ] );
        }
    }
    for ( int i = 0; i < verts.size(); ++i )
    {
        for ( int j = i + 1; j < verts.size(); ++j )
        {
            // round to a precision better than the smallest distance between points on meshes of the object
            // this method probably works for any closed surface - gaps between meshes must be filled with a mesh with a small IPD
            float IPD = SmallestInterPointDist();
            if ( glm::round( verts[ i ] * 10.f / IPD ) == glm::round( verts[ j ] * 10.f / IPD ) )
            {
                verts.erase( verts.begin() + j );
                --j;
            }
        }
        if ( local )
            verts[ i ] = transform->WorldToLocalPoint( verts[ i ] );
    }
    verts.shrink_to_fit();
    return verts;
}
vector<float> Renderable::GetPlaneDists()
{
    vector<float> dists;
    dists.reserve( Elements.size() );
    for ( int i = 0; i < Elements.size(); ++i )
    {
        if ( Elements[ i ]->IsMesh() )
        {
            float meshdist = static_cast<Mesh *>( Elements[ i ] )->GetPlaneDist();
            dists.push_back( meshdist );
        }
        else
        {
            vector<float> childdists = Elements[ i ]->GetPlaneDists();
            for ( int j = 0; j < childdists.size(); ++j )
                dists.push_back( childdists[ j ] );
        }
    }
    dists.shrink_to_fit();
    return dists;
}
vector<Mesh *> Renderable::GetMeshes()
{
    vector<Mesh *> meshes;
    meshes.reserve( Elements.size() );
    for ( int i = 0; i < Elements.size(); ++i )
    {
        if ( Elements[ i ]->IsMesh() )
            meshes.push_back( static_cast<Mesh *>( Elements[ i ] ) );
        else
        {
            vector<Mesh *> children = Elements[ i ]->GetMeshes();
            for ( int j = 0; j < children.size(); ++j )
                meshes.push_back( children[ j ] );
        }
    }
    meshes.shrink_to_fit();
    return meshes;
}

float Renderable::SmallestInterPointDist()
{
    float result = INFINITY;
    for ( int i = 0; i < Elements.size(); ++i )
    {
        if ( Elements[ i ]->IsMesh() && static_cast<Mesh *>( Elements[ i ] )->smallest_inter_point_dist < result )
            result = static_cast<Mesh *>( Elements[ i ] )->smallest_inter_point_dist;
        else
        {
            float childresult = Elements[ i ]->SmallestInterPointDist();
            if ( childresult < result )
                result = childresult;
        }
    }
    return result;
}
