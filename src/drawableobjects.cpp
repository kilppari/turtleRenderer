/* --------------------------------------------------------------------------
 *
 * drawableobjects.cpp
 *
 * Implementations of drawable objects.
 *
 * 2011 (C) Pekka Mäkinen
 *
 * -------------------------------------------------------------------------- */

/* Version history
 *
 * --------------------------------------------------------------------------
 * Version  : < ?.? >
 * Date     : < DD-Month-YYYY hh:mm >
 * Author   : < name >
 * Comments :
 * < Add comments about changes >
 * --------------------------------------------------------------------------
 * Version  : 2.0
 * Date     : 10-Jun-2011 21:03
 * Author   : Pekka Mäkinen
 * Comments :
 *  - Added ParticleBox and Robot -drawables.
 * --------------------------------------------------------------------------
 * Version  : 1.1
 * Date     : 24-Apr-2011 12:15
 * Author   : Pekka Mäkinen
 * Comments :
 *  - Updates to MaterialManager
 * --------------------------------------------------------------------------
 * Version  : 1
 * Date     : 3-May-2011 18:35
 * Author   : Pekka Mäkinen
 * Comments :
 *  - File template updated. Starting version numbering from 1.0.
 *    Notable changes:
 *    Added a new class that supports WaveFront objects and a class to
 *    demonstrate DDA algorithm, Bresenham's circle algorithm and
 *    supersampling.
 * -------------------------------------------------------------------------- */

/* **************************************************************************

    Contents

   ************************************************************************** */
/*
    Version history
    Contents
    1. Mandatory include files
    2. Function implementations
        2.1. BaseDrawable
            2.1.1. BaseDrawable ( ctor )
            2.1.2. setScaling & scale
            2.1.3. checkBounds
        2.2. Box
            2.2.1. Box ( ctor )
            2.2.2. draw
            2.2.3. draw ( overloaded )
        2.3. Plane
            2.3.1. Plane ( ctor )
            2.3.2. draw
        2.4. WFObject
            2.4.1. WFObject ( ctor )
            2.4.2. draw
            2.4.3. setMaterial
            2.4.4. setTexture
        2.5. RasterMap
            2.5.1. RasterMap ( ctor & dtor )
            2.5.2. drawPixel
            2.5.3. superSample
            2.5.4. drawLine
            2.5.5. drawCircle
            2.5.6. draw
        2.6. ParticleBox
            2.6.1. ParticleBox ( ctor )
            2.6.2. draw
        2.7. Robot
            2.7.1. Robot ( ctor )
            2.7.2. createBody
            2.7.3. drawWheel
            2.7.4. drawBody
            2.7.5. move
            2.7.6. draw

*/

/* **************************************************************************

    1. Mandatory include files

   ************************************************************************** */

#include "wf_loader.h"
#include <cassert>
//#include <QGLWidget>
#include <math.h>
#include "timer.h"
#include "stdio.h"
#include "drawableobjects.h"
#include <GL/glu.h>

/* **************************************************************************

    2. Function implementations

   ************************************************************************** */

const float PI = 3.14159265;

/* --------------------------------------------------------------------------
 *  2.1. BaseDrawable
 *
 *  See drawableobjects.h for class definition.
 * -------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------
 *  2.1.1. BaseDrawable
 *
 *  Constructor
 * -------------------------------------------------------------------------- */
BaseDrawable::BaseDrawable() :
    m_IsMovable( false ),
    m_IsRotatable( false )
{
    m_Position  = Vector3f( 0, 0, 0 );
    m_Rotation  = Vector3f( 0, 0, 0 );
    m_Scaling   = Vector3f( 1, 1, 1 );
    m_Color     = Vector3f( 0, 0, 0 );
    m_Origin    = Vector3f( 0, 0, 0 );
}

/* --------------------------------------------------------------------------
 *  2.1.2. setScaling & scale
 *
 *  Setters for scale values. Each function also checks the scale bounds.
 * -------------------------------------------------------------------------- */
void BaseDrawable::setScaling( const Vector3f &scale )
{
    m_Scaling = scale;
    checkBounds();
}
void BaseDrawable::setScaling( GLfloat x, GLfloat y, GLfloat z )
{
    m_Scaling = Vector3f( x, y ,z );
    checkBounds();
}
void BaseDrawable::scale( GLfloat x, GLfloat y, GLfloat z )
{
    m_Scaling.x += x;
    m_Scaling.y += y;
    m_Scaling.z += z;
    checkBounds();
}

/* --------------------------------------------------------------------------
 *  2.1.3. checkBounds
 *
 *  Checks if scale value is inside acceptable values.
 * -------------------------------------------------------------------------- */
void BaseDrawable::checkBounds()
{
    if( m_Scaling.x < 0.2 ) m_Scaling.x = 0.2;
    if( m_Scaling.y < 0.2 ) m_Scaling.y = 0.2;
    if( m_Scaling.z < 0.2 ) m_Scaling.z = 0.2;
    if( m_Scaling.x > 2 ) m_Scaling.x = 2;
    if( m_Scaling.y > 2 ) m_Scaling.y = 2;
    if( m_Scaling.z > 2 ) m_Scaling.z = 2;
}

/* --------------------------------------------------------------------------
 *  2.2. Box
 *
 *  See drawableobjects.h for class definition.
 * -------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------
 *  2.2.1. Box
 *
 *  Creates vertices and normals for a box object. Size can be given as width,
 *  height an depth values. Origin is at the centre of the object.
 * -------------------------------------------------------------------------- */
Box::Box( GLfloat w, GLfloat h, GLfloat d )
{
    assert( w > 0 && h > 0 && d > 0 );
    w /= 2;
    h /= 2;
    d /= 2;

    /* Coord of each corner. */
    GLfloat temp[] = { -w, -h, -d,
                        w, -h, -d,
                        w,  h, -d,
                       -w,  h, -d,
                       -w, -h,  d,
                        w, -h,  d,
                        w,  h,  d,
                       -w,  h,  d };

    memcpy( m_Vertices, temp, sizeof(temp) );

    /* Generate normal vectors, one for each side */
    GLfloat normals[] = { 0,  0,  1,
                          1,  0,  0,
                          0, -1,  0,
                          0,  0, -1,
                         -1,  0,  0,
                          0,  1,  0 };

    memcpy( m_Normals, normals, sizeof( normals ) );

    /* Define which side is assosiated to which corner coords.
     * All coords must be on the same plane. */
    GLubyte frontIndices[]      = { 4, 5, 6, 7 };
    GLubyte rightIndices[]      = { 1, 2, 6, 5 };
    GLubyte bottomIndices[]     = { 0, 1, 5, 4 };
    GLubyte backIndices[]       = { 0, 3, 2, 1 };
    GLubyte leftIndices[]       = { 0, 4, 7, 3 };
    GLubyte topIndices[]        = { 2, 3, 7, 6 };

    memcpy( m_Indices[0], frontIndices, sizeof( frontIndices ) );
    memcpy( m_Indices[1], rightIndices, sizeof( rightIndices ) );
    memcpy( m_Indices[2], bottomIndices, sizeof( bottomIndices ) );
    memcpy( m_Indices[3], backIndices, sizeof( backIndices ) );
    memcpy( m_Indices[4], leftIndices, sizeof( leftIndices ) );
    memcpy( m_Indices[5], topIndices, sizeof( topIndices ) );

    /* Defines how many points each indice has.
     * This is for the glMultiDrawElements. */
    GLsizei tempCount[] = { 4, 4, 4, 4, 4, 4 };
    memcpy( m_Count, tempCount, sizeof( tempCount ) );

}

/* --------------------------------------------------------------------------
 *  2.2.2. draw
 *
 *  Implementation of the pure virtual method.
 *  Calls draw( bool, bool ) which does the real drawing.
 * -------------------------------------------------------------------------- */
void Box::draw()
{
    draw( true );
}

/* --------------------------------------------------------------------------
 *  2.2.3. draw ( overloaded )
 *
 *  Draws the box using vertex array. Can be drawn as a solid or a wireframe
 *  model. Parameter loadIdentity defines if the matrix multiplications should
 *  be done on a identity matrix or on top of previous multiplications.
 * -------------------------------------------------------------------------- */
void Box::draw( bool loadIdentity, bool wireframe )
{
    glMatrixMode( GL_MODELVIEW );
    if( loadIdentity )
        glLoadIdentity();

    /* Position, scaling and rotation.
     */
    glTranslatef( m_Position.x, m_Position.y, m_Position.z );

    glScalef( m_Scaling.x, m_Scaling.y, m_Scaling.z );

    glRotatef( m_Rotation.x, 1.0, 0.0, 0.0 );
    glRotatef( m_Rotation.y, 0.0, 1.0, 0.0 );
    glRotatef( m_Rotation.z, 0.0, 0.0, 1.0 );

    /* Origin translation */
    glTranslatef( m_Origin.x, m_Origin.y, m_Origin.z );

    GLfloat color[] = { m_Color.x / 255, m_Color.y / 255, m_Color.z / 255, 1.0 };
    GLfloat shininess[] = { 2.0 };

    /* Set material's ambient and diffuse parameters.
     * In other words: What color our object seems to be. */
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color );
    glMaterialfv( GL_FRONT, GL_SHININESS, shininess );

    /* Enable vertex array and tell OpenGL where our stored vertices lie. */
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, m_Vertices );

    /* Draw each side of the box. */
    for( int i = 0; i < 6; i++ )
    {
        if( wireframe )
            glBegin( GL_LINE_STRIP );
        else
            glBegin( GL_QUADS );

        glNormal3fv( &m_Normals[ i * 3 ] );
        glArrayElement( m_Indices[ i ][ 0 ] );
        glArrayElement( m_Indices[ i ][ 1 ] );
        glArrayElement( m_Indices[ i ][ 2 ] );
        glArrayElement( m_Indices[ i ][ 3 ] );
        glEnd();
    }
}

/* --------------------------------------------------------------------------
 *  2.3. Plane
 *
 *  See drawableobjects.h for class definition.
 * -------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------
 *  2.3.1 Plane ( ctor )
 *
 *  Constructor for a plane object.
 * -------------------------------------------------------------------------- */
Plane::Plane( GLfloat w, GLfloat h )
{
    assert( w > 0 && h > 0 );

    w /= 2;
    h /= 2;
    /* Create coords that lie on a z-plane. */
    GLfloat vertices[] = { -w, -h, 0,
                            w, -h, 0,
                           -w,  h, 0,
                            w,  h, 0 };

    memcpy( m_Vertices, vertices, sizeof( vertices ) );
}

/* --------------------------------------------------------------------------
 *  2.3.2. draw
 *
 *  Draw the plane.
 * -------------------------------------------------------------------------- */
void Plane::draw()
{
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glTranslatef( m_Position.x, m_Position.y, m_Position.z );
    glRotatef( m_Rotation.x, 1.0, 0.0, 0.0 );
    glRotatef( m_Rotation.y, 0.0, 1.0, 0.0 );
    glRotatef( m_Rotation.z, 0.0, 0.0, 1.0 );

    GLfloat color[] = { m_Color.x / 255, m_Color.y / 255, m_Color.z / 255, 1.0 };


    GLfloat shininess[] = { 50.0 };


    /* Set material's ambient and diffuse parameters.
     * In other words: What color our object seems to be. */

    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color );

    glMaterialfv( GL_FRONT, GL_SHININESS, shininess );

    /* Draw using triangle strips. */
    glBegin( GL_TRIANGLE_STRIP );

    glNormal3f( 0.0, -1.0, 0.0 );
    glTexCoord2f( 0, 0 );        glVertex3fv( &m_Vertices[0] );
    glTexCoord2f( 1, 0 );        glVertex3fv( &m_Vertices[3] );
    glTexCoord2f( 0, 1 );        glVertex3fv( &m_Vertices[6] );
    glTexCoord2f( 1, 1 );        glVertex3fv( &m_Vertices[9] );

    glEnd();
}

/* --------------------------------------------------------------------------
 *  2.4. WFObject
 *
 *  See drawableobjects.h for class definition.
 * -------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------
 *  2.4.1. WFObject ( ctor )
 *
 *  Loads a WaveFront object file and parses model data to m_ModelData.
 * -------------------------------------------------------------------------- */
WFObject::WFObject( const char *filename )
{
    WFLoader loader;
    loader.load( filename, WFLoader::OBJ_FILE );

    m_ModelData = loader.m_LoadedData;
}

/* --------------------------------------------------------------------------
 *  2.4.2. draw
 *
 *  Draws the model using parameters from m_ModelData and m_MaterialData.
 * -------------------------------------------------------------------------- */
void WFObject::draw()
{
    GLfloat color[ 4 ];

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    /* Orientations
     */
    glTranslatef( m_Position.x, m_Position.y, m_Position.z );
    glRotatef( m_Rotation.x, 1.0, 0.0, 0.0 );
    glRotatef( m_Rotation.y, 0.0, 1.0, 0.0 );
    glRotatef( m_Rotation.z, 0.0, 0.0, 1.0 );

    /* Material reflective attributes
     */
    color[ 3 ] = 1.0;
    color[ 0 ] = m_MaterialData.ambient.r;
    color[ 1 ] = m_MaterialData.ambient.g;
    color[ 2 ] = m_MaterialData.ambient.b;
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, color );
    color[ 0 ] = m_MaterialData.diffuse.r;
    color[ 1 ] = m_MaterialData.diffuse.g;
    color[ 2 ] = m_MaterialData.diffuse.b;
    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, color );
    color[ 0 ] = m_MaterialData.specular.r;
    color[ 1 ] = m_MaterialData.specular.g;
    color[ 2 ] = m_MaterialData.specular.b;
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, color );

    glMaterialf( GL_FRONT, GL_SHININESS, m_MaterialData.shininess );

    TextureManager *texMngrPtr;
    texMngrPtr = TextureManager::getInstance();
    const Texture *tex = texMngrPtr->getTexturePtr( m_TextureName.c_str() );


    glEnable( GL_TEXTURE_2D );

    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    glBindTexture( GL_TEXTURE_2D, tex->id );

    /* Vertices
     */
    glBegin( GL_TRIANGLES );
    for( unsigned int i = 0; i < m_ModelData.vertexFaces.size(); i++ )
        {
            /* Subtract one, because numbering starts from 1 in
               WaveFront files. */
            int v = m_ModelData.vertexFaces[ i ] - 1;
            int n = m_ModelData.normalFaces[ i ] - 1;
            int t = m_ModelData.textureFaces[ i ] - 1;

            glTexCoord2f( m_ModelData.textureCoords[ t ].x,
                          m_ModelData.textureCoords[ t ].y );
            glNormal3f( m_ModelData.normals[ n ].x, m_ModelData.normals[ n ].y,
                        m_ModelData.normals[ n ].z );
            glVertex3f( m_ModelData.vertices[ v ].x,
                        m_ModelData.vertices[ v ].y,
                        m_ModelData.vertices[ v ].z );
        }
    glEnd();
    glDisable( GL_TEXTURE_2D );
}

/* --------------------------------------------------------------------------
 *  2.4.3. setMaterial
 *
 *  Get material 'name' from Material manager and sets it to this model's
 *  material.
 * -------------------------------------------------------------------------- */
bool WFObject::setMaterial( const char *name )
{
    MaterialManager *matMngrPtr = NULL;
    matMngrPtr = MaterialManager::getInstance();
    if( matMngrPtr != NULL )
    {
        m_MaterialData = matMngrPtr->getMaterial( name );
        return true;
    }
    return false;
}

/* --------------------------------------------------------------------------
 *  2.4.4. setTexture
 *
 *  Sets the name of the texture used in this model.
 * -------------------------------------------------------------------------- */
void WFObject::setTexture( const char *name )
{
    m_TextureName = std::string( name );
}

/* --------------------------------------------------------------------------
 *  2.5. RasterMap
 *
 *  See drawableobjects.h for class definition.
 * -------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------
 *  2.5.1. RasterMap ( ctor & dtor )
 *
 *  Creates a Rastersimulator.
 * -------------------------------------------------------------------------- */
RasterMap::RasterMap( int width, int height, GLfloat pixelSize ) :
    m_GridWidth( width ),
    m_GridHeight( height ),
    m_PixelSize( pixelSize )
{
    /* Simulate raster map by allocating grid of color values. */
    m_RasterMap = new Color4f*[ width ];
    for( int x = 0; x < width; x++ )
    {
        m_RasterMap[x] = new Color4f[ height ];
        for( int y = 0; y < height; y++ )
        {
            /* Set default color to match our scene's whiteboard's color */
            m_RasterMap[x][y] = Color4f( 73, 74, 74, 255 );
        }

    }
}

RasterMap::~RasterMap()
{
    if( m_RasterMap != NULL )
    {
        for( int x = 0; x < m_GridWidth; ++x )
        {
            delete [] m_RasterMap[x];
        }
        delete [] m_RasterMap;
    }
}

/* --------------------------------------------------------------------------
 *  2.5.2. drawPixel
 *
 *  Draws a 'pixel' with GL_QUADS. Pixel size and boundaries are
 *  predetermined. If pixel is out of bounds it will get clipped.
 * -------------------------------------------------------------------------- */
void RasterMap::drawPixel( int x, int y, const Color4f &color )
{
    GLfloat fX, fY;
    /* Simple clipping. */
    if( x < 0 || x > m_GridWidth - 1 || y < 0 || y > m_GridHeight - 1 )
        return;

    fX = ( GLfloat )x * m_PixelSize;
    fY = ( GLfloat )y * m_PixelSize;

    m_RasterMap[x][y] = color;

    glColor4f( color.r/255, color.g/255, color.b/255, 1.0 );
    glBegin( GL_QUADS );
    glVertex3f( fX, fY, 0 );
    glVertex3f( fX + m_PixelSize, fY, 0 );
    glVertex3f( fX + m_PixelSize, fY + m_PixelSize, 0 );
    glVertex3f( fX, fY + m_PixelSize, 0 );
    glEnd();
}

/* --------------------------------------------------------------------------
 *  2.5.3. superSample
 *
 *  Function for anti aliasing.
 *  Calculates pixel's color by using weighted values of neighbouring pixels.
 * -------------------------------------------------------------------------- */
void RasterMap::superSample( int x, int y )
{
   Color4f colorGrid[3][3];
   int tX, tY;
   GLfloat r, g, b;

   /* Get 3x3 grid of neighbouring pixels. */
   for( int iX = -1; iX < 2; iX++ )
   {
       tX = x + iX;
       if( tX < 0 ) tX = 0;
       if( tX >= m_GridWidth ) tX = x;

       for( int iY = -1; iY < 2; iY++ )
       {
           tY = y + iY;
           if( tY < 0 ) tY = 0;
           if( tY >= m_GridHeight ) tY = y;

           colorGrid[iX+1][iY+1] = m_RasterMap[tX][tY];
       }
   }

   /* Calculate color values. */
   r = colorGrid[0][0].r + 2*colorGrid[1][0].r +   colorGrid[2][0].r +
       2*colorGrid[0][1].r + 4*colorGrid[1][1].r + 2*colorGrid[2][1].r +
       colorGrid[0][2].r + 2*colorGrid[1][2].r +   colorGrid[2][2].r;
   g = colorGrid[0][0].g + 2*colorGrid[1][0].g +   colorGrid[2][0].g +
       2*colorGrid[0][1].g + 4*colorGrid[1][1].g + 2*colorGrid[2][1].g +
       colorGrid[0][2].g + 2*colorGrid[1][2].g +   colorGrid[2][2].g;
   b = colorGrid[0][0].b + 2*colorGrid[1][0].b +   colorGrid[2][0].b +
       2*colorGrid[0][1].b + 4*colorGrid[1][1].b + 2*colorGrid[2][1].b +
       colorGrid[0][2].b + 2*colorGrid[1][2].b +   colorGrid[2][2].b;

   r /= 16;
   g /= 16;
   b /= 16;

   /* Draw pixel using the calculated color. */
   drawPixel( x, y, Color4f( r, g, b, 1.0 ) );
}

/* --------------------------------------------------------------------------
 *  2.5.4. drawLine
 *
 *  Draws a line of pixels using the DDA ( Digital differential analyzer )
 *  algorithm.
 *  Reference: Chapter 6.8: Rasterization, Interactive computer graphics
 *             ( 6th edition )
 * -------------------------------------------------------------------------- */
void RasterMap::drawLine( int x0, int y0, int x1, int y1 )
{
    int ix, iy;
    GLfloat y = y0, x = x0;

    /* Calculate slope */
    GLfloat m = ( GLfloat )( y1 - y0 ) / ( GLfloat )( x1 - x0 );

    /* Draw pixels */
    if( m <= 1 )
    {
        for( ix = x0; ix <= x1; ix++ )
        {
            /* For each x, increase y by slope and
               round it to nearest integer */
            drawPixel( ix, floor( y + 0.5 ), m_Color );
            /* Antialiasing */
            superSample( ix, floor( y + 0.5 ) - 1 );
            superSample( ix, floor( y + 0.5 ) + 1 );
            y += m;
        }
    }
    /* Swap the roles of x and y for large slopes. */
    else
    {
        for( iy = y0; iy <= y1; iy++ )
        {
            drawPixel( floor( x + 0.5 ), iy, m_Color );
            superSample( floor( x + 0.5 ) - 1, iy );
            superSample( floor( x + 0.5 ) + 1, iy );
            x += (1.0 / m);
        }
    }
}

/* --------------------------------------------------------------------------
 *  2.5.5. drawCircle
 *
 *  Draws a circle of pixels using the Bresenham's Circle algorithm.
 *  One eight of the circle points are calculated and rest can be determined
 *  by using symmetry.
 *  Reference: Lecture slides and wikipedia.
 * -------------------------------------------------------------------------- */
void RasterMap::drawCircle( int x0, int y0, int R )
{
    int x = 0;
    int y = R;
    int deltaE, deltaSE;
    float decision;

    deltaE = 2 * x + 3;
    deltaSE = 2 * ( x - y ) + 5;
    decision = ( x+1 ) * ( x+1 ) + ( y-0.5 ) * ( y-0.5 ) - R * R;

    /* Starting pixels. */
    drawPixel( x0, y0 + R, m_Color );
    drawPixel( x0 + R, y0, m_Color );
    drawPixel( x0, y0 - R, m_Color );
    drawPixel( x0 - R, y0, m_Color );

    while( y > x )
    {
        if( decision < 0 )
        {
            /* Move east. */
            decision += deltaE;
        }
        else
        {
            /* Move south east. */
            y--;
            decision += deltaSE;
            deltaSE += 2;
        }

        deltaSE += 2;
        deltaE += 2;
        x++;

        /* Draw eight pixels each round so that full circle is drawn. */
        drawPixel( x0 + x, y0 + y, m_Color );
        drawPixel( x0 + x, y0 - y, m_Color );
        drawPixel( x0 - x, y0 - y, m_Color );
        drawPixel( x0 - x, y0 + y, m_Color );
        drawPixel( x0 + y, y0 + x, m_Color );
        drawPixel( x0 + y, x0 - x, m_Color );
        drawPixel( x0 - y, x0 - x, m_Color );
        drawPixel( x0 - y, x0 + x, m_Color );
    }
}

/* --------------------------------------------------------------------------
 *  2.5.6. draw
 *
 *  Draws a grid of the rastermap and various lines and circles. Lines
 *  are antialiased.
 * -------------------------------------------------------------------------- */
void RasterMap::draw()
{
    glShadeModel( GL_FLAT );
    glDisable( GL_LIGHTING );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glTranslatef( m_Position.x, m_Position.y, m_Position.z );
    glRotatef( m_Rotation.x, 1.0, 0.0, 0.0 );
    glRotatef( m_Rotation.y, 0.0, 1.0, 0.0 );
    glRotatef( m_Rotation.z, 0.0, 0.0, 1.0 );
    glScalef( m_Scaling.x, m_Scaling.y, m_Scaling.z );

    /* Draw the grid */
    glColor4f( 0.3, 0.3, 0.3, 1.0 );
    glBegin( GL_LINES );
    for( int x = 0; x <= m_GridWidth; x++ )
    {
        glVertex3f( m_PixelSize * x, 0, 0 );
        glVertex3f( m_PixelSize * x, m_PixelSize * m_GridHeight, 0 );
    }
    for( int y = 0; y <= m_GridHeight; y++ )
    {
        glVertex3f( 0, m_PixelSize * y, 0 );
        glVertex3f( m_PixelSize * m_GridWidth, m_PixelSize * y, 0 );
    }
    glEnd();
    setColor( Color4f( 255, 255, 0, 0 ) );
    drawCircle( 0, 0, 15 );

    setColor( Color4f( 255, 0, 0, 0 ) );
    drawLine( 0, 0, 14, 9 );

    setColor( Color4f( 255, 0, 255, 0 ) );
    drawLine( -4, 20, 35, 5 );

    setColor( Color4f( 70, 30, 255, 0 ) );
    drawLine( 30, -3, 33, 34 );

    setColor( Color4f( 10, 30, 100, 0 ) );
    drawLine( -3, 23, 35, 23 );

    setColor( Color4f( 145, 20, 0, 0 ) );
    drawCircle( 17, 17, 10 );

    glEnable( GL_LIGHTING );
    glShadeModel( GL_SMOOTH );
}

/* --------------------------------------------------------------------------
 *  2.6. ParticleBox
 *
 *  See drawableobjects.h for class definition.
 *  Reference: Chapter 9.6 in Interactive Computer Graphics, 6th Ed.
 *  ( Edvard Angel, Dave Shreiner )
 * -------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------
 *  2.6.1. ParticleBox ( ctor & dtor )
 *
 *  Creates a particle system
 * -------------------------------------------------------------------------- */
ParticleBox::ParticleBox( float sideLength, bool gravity, float coef ) :
    m_SideLength( sideLength ),
    m_Coef( coef ),
    m_GravityEnabled( gravity )
{
    /* Initialize particle positions and velocities. */
    for( int i = 0; i < MAX_PARTICLES; i++ )
    {
        m_Particles[ i ].mass = 1.0f;

        for( int j = 0; j < 3; j++ )
        {
            m_Particles[ i ].color[j] = ( float )rand() / RAND_MAX;
            m_Particles[ i ].position[ j ] =
                sideLength * ( ( float ) rand() / RAND_MAX ) - sideLength / 2;
            m_Particles[ i ].velocity[ j ] = 1 * ( ( float ) rand() / RAND_MAX );
        }
        m_Particles[ i ].position[ 3 ] = 1.0;
        m_Particles[ i ].velocity[ 3 ] = 0.0;
        m_Particles[ i ].color[ 3 ] = 1.0;
    }
    /* Generate new buffer object */
//    glGenBuffers( 1, m_Buffers[0] );
}

/* --------------------------------------------------------------------------
 *  2.6.2 checkGravity
 *
 *  returns 'gravity' multiplier, if it is enabled.
 * -------------------------------------------------------------------------- */
float ParticleBox::checkGravity( int i, int j )
{
    if( !m_GravityEnabled ) return( 0.0 );
    else if( j == 1 ) return( -1.0 );
    else return( 0.0 );
}

/* --------------------------------------------------------------------------
 *  2.6.3 checkCollision
 *
 *  Checks if a specific particle collided
 * -------------------------------------------------------------------------- */
void ParticleBox::checkCollision( int i )
{
    for( int j = 0; j < 3; j++ )
    {
        float posBoundary = m_SideLength / 2;
        float negBoundary = - m_SideLength / 2;
        if( m_Particles[ i ].position[ j ] >= posBoundary )
        {
            m_Particles[ i ].velocity[ j ] = - m_Coef *
                m_Particles[ i ].velocity[ j ];
            m_Particles[ i ].position[ j ] = posBoundary -
                m_Coef * ( m_Particles[ i ].position[ j ] - posBoundary );
        }
        if( m_Particles[ i ].position[ j ] <= negBoundary )
        {
            m_Particles[ i ].velocity[ j ] =
                - m_Coef * m_Particles[ i ].velocity[ j ];
            m_Particles[ i ].position[ j ] = negBoundary +
                m_Coef * ( m_Particles[ i ].position[ j ] - negBoundary );
        }
    }
}

GLfloat ParticleBox::getBoxPos( int j )
{
    assert( j < 3 );
    if( j == 0 ) return m_Position.x;
    else if( j == 1 ) return m_Position.y;
    else if( j == 2 ) return m_Position.z;

    return 0;
}

/* --------------------------------------------------------------------------
 *  2.6.4. draw
 *
 *  Draw particles
 * -------------------------------------------------------------------------- */
void ParticleBox::draw()
{
    glPointSize( 5 );
    glShadeModel( GL_FLAT );
    glDisable( GL_LIGHTING );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glTranslatef( m_Position.x, m_Position.y, m_Position.z );
    glRotatef( m_Rotation.x, 1.0, 0.0, 0.0 );
    glRotatef( m_Rotation.y, 0.0, 1.0, 0.0 );
    glRotatef( m_Rotation.z, 0.0, 0.0, 1.0 );

    float dt = 0.001 * m_Timer.getDelta();

    glBegin( GL_POINTS );
    for( int i = 0; i < MAX_PARTICLES; i++ )
    {

        for( int j = 0; j < 3; j++ )
        {
            m_Particles[ i ].position[ j ] += dt * m_Particles[ i ].velocity[ j ];
            m_Particles[ i ].velocity[ j ] += dt * checkGravity( i, j ) / m_Particles[ i ].mass;

        }
        checkCollision( i );
        glColor4fv( m_Particles[ i ].color );
        glVertex4fv( m_Particles[ i ].position );

    }
    glEnd();

    glEnable( GL_LIGHTING );
    glShadeModel( GL_SMOOTH );

}

/* --------------------------------------------------------------------------
 *  2.7. Robot
 *
 *  See drawableobjects.h for class definition.
 * -------------------------------------------------------------------------- */


void CALLBACK errorCallback( GLenum errorCode )
{
    const GLubyte *estring;
    estring = gluErrorString( errorCode );
    fprintf( stderr, "Quadric Error: %s\n", estring );
    exit(0);
}

/* --------------------------------------------------------------------------
 *  2.7.1. Robot
 *
 *  creates an animated robot
 * -------------------------------------------------------------------------- */
Robot::Robot() :
    m_WheelRotation( 0 )
{
    createBody();
}

Robot::~Robot()
{
    delete m_Body[0];
    delete m_Body[1];
    delete m_Head;
}

/* --------------------------------------------------------------------------
 *  2.7.2. createBody
 *
 *  creates the Body of the robot
 * -------------------------------------------------------------------------- */
void Robot::createBody()
{
    /* Create quadric object for the wheel */
    m_Wheel = gluNewQuadric();
    gluQuadricCallback( m_Wheel, GLU_ERROR, (_GLUfuncptr)errorCallback );
    gluQuadricDrawStyle( m_Wheel, GLU_FILL );
    gluQuadricNormals( m_Wheel, GLU_SMOOTH );

    /* body of the robot */
    m_Body[ 0 ] = new Box( 1.0, 1.5, 1.2 );
    m_Body[ 1 ] = new Box( 0.3, 0.8, 0.2 );

    m_Head = new Box( 0.8, 1.0, 1.0 );
    m_Head->setColor( 100, 60, 60 );
}

/* --------------------------------------------------------------------------
 *  2.7.3. drawWheel
 *
 *  Draws a wheel of the robot. Rotation of the wheel can be specified.
 * -------------------------------------------------------------------------- */
void Robot::drawWheel( int direction ) //GLfloat rotation )
{

    GLfloat color[] = { 1.0, 1.0, 0.0, 1.0 };
    GLfloat shininess[] = { 30.0 };

    glShadeModel( GL_SMOOTH );
    glDisable( GL_LIGHTING );

    if( direction == STILL )
        glRotatef( m_WheelRotation, 0.0, 0.0, 1.0 );

    if( direction == FORWARD )
        glRotatef( m_WheelRotation+= 6, 0.0, 0.0, 1.0 );
    else if( direction == BACKWARD )
        glRotatef( m_WheelRotation-= 6, 0.0, 0.0, 1.0 );

    glPushMatrix();
        glTranslatef( 0.0, 0.0, 0.51 );
        glBegin( GL_QUADS );
            glColor3f( 0.4, 0.4, 0.6 );
            glVertex3f( -0.3, -0.3, 0 );
            glVertex3f( 0.3, -0.3, 0 );
            glVertex3f( 0.3, 0.3, 0 );
            glVertex3f( -0.3, 0.3, 0 );
        glEnd();
    glPopMatrix();

    glPushMatrix();
        glTranslatef( 0.0, 0.0, -0.01 );
        glRotatef( 180, 1.0, 0.0, 0.0 );
        glBegin( GL_QUADS );
            glColor3f( 0.4, 0.4, 0.6 );
            glVertex3f( -0.3, -0.3, 0 );
            glVertex3f( 0.3, -0.3, 0 );
            glVertex3f( 0.3, 0.3, 0 );
            glVertex3f( -0.3, 0.3, 0 );
        glEnd();
    glPopMatrix();

    glEnable( GL_LIGHTING );

    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color );
    glMaterialfv( GL_FRONT, GL_SHININESS, shininess );

    gluCylinder( m_Wheel, 0.5, 0.5, 0.5, 15, 5 );

    glPushMatrix();
        glRotatef( 180, 0.0, 1.0, 0.0 );
        gluDisk( m_Wheel, 0, 0.5, 15, 5 );
        glPopMatrix();
    glTranslatef( 0, 0, 0.5 );

    gluDisk( m_Wheel, 0, 0.5, 15, 5 );

}

/* --------------------------------------------------------------------------
 *  2.7.4. drawBody
 *
 *  Draws the body and head of the robot.
 * -------------------------------------------------------------------------- */
void Robot::drawBody()
{
    glPushMatrix();
    m_Head->draw( false );
    glPopMatrix();
    glPushMatrix();
    glTranslatef( 0.0, -1.25, 0.0 );
    m_Body[ 0 ]->draw( false );
    glTranslatef( 0.0, -1.15, -0.4 );
    m_Body[ 1 ]->draw( false );
    glPopMatrix();
}

/* --------------------------------------------------------------------------
 *  2.7.5. move
 *
 *  moves the robot
 * -------------------------------------------------------------------------- */
void Robot::move( int direction )
{
    /* Limit updating to ~30 fps. */
    if( m_Timer.getElapsed() < 33 ) return;
    m_Timer.reset();

    switch( direction )
    {
    case FORWARD:
        m_Position.z = m_Position.z + 0.2 * sin( m_Rotation.y * PI / 180 );
        m_Position.x = m_Position.x - 0.2 * cos( m_Rotation.y * PI / 180 );
        m_Direction = FORWARD;
        break;
    case BACKWARD:
        m_Position.z = m_Position.z - 0.2 * sin( m_Rotation.y * PI / 180 );
        m_Position.x = m_Position.x + 0.2 * cos( m_Rotation.y * PI / 180 );
        m_Direction = BACKWARD;
        break;
    case TURN_LEFT:
        rotate( 0, 4, 0 );
        if( m_Rotation.y > 359 )
            m_Rotation.y = m_Rotation.y - 360;
        m_Direction = STILL;
        break;
    case TURN_RIGHT:
        rotate( 0, -4, 0 );
        if( m_Rotation.y < 0 )
            m_Rotation.y = m_Rotation.y + 360;
        m_Direction = STILL;
        break;
    case TURN_HEAD_RIGHT:
        m_Head->rotate( 0, -4, 0 );
        m_Direction = STILL;
        break;
    case TURN_HEAD_LEFT:
        m_Head->rotate( 0, 4, 0 );
        m_Direction = STILL;
        break;
    }
}

/* --------------------------------------------------------------------------
 *  2.7.6. draw
 *
 *  draws the robot
 * -------------------------------------------------------------------------- */
void Robot::draw()
{

    glEnable( GL_LIGHTING );
    glShadeModel( GL_SMOOTH );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glTranslatef( m_Position.x, m_Position.y, m_Position.z );
    glRotatef( m_Rotation.x, 1.0, 0.0, 0.0 );
    glRotatef( m_Rotation.y, 0.0, 1.0, 0.0 );
    glRotatef( m_Rotation.z, 0.0, 0.0, 1.0 );
    glColor3f( 1, 0, 1 );

    drawBody();
    glTranslatef( 0.0, -2.75, -0.3 );
    drawWheel( m_Direction );
}

/* End of drawableobjects.cpp */
