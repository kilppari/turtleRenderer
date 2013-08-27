/* --------------------------------------------------------------------------
 *
 * drawableobjects.h
 *
 * Specifies the objects that can be drawn by the Renderer class. All drawable
 * objects must inherit from IDrawable in order for Renderer to be able to draw
 * them.
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
 * Date     : 10-Jun-2011 21:08
 * Author   : Pekka Mäkinen
 * Comments :
 *  - Added ParticleBox and Robot -drawables.
 * --------------------------------------------------------------------------
 * Version  : 1.0
 * Date     : 3-May-2011 19:08
 * Author   : Pekka Mäkinen
 * Comments :
 *  - File template updated. Starting version numbering from 1.0.
 *    Notable changes:
 *    Added a new class that supports WaveFront objects and a class to
 *    demonstrate DDA algorithm, Bresenham's circle algorithm and
 *    supersampling.
 * -------------------------------------------------------------------------- */

#ifndef DRAWABLEOBJECTS_H
#define DRAWABLEOBJECTS_H

/* **************************************************************************

    Contents

   ************************************************************************** */
/*
    Version history
    Contents
    1. Mandatory include files
    2. Data structures
        2.1. < ? >
    3. Interfaces
        3.1. < ? >
    4. Classes
        4.1. BaseDrawable
        4.2. Box
        4.3. Plane
        4.4. WFObject
        4.5. RasterMap
        4.6. ParticleBox
        4.7. Robot
 */


/* **************************************************************************

    1. Mandatory include files

   ************************************************************************** */

#include "renderer.h"
#include "timer.h"
#include <GL/glut.h>
/* **************************************************************************

    2. Data structures

   ************************************************************************** */
/* --------------------------------------------------------------------------
 *  2.1. Particle
 * -------------------------------------------------------------------------- */
struct Particle
{
    float	position[4];
    float	velocity[4];
    float	color[4];
    float	mass;
    float   dummy[3];
};

const int MAX_PARTICLES = 50;

/* **************************************************************************

    3. Interfaces

   ************************************************************************** */
/* --------------------------------------------------------------------------
 *  3.1. < ? >
 * -------------------------------------------------------------------------- */

/* **************************************************************************

    4. Classes

   ************************************************************************** */

/* --------------------------------------------------------------------------
 *  4.1. BaseDrawable
 *
 *  Abstract base class that inherits from IDrawable. Implements all the set
 *  and get functions from the IDrawable so that succeeding child classes must
 *  only implement the method draw().
 * -------------------------------------------------------------------------- */
class BaseDrawable : public IDrawable
{
protected:
    Vector3f    m_Position;
    Vector3f    m_Rotation;
    Vector3f    m_Color;
    Vector3f    m_Scaling;
    int         m_Name;
    bool        m_IsMovable;
    bool        m_IsRotatable;
    Vector3f    m_Origin;

public:
    BaseDrawable();
    virtual ~BaseDrawable() {}

    /* Basic setters and getters.
     */
    virtual void        setMovable( bool state ) { m_IsMovable = state; }
    virtual bool        isMovable() { return m_IsMovable; }
    virtual void        setRotatable( bool state ) { m_IsRotatable = state; }
    virtual bool        isRotatable() { return m_IsRotatable; }
    virtual void        setPosition( const Vector3f &pos ) { m_Position = pos; }
    virtual void        setPosition( GLfloat x, GLfloat y, GLfloat z )
                                   { m_Position = Vector3f( x, y, z ); }
    virtual Vector3f    getPosition() { return m_Position; }

    virtual void        setRotation( const Vector3f &rot ) { m_Rotation = rot; }
    virtual void        setRotation( GLfloat x, GLfloat y, GLfloat z )
                                   { m_Rotation = Vector3f( x, y, z ); }
    virtual void        rotate( GLfloat x, GLfloat y, GLfloat z )
                                   { m_Rotation = Vector3f( m_Rotation.x+x,
                                     m_Rotation.y+y, m_Rotation.z+z ); }
    virtual Vector3f    getRotation() { return m_Rotation; }

    virtual void        setOrigin( GLfloat x, GLfloat y, GLfloat z )
                                   { m_Origin = Vector3f( x, y, z ); }
    virtual void        setOrigin( const Vector3f &pos ) { m_Origin = pos; }
    virtual Vector3f    getOrigin() { return m_Origin; }

    virtual void        setScaling( const Vector3f &scale );
    virtual void        setScaling( GLfloat x, GLfloat y, GLfloat z );
    virtual void        scale( GLfloat x, GLfloat y, GLfloat z );

    virtual void        setName( int name ) { m_Name = name; }
    virtual int         getName() { return m_Name; }

    virtual void        move( GLfloat x, GLfloat y, GLfloat z );

    virtual void        setColor( int r, int g, int b )
                                { m_Color = Vector3f( r, g, b ); }
    virtual Vector3f    getColor() { return m_Color; }

    /* Left for inheriting classes to implement. */
    virtual void draw() = 0;

protected:
    virtual void checkBounds();
};

inline void BaseDrawable::move( GLfloat x, GLfloat y, GLfloat z )
{
    m_Position.x += x;
    m_Position.y += y;
    m_Position.z += z;
}

/* --------------------------------------------------------------------------
 *  4.2. Box
 *
 *  A box model that can be created by defining its width, height and depth.
 * -------------------------------------------------------------------------- */
class Box : public BaseDrawable
{
private:
    GLfloat     m_Vertices[ 3 * 8 ];
    GLfloat     m_Normals[ 3 * 6 ];
    GLubyte     m_Indices[ 6 ][ 4 ];
    GLsizei     m_Count[ 6 ];

public:
    explicit Box( GLfloat w, GLfloat h, GLfloat d );
    void draw();
    void draw( bool loadIdentity, bool wireframe = false );
};

/* --------------------------------------------------------------------------
 *  4.3. Plane
 *
 *  A plane model. Defined by width and height.
 * -------------------------------------------------------------------------- */
class Plane : public BaseDrawable
{
public:
    GLuint      m_Texture;
    GLubyte     m_CheckImage[ 64 ][ 64 ][ 4 ];
private:
    GLfloat     m_Vertices[ 12 ];



public:
    explicit Plane( GLfloat w, GLfloat h );
    void draw();
};

/* --------------------------------------------------------------------------
 *  4.4. WFObject
 *
 *  Supports imported WaveFront files.
 * -------------------------------------------------------------------------- */
class WFObject : public BaseDrawable
{
private:
    ModelData           m_ModelData;
    MaterialData        m_MaterialData;
    std::string         m_TextureName;

public:
    WFObject( const char *filename );
    void draw();
    bool setMaterial( const char *name );
    void setTexture( const char *name );
};

/* --------------------------------------------------------------------------
 *  4.5. RasterMap
 *
 *  Demonstrates DDA algorith, Bresenham's circle algorithm and anti aliasing.
 * -------------------------------------------------------------------------- */
class RasterMap : public BaseDrawable
{
private:
    /* Rastermaps width and height */
    int         m_GridWidth;
    int         m_GridHeight;
    /* Pixel's width in the map */
    GLfloat     m_PixelSize;

    /* Current color in use. */
    Color4f     m_Color;
    /* Pointer for dynamically creating rastermap. */
    Color4f     **m_RasterMap;

    void drawPixel( int x, int y, const Color4f &color );
    void superSample( int x, int y );
    void setColor( const Color4f &color ) { m_Color = color; }
    /* Draws a line using DDA-algorithm. */
    void drawLine( int x0, int y0, int x1, int y1 );
    /* Drawn a circle using Bresenham's circle algorithm. */
    void drawCircle( int x, int y, int R );

public:
    RasterMap( int width, int height, GLfloat pixelSize );
    ~RasterMap();
    void draw();
};

/* --------------------------------------------------------------------------
 *  4.6. ParticleBox
 *
 *  Simple particle system to demonstrate inelastic and elastic collisions.
 * -------------------------------------------------------------------------- */
class ParticleBox : public BaseDrawable
{
private:
    Particle            m_Particles[ MAX_PARTICLES ];
    GLuint              m_Buffers[ 1 ];
    Timer               m_Timer;
    float               m_SideLength;
    float               m_Coef;
    bool                m_GravityEnabled;

public:
    ParticleBox( float sideLength, bool gravity, float coef );
    void draw();

private:
    float checkGravity( int i, int j );
    void  checkCollision( int i );
    GLfloat getBoxPos( int j );
};

#ifndef CALLBACK
#define CALLBACK
#endif
void CALLBACK errorCallback( GLenum errorCode );

/* --------------------------------------------------------------------------
 *  4.7. Robot
 *
 *  Robot model which demonstrates hiearchical kinematics.
 * -------------------------------------------------------------------------- */
class Robot : public BaseDrawable
{
private:

    Box *m_Body[ 2 ];
    Box *m_Head;
    Timer m_Timer;
    GLUquadricObj       *m_Wheel;
    float               m_WheelRotation;
    int                 m_Direction;

public:
    Robot();
    ~Robot();

    enum { FORWARD, BACKWARD, TURN_LEFT, TURN_RIGHT, STILL, TURN_HEAD_LEFT,
           TURN_HEAD_RIGHT };

    void move( int direction );

private:
    void createBody();
    void drawWheel( int direction );
    void drawBody();
    void draw();

};



#endif /* DRAWABLEOBJECTS_H */
