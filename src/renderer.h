/* --------------------------------------------------------------------------
 * renderer.h
 *
 * Creates a rendering context for OpenGL and offers interaction with the
 * drawn objects.
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
 * Version  : 1.0
 * Date     : 23-Apr-2011 13:14
 * Author   : Pekka Mäkinen
 * Comments :
 *  - File template updated. Starting version numbering from 1.0. Added
 *    ModelData and MaterialData structs and a new class: MaterialManager.
 * -------------------------------------------------------------------------- */

#ifndef RENDERER_H
#define RENDERER_H


/* **************************************************************************

    Contents

   ************************************************************************** */
/*
    Version history
    Contents
    1. Mandatory include files
    2. Data structures
        2.1. Vector3f
        2.2. Color4f
        2.3. ModelData
        2.4. MaterialData
        2.5. Texture
    3. Interfaces
        3.1. IDrawable
    4. Classes
        4.1. Renderer
        4.2. MaterialManager
 */


/* **************************************************************************

    1. Mandatory include files

   ************************************************************************** */

#include <QGLWidget>
#include <QMouseEvent>
#include <list>


/* **************************************************************************

    2. Data structures

   ************************************************************************** */

/* --------------------------------------------------------------------------
 *  2.1. Vector3f
 *
 *  Structure for holding three GLfloats.
 * -------------------------------------------------------------------------- */
struct Vector3f
{
    GLfloat x, y, z;

    Vector3f() : x( 0 ), y( 0 ), z( 0 ) {}
    Vector3f( GLfloat _x, GLfloat _y, GLfloat _z ) :
        x( _x ), y( _y ), z( _z ) {}
};

/* --------------------------------------------------------------------------
 *  2.2. Color4f
 *
 *  Structure for holding four GLfloats.
 *  r (red), g (green), b (blue) and a (alpha).
 * -------------------------------------------------------------------------- */
struct Color4f
{
    GLfloat r, g, b, a;
    Color4f() : r( 0 ), g ( 0 ), b( 0 ), a( 1.0 ) {}
    Color4f( GLfloat _r, GLfloat _g, GLfloat _b, GLfloat _a ) :
        r( _r ), g( _g ), b( _b ), a( _a ) {}
};

/* --------------------------------------------------------------------------
 *  2.3. ModelData
 *
 *  Stores the geometric information ( vertices, normals, texture coords )
 *  about renderable objects.
 * -------------------------------------------------------------------------- */
struct ModelData
{
    std::vector< Vector3f >     vertices;
    std::vector< Vector3f >     normals;
    std::vector< Vector3f >     textureCoords;
    std::vector< int >          vertexFaces;
    std::vector< int >          normalFaces;
    std::vector< int >          textureFaces;
    bool                        isSmoothShaded;

    //TODO: Method for clearing all vectors
};

/* --------------------------------------------------------------------------
 *  2.4. MaterialData
 *
 * Stores the material information of renderable objects.
 * ( e.g. ambient reflection )
 * -------------------------------------------------------------------------- */
struct MaterialData
{
    Color4f             ambient;
    Color4f             diffuse;
    Color4f             specular;
    Color4f             emission;
    GLfloat             shininess;
};

/* --------------------------------------------------------------------------
 *  2.5. Texture
 *
 * Stores Texture information
 * -------------------------------------------------------------------------- */
struct Texture
{
    GLuint      id;
    GLuint      width;
    GLuint      height;
};

/* **************************************************************************

    3. Interfaces

   ************************************************************************** */

/* --------------------------------------------------------------------------
 *  3.1. IDrawable
 *
 *  Interface for objects that can be drawn by the Renderer class.
 * -------------------------------------------------------------------------- */
class IDrawable
{
public:
    virtual ~IDrawable() {}

    /* Setter and getters. */
    virtual void        setMovable( bool state )                        = 0;
    virtual bool        isMovable()                                     = 0;
    virtual void        setRotatable( bool state )                      = 0;
    virtual bool        isRotatable()                                   = 0;
    virtual void        setPosition( const Vector3f &pos )              = 0;
    virtual void        setPosition( GLfloat x, GLfloat y, GLfloat z )  = 0;
    virtual Vector3f    getPosition()                                   = 0;
    virtual void        setRotation( const Vector3f &rot )              = 0;
    virtual void        setRotation( GLfloat x, GLfloat y, GLfloat z )  = 0;
    virtual Vector3f    getRotation()                                   = 0;
    virtual void        setScaling( const Vector3f &scale )             = 0;
    virtual void        setScaling( GLfloat x, GLfloat y, GLfloat z )   = 0;
    virtual void        scale( GLfloat x, GLfloat y, GLfloat z )        = 0;
    virtual void        setName( int name )                             = 0;
    virtual int         getName()                                       = 0;
    virtual void        move( GLfloat x, GLfloat y, GLfloat z )         = 0;
    virtual void        setColor( int r, int g, int b )                 = 0;
    virtual Vector3f    getColor()                                      = 0;

    /* draw() is called by the Renderer, and is expected to implement
     * necessary OpenGL commands to draw a required object. */
    virtual void draw()                                                 = 0;
};


/* **************************************************************************

    4. Classes

   ************************************************************************** */

/* --------------------------------------------------------------------------
 *  4.1. Renderer
 *
 *  Widget for OpenGL rendering context. Manages drawable objects, calculates
 *  positioning and rotation and draws each object.
 * -------------------------------------------------------------------------- */
class Renderer : public QGLWidget
{
    Q_OBJECT

private:

    /* Some typedefs to make code cleaner. */
    typedef std::list< IDrawable* >             DrawableList;
    typedef DrawableList::iterator              DrawableIterator;

    /* Last saved position of mousepointer. */
    QPoint              m_MouseLastPos;

    /* List of drawable object pointers. */
    DrawableList        m_Objects;

    /* Currently chosen object from the DrawableList. */
    IDrawable           *m_pChosenObject;

    IDrawable           *m_pRobot;

    /* True if an object has been clicked but no mouse
       release event have been received yet. */
    bool                m_ObjectDragOngoing;

public:
    Renderer( QWidget *parent = 0 );
    ~Renderer();
    void attachObject( IDrawable *object, bool isRobot = false );
    void removeObject( IDrawable *object );
    void clearAllObjects();

protected:
    /* Reimplementations from QGLWidget. */
    void initializeGL();
    void resizeGL( int width, int height );
    void paintGL();

    /* Reimplementations from QWidget. */
    void mousePressEvent( QMouseEvent *event );
    void mouseReleaseEvent( QMouseEvent *event );
    void mouseMoveEvent( QMouseEvent *event );
    void keyPressEvent( QKeyEvent *event );

private:
    /* Draws each object in m_Objects. */
    void draw();

    /* Check if an object lies in spesific position on screen. */
    int objectAtPosition( const QPoint &pos );

signals:
    /* Signals for sending object's color and position. */
    void objectRGB( int r, int g, int b );
    void locationChanged( float x, float y, float z );
    void locationChanged( const Vector3f &pos );

public slots:
    void changeObjectColor( int r, int g, int b );
    void changeObjectPosition( float x, float y, float z );

};


/* --------------------------------------------------------------------------
 *  4.2. MaterialManager
 *
 *  Singleton class for keeping track of all material information.
 * -------------------------------------------------------------------------- */

class MaterialManager
{
private:
    typedef std::pair< std::string, MaterialData >  MaterialPair;
    typedef std::map< std::string, MaterialData >   MaterialMap;
    typedef MaterialMap::iterator                   MaterialIterator;

    MaterialMap                 m_Materials;

    /* Handles own static pointer. */
    static MaterialManager      *m_pInstance;

    /* Prevent outside calling of ctor, copy-ctor and assignment operator. */
    MaterialManager();
    MaterialManager( const MaterialManager & );
    MaterialManager& operator=( const MaterialManager & );

public:
    enum MaterialAttribute { MAT_AMBIENT, MAT_DIFFUSE, MAT_SPECULAR,
                             MAT_EMISSION, MAT_SHININESS };

    ~MaterialManager();
    static              MaterialManager* getInstance();
//    void        addMaterial( const std::string &name, const MaterialData &data );
    void            addMaterial( const char *name, const MaterialData &data );
    void            removeMaterial( const std::string &name );
    void            removeMaterial( const char *name );
    void            clearAllMaterials();
    void            setValue( const char *matName, MaterialAttribute attr,
                              const Color4f &color );
    const MaterialData    &getMaterial( const char *name );
};

/* --------------------------------------------------------------------------
 *  4.3. TextureManager
 *
 *  Singleton class for keeping track of all textures
 * -------------------------------------------------------------------------- */
class TextureManager
{
public:
    typedef std::map< QString, Texture >        TextureMap;
    typedef TextureMap::iterator                TexMapIt;

private:
    /* Handles own static pointer. */
    static TextureManager       *m_pInstance;
    TextureMap                  m_Textures;

    Renderer                    *m_pRenderer;

    /* Prevent outside calling of ctor, copy-ctor and assignment operator. */
    TextureManager();
    TextureManager( const TextureManager & );
    TextureManager& operator=( const TextureManager & );


public:
    static TextureManager* getInstance();
    GLuint      createTexture( const char *name, const QImage &image );
    const Texture *getTexturePtr( const QString &texName );
    void        setRenderer( Renderer *pRenderer ) { m_pRenderer = pRenderer; }
    GLuint      bindTexture( const QPixmap &pixmap );
};
/* -------------------------------------------------------------------------- */
#endif /* RENDERER_H */

