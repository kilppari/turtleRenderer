#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <cassert>
#include "wf_loader.h"

using namespace std;

bool WFLoader::load( const char *filepath, FileType type )
{
    FILE        *pFile;
    int         c;
    int         i = 0;
    /* Max line width 100 */
    char        line[ 100 ] = "\0";

    pFile = fopen( filepath, "r" );
    if( pFile != NULL )
    {
        while( ( c = fgetc( pFile ) ) != EOF )
        {
            if( c != '\n' && i < 99 )
            {
                line[ i++ ] = c;
            }
            else
            {
                line[ i ] = '\0';
                switch( type )
                {
                case OBJ_FILE:
                    parseObjectFile( line );
                    break;
                case MTL_FILE:
                    parseMaterialFile( line );
                    break;
                default:
                    assert( 0 );
                }
                line[ 0 ] = '\0';
                i = 0;
            }
        }
        fclose( pFile );
        return true;
    }
    return false;
}

void WFLoader::parseObjectFile( const char *line )
{
    char        tempStr[20] = "\0";
    float       x, y, z;
    int         v1 = -1, v2, v3;
    int         t1 = -1, t2, t3;
    int         n1 = -1, n2, n3;
    char        *pStr = NULL;

    if( strlen( line ) > 0 )
    {
        if( line[ 0 ] == 'm' )
        {
            sscanf( line, "%s", tempStr );
            /* Material library file information */
            if( strcmp( tempStr, "mtllib" ) == 0 )
            {
                sscanf( line, "%*s %s", tempStr );
                load( tempStr, MTL_FILE );
            }
        }
        /* Vertex information
         */
        else if( line[ 0 ] == 'v' )
        {
            /* What type of data. */
            switch( line[ 1 ] )
            {
            /* Vertex coordinates */
            case ' ':
                sscanf( line, "%*s %f %f %f", &x, &y, &z );
                m_LoadedData.vertices.push_back( Vector3f( x, y, z ) );
                break;
            /* Normal vectors */
            case 'n':
                sscanf( line, "%*s %f %f %f", &x, &y, &z );
                m_LoadedData.normals.push_back( Vector3f( x, y, z ) );
                break;
            /* Texture coordinates */
            case 't':
                sscanf( line, "%*s %f %f", &x, &y );
                m_LoadedData.textureCoords.push_back( Vector3f( x, y, 0 ) );
                break;
            default:
                break;
            }
        }
        /* Shading information
         */
        else if( line[ 0 ] == 's' )
        {
            sscanf( line, "%*c %d", &v2 );
            if( v2 > 0 ) m_LoadedData.isSmoothShaded = true;
            else m_LoadedData.isSmoothShaded = false;
        }
        /* Face information
         */
        else if( line[ 0 ] == 'f' )
        {
            sscanf( line, "%*c %s", tempStr );

            /* Check which ones of the possible vertex/texture/normal indice
               information is available. */
            if( ( pStr = strchr( tempStr, '/' ) ) != NULL )
            {
                if( strchr( pStr+1, '/' ) != NULL )
                {
                    /* Vertex and normal indices available,
                       no texture indices. */
                    if( *(pStr + 1) == '/' )
                    {
                        sscanf( line, "%*c %d%*2c%d %d%*2c%d %d%*2c%d", &v1, &n1, &v2, &n2, &v3, &n3 );
                    }
                    /* Vertex, texture and normal indices available. */
                    else
                    {
                        sscanf( line, "%*c %d%*c%d%*c%d %d%*c%d%*c%d %d%*c%d%*c%d",
                                &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3 );
                    }
                }
                /* Vertex and texture indices available, no normal indices. */
                else
                {
                    scanf( line, "%*c %d%*c%d %d%*c%d %d%*c%d", &v1, &t1, &v2, &t2, &v3, &t3 );
                }
            }
            /* Only vertex indices available. */
            else
            {
                scanf( line, "%*c %d %d %d", &v1, &v2, &v3 );
            }
            /* Save face data. */
            m_LoadedData.vertexFaces.push_back( v1 );
            m_LoadedData.vertexFaces.push_back( v2 );
            m_LoadedData.vertexFaces.push_back( v3 );
            if( t1 != -1 )
            {
                m_LoadedData.textureFaces.push_back( t1 );
                m_LoadedData.textureFaces.push_back( t2 );
                m_LoadedData.textureFaces.push_back( t3 );
            }
            if( n1 != -1 )
            {
                m_LoadedData.normalFaces.push_back( n1 );
                m_LoadedData.normalFaces.push_back( n2 );
                m_LoadedData.normalFaces.push_back( n3 );
            }
        }
    }
}

void WFLoader::parseMaterialFile( const char *line )
{
    /* Variables
     */
    char                tempStr[20] = "\0";
    MaterialManager     *matMngrPtr = NULL;
    MaterialData        matData;
    float               r, g, b;

    /* Code
     */
    matMngrPtr = MaterialManager::getInstance();

    if( strlen( line ) > 0 )
    {
        if( line[ 0 ] == 'n' )
        {
            sscanf( line, "%s", tempStr );
            /* Material name */
            if( strcmp( tempStr, "newmtl" ) == 0 )
            {
                sscanf( line, "%*s %s", m_CurrentMatName );
                matMngrPtr->addMaterial( m_CurrentMatName, matData );
            }
        }
        else if( line[ 0 ] == 'N' )
        {
            switch( line[ 1 ] )
            {
            case 's':
                /* Specular exponent ( shininess ) */
                sscanf( line, "%*s %f", &r );
                /* Even though Color4f is used, only the value of r gets
                   saved into the Material values */
                matMngrPtr->setValue( m_CurrentMatName,
                                      MaterialManager::MAT_SHININESS,
                                      Color4f( r, 0.0, 0.0, 1.0 ) );
                break;
            }
        }
        /* Reflectivity information */
        else if( line[ 0 ] == 'K' )
        {

            switch( line[ 1 ] ) //if( line[ 1 ] == 'a' )
            {
            case 'a':
                /* Ambient */
                sscanf( line, "%*s %f %f %f", &r, &g, &b );
                matMngrPtr->setValue( m_CurrentMatName,
                                      MaterialManager::MAT_AMBIENT,
                                      Color4f( r, g, b, 1.0 ) );
                break;
            case 'd':
                /* Diffuse */
                sscanf( line, "%*s %f %f %f", &r, &g, &b );
                matMngrPtr->setValue( m_CurrentMatName,
                                      MaterialManager::MAT_DIFFUSE,
                                      Color4f( r, g, b, 1.0 ) );
                break;
            case 's':
                /* Specular */
                sscanf( line, "%*s %f %f %f", &r, &g, &b );
                matMngrPtr->setValue( m_CurrentMatName,
                                      MaterialManager::MAT_SPECULAR,
                                      Color4f( r, g, b, 1.0 ) );
                break;
            }
        }
    }
}

void WFLoader::printData()
{
    for( int i = 0; i < m_LoadedData.vertices.size(); i++ )
    {
        cout << "x: " << m_LoadedData.vertices[ i ].x;
        cout << ", y: " << m_LoadedData.vertices[ i ].y;
        cout << ", z: " << m_LoadedData.vertices[ i ].z << endl;
    }
    for( int i = 0; i < m_LoadedData.normals.size(); i++ )
    {
        cout << "x: " << m_LoadedData.normals[ i ].x;
        cout << ", y: " << m_LoadedData.normals[ i ].y;
        cout << ", z: " << m_LoadedData.normals[ i ].z << endl;
    }
    for( int i = 0; i < m_LoadedData.vertexFaces.size(); i++ )
    {
        cout << "v" << i << ": " << m_LoadedData.vertexFaces[ i ];
        if( i % 3 == 0 ) cout << endl;
        else cout << ", ";
    }
    for( int i = 0; i < m_LoadedData.normalFaces.size(); i++ )
    {
        cout << "n" << i << ": " << m_LoadedData.normalFaces[ i ];
        if( i % 3 == 0 ) cout << endl;
        else cout << ", ";
    }
}
/*
int main( void )
{
    WFLoader loader;
    loader.load( "testbox.obj" );
    loader.printData();
    return 0;
}
*/
