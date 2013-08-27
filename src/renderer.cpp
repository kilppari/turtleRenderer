/* --------------------------------------------------------------------------
 * renderer.cpp
 *
 * Implementation for the Renderer and MaterialManager classes.
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
 * Version  : 1.1
 * Date     : 24-Apr-2011 12:15
 * Author   : Pekka Mäkinen
 * Comments :
 *  - Updates to MaterialManager
 * --------------------------------------------------------------------------
 * Version  : 1
 * Date     : 23-Apr-2011 13:14
 * Author   : Pekka Mäkinen
 * Comments :
 *  - File template updated. Starting version numbering from 1.0. Added
 *    implementation for MaterialManager.
 * -------------------------------------------------------------------------- */

/* **************************************************************************

    Contents

   ************************************************************************** */
/*
    Version history
    Contents
    1. Mandatory include files
    2. Function implementations
        2.1. Renderer
            2.1.1.  Renderer ( ctor & dtor )
            2.1.2.  attachObject
            2.1.3.  removeObject
            2.1.4.  clearAllObjects
            2.1.5.  initializeGL
            2.1.6.  resizeGL
            2.1.7.  paintGL
            2.1.8.  mousePressEvent
            2.1.9.  mouseReleaseEvent
            2.1.10. mouseMoveEvent
            2.1.11. keyPressEvent
            2.1.12. draw
            2.1.13. objecAtPosition
            2.1.14. changeObjectColor
            2.1.15. changeObjectPosition
        2.2. MaterialManager
            2.2.1.  MaterialManager ( ctor, copy-ctor, assignment op. & dtor )
            2.2.2.  getInstance
            2.2.3.  addMaterial
            2.2.4.  removeMaterial
            2.2.5.  clearAllMaterials
            2.2.6.  setValue
            2.2.7.  getMaterial
 */


/* **************************************************************************

    1. Mandatory include files

   ************************************************************************** */
#include "renderer.h"
#include <math.h>
#include <cassert>
#include "drawableobjects.h"
#include "GL/glu.h"

/* **************************************************************************

    2. Function implementations

   ************************************************************************** */

/* --------------------------------------------------------------------------
 *  2.1. Renderer
 *
 *  See renderer.h for more details about this class.
 * -------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------
 *  2.1.1. Renderer
 *         ~Renderer
 *
 *  Constructor and destructor for the Renderer.
 * -------------------------------------------------------------------------- */
Renderer::Renderer( QWidget *parent )
    : QGLWidget( parent ),
      m_pChosenObject( NULL ),
      m_ObjectDragOngoing( false )
{
    /* Specify OpenGL display context. */
    setFormat( QGLFormat( QGL::DoubleBuffer | QGL::DepthBuffer ) );
}

Renderer::~Renderer()
{
    /* Delete all objects from the drawable list */
    clearAllObjects();
}

/* --------------------------------------------------------------------------
 *  2.1.2. attachObject
 *
 *  Adds drawable object to the list of objects.
 * -------------------------------------------------------------------------- */
void Renderer::attachObject( IDrawable *object, bool isRobot )
{
    m_Objects.push_back( object );

    if( isRobot )
        m_pRobot = object;
}

/* --------------------------------------------------------------------------
 *  2.1.3. removeObject
 *
 *  Removes an object from the drawables list.
 * -------------------------------------------------------------------------- */
void Renderer::removeObject( IDrawable *object )
{
    m_Objects.remove( object );
    delete object;
}

/* --------------------------------------------------------------------------
 *  2.1.4. clearAllObjects
 *
 *  Clear the object list.
 * -------------------------------------------------------------------------- */
void Renderer::clearAllObjects()
{
    while( !m_Objects.empty() )
    {
        DrawableIterator it = m_Objects.begin();
        removeObject( *it );
    }
}

/* --------------------------------------------------------------------------
 *  2.1.5. initializeGL
 *
 *  Reimplementation from QGLWidget. Sets up rendering context.
 * -------------------------------------------------------------------------- */
void Renderer::initializeGL()
{
    /* Set clear color to black. */
    glClearColor( 0.0, 0.0, 0.0, 0.0 );

    /* Enable smooth shading. */
    glShadeModel( GL_SMOOTH );

    /* Enable depth comparison and updates to the depth buffer. */
    glEnable( GL_DEPTH_TEST );

    glEnable( GL_TEXTURE_2D );

    /* Enable culling to discard (in our case) back-facing polygons. */
    glEnable( GL_CULL_FACE );

    /* Ensure that normals are of unit length. */
    glEnable( GL_NORMALIZE );

    /* Load some textures */
    TextureManager *texMngrPtr;
    texMngrPtr = TextureManager::getInstance();
    texMngrPtr->createTexture( "Marble", QImage("marble.jpg") );
    texMngrPtr->createTexture( "Wall", QImage("brick_wall.jpg") );
    texMngrPtr->createTexture( "Whiteboard", QImage("whiteboard.jpg") );
    texMngrPtr->createTexture( "Floor", QImage("floor.jpg") );

    /* Enable lighting. */

    /* Positional white light */

    GLfloat pos[] = { 1.0, 3.0, 1.0, 1.0 };
    GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };

    glLightfv( GL_LIGHT0, GL_POSITION, pos );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, white );
    glLightfv( GL_LIGHT0, GL_SPECULAR, white );

    glLightf( GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.4 );
    glLightf( GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.01 );
    glLightf( GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01 );
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, ambient );
    glLightModeli( GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );

}

/* --------------------------------------------------------------------------
 *  2.1.6. resizeGL
 *
 *  Reimplementation from QGLWidget.
 *  Sets up viewport and projection. Function is called before paintGL is
 *  called the first time, but after initializeGL() is called. Also called
 *  whenever the widget is resized.
 * -------------------------------------------------------------------------- */
void Renderer::resizeGL( int width, int height )
{
    /* Set available screen area. Origin at 0,0. */
    glViewport( 0, 0, width, height );

    /* Select projection matrix and clear it so that only the subsequent
       transformations have effect. */
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    /* Calculate width to height ratio and specify perspective projection. */
    GLfloat x = GLfloat( width ) / height;
    glFrustum( -x, x, -1.0, 1.0, 4.0, 20.0 );
//    gluLookAt( 4, 1, 0, 1, 0, -12, 0, 1, 0 );
    /* Change back to modelview matrix. */
    glMatrixMode( GL_MODELVIEW );
    setFocus();
}

/* --------------------------------------------------------------------------
 *  2.1.7. paintGL
 *
 *  Reimplementation from QGLWidget.
 *  Clears buffers and draws the screen. Actual drawing done in draw().
 * -------------------------------------------------------------------------- */
void Renderer::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    draw();
}

/* --------------------------------------------------------------------------
 *  2.1.8. mousePressEvent
 *
 *  Reimplementation from QWidget.
 *  Save the position of mouse pressing event and check if an object is
 *  clicked. If drawable object is clicked, emits object's colorinfo, starts
 *  object dragging and sets keyboard focus to this widget.
 * -------------------------------------------------------------------------- */
void Renderer::mousePressEvent( QMouseEvent *event )
{
    int         objName;

    m_MouseLastPos = event->pos();

    /* Check if an object has been clicked. */

    if( ( objName = objectAtPosition( event->pos() ) ) != -1 )
    {
        DrawableIterator it  = m_Objects.begin();
        DrawableIterator end = m_Objects.end();

        IDrawable   *obj;
        while( it != end )
        {
            obj = *it;
            if( obj->getName() == objName )
            {
                m_pChosenObject = obj;

                emit objectRGB( obj->getColor().x, obj->getColor().y,
                                              obj->getColor().z );
                if( obj->isMovable() )
                    m_ObjectDragOngoing = true;

                emit locationChanged( obj->getPosition().x,
                                      obj->getPosition().y,
                                      obj->getPosition().z );
                setFocus();
                break;
            }
            ++it;
        }
    }

    setFocus();
//    updateGL();
}

/* --------------------------------------------------------------------------
 *  2.1.9. mouseReleaseEvent
 *
 *  Reimplementation from QWidget. Sets object dragging off.
 * -------------------------------------------------------------------------- */
void Renderer::mouseReleaseEvent( QMouseEvent * )
{
    m_ObjectDragOngoing = false;
}

/* --------------------------------------------------------------------------
 *  2.1.10. mouseMoveEvent
 *
 *  Reimplementation from QWidget.
 *  Calculates translation and rotation values for drawn objects when mouse
 *  buttons are pressed while moving the mouse.
 * -------------------------------------------------------------------------- */
void Renderer::mouseMoveEvent( QMouseEvent *event )
{
    GLint       nearClippingPlane = 4, objDist;
    GLfloat     rotX, rotY, posX, posY, posZ;
    GLfloat     whRatio, alpha, theta;
    GLfloat     objPlaneWidth, objPlaneHeight;
    GLfloat     dx = GLfloat( event->x() - m_MouseLastPos.x() ) / width();
    GLfloat     dy = GLfloat( event->y() - m_MouseLastPos.y() ) / height();

    /* Do nothing if no object has been chosen yet. */
    if( m_pChosenObject == NULL )
    {
        return;
    }

    /* Move object if it has been clicked and mouse button is
       still being pressed while moving the mouse. */
    if( event->buttons() & Qt::LeftButton )
    {
        if( m_ObjectDragOngoing )
        {
            /*
             * To make objects easily movable by mouse from any Z-position, we
             * need to calculate the width and height of the object's plane (The
             * plane that faces camera.) and multiply them with the normalized
             * delta of mouse movement to get the correct x- and y-movement
             * distance. See 'Perspective projection' in chapter 3 from the
             * 'OpenGL programming guide' ( 7th Edition ).
             *
             * Following code assumes that Perspective projection is spesified
             * as:
             *  Near clipping plane width = (screen width / screen height) * 2
             *  Near clipping plane height = 2
             */
            whRatio = ( GLfloat )width() / height();
            /* Alpha and theta are the x- and y-angles of the right triangles
             * of the field of vision. */
            alpha          = ( GLfloat )atan( whRatio / nearClippingPlane );
            theta          = ( GLfloat )atan( 1.0 / nearClippingPlane );
            objDist        = abs( m_pChosenObject->getPosition().z );
            objPlaneWidth  = 2.0 * objDist * tan( alpha );
            objPlaneHeight = 2.0 * objDist * tan( theta );

            posX = m_pChosenObject->getPosition().x + objPlaneWidth  * dx;
            posY = m_pChosenObject->getPosition().y - objPlaneHeight * dy;
            posZ = m_pChosenObject->getPosition().z;

            m_pChosenObject->setPosition( posX, posY, posZ );
            emit locationChanged( posX, posY, posZ );
            updateGL();
        }
    }
    /* Rotate object when right button is held. */
    else if( event->buttons() & Qt::RightButton )
    {
        if( m_pChosenObject->isRotatable() )
        {
            rotX = m_pChosenObject->getRotation().x + 180 * dy;
            rotY = m_pChosenObject->getRotation().y + 180 * dx;
            m_pChosenObject->setRotation( rotX, rotY, 0 );
            updateGL();
        }
    }

    m_MouseLastPos = event->pos();
}

/* --------------------------------------------------------------------------
 *  2.1.11. keyPressEvent
 *
 *  Reimplementation from QWidget.
 *  Handles custom key press events.
 * -------------------------------------------------------------------------- */
void Renderer::keyPressEvent( QKeyEvent *event )
{
    switch( event->key() )
    {
    case Qt::Key_Right:
        if( m_pChosenObject != NULL )
            m_pChosenObject->move( 0.1, 0.0, 0.0 );
        break;
    case Qt::Key_Left:
        if( m_pChosenObject != NULL )
            m_pChosenObject->move( -0.1, 0.0, 0.0 );
        break;
    case Qt::Key_Up:
        if( m_pChosenObject != NULL )
            m_pChosenObject->move( 0.0, 0.1, 0.0 );
        break;
    case Qt::Key_Down:
        if( m_pChosenObject != NULL )
            m_pChosenObject->move( 0.0, -0.1, 0.0 );
        break;
    case Qt::Key_Period:
        if( m_pChosenObject != NULL )
            m_pChosenObject->move( 0.0, 0.0, -0.1 );
        break;
    case Qt::Key_Comma:
        if( m_pChosenObject != NULL )
            m_pChosenObject->move( 0.0, 0.0, 0.1 );
        break;
    case Qt::Key_M:
        if( m_pChosenObject != NULL )
            m_pChosenObject->scale( 0.1, 0.1, 0.1 );
        break;
    case Qt::Key_N:
        if( m_pChosenObject != NULL )
            m_pChosenObject->scale( -0.1, -0.1, -0.1 );
        break;
    case Qt::Key_W:
        if( m_pRobot != NULL )
        {
            ( ( Robot* )m_pRobot )->move( Robot::FORWARD );
            break;
        }
    case Qt::Key_A:
        if( m_pRobot != NULL )
        {
            ( ( Robot* )m_pRobot )->move( Robot::TURN_LEFT );
            break;
        }
    case Qt::Key_S:
        if( m_pRobot != NULL )
        {
            ( ( Robot* )m_pRobot )->move( Robot::BACKWARD );
        }
        break;
    case Qt::Key_D:
        if( m_pRobot != NULL )
        {
            ( ( Robot* )m_pRobot )->move( Robot::TURN_RIGHT );
        }
        break;
    case Qt::Key_E:
        if( m_pRobot != NULL )
        {
            ( ( Robot* )m_pRobot )->move( Robot::TURN_HEAD_RIGHT );
        }
        break;
    case Qt::Key_Q:
        if( m_pRobot != NULL )
        {
            ( ( Robot* )m_pRobot )->move( Robot::TURN_HEAD_LEFT );
        }
        break;
    }
    if( m_pChosenObject != NULL )
        emit locationChanged( m_pChosenObject->getPosition() );
    updateGL();
}

/* --------------------------------------------------------------------------
 *  2.1.12. draw
 *
 *  Iterate through the list of drawable objects and draw each of them. Before
 *  an object is drawn, it is assosiated with a name to enable mouse hit
 *  detection.
 * -------------------------------------------------------------------------- */
void Renderer::draw()
{
    DrawableIterator it  = m_Objects.begin();
    DrawableIterator end = m_Objects.end();
    int i = 0;
    /* Pointer to an IDrawable, makes the code a bit more easier to read. */
    IDrawable *obj;

    while( it != end )
    {
        obj = (*it);

        if( obj->isMovable() || obj->isRotatable() )
        {
            glLoadName( i );
            obj->setName( i++ );
        }

        obj->draw();
        ++it;
    }
}

/* --------------------------------------------------------------------------
 *  2.1.13. objectAtPosition
 *
 *  Checks if a drawn object is under given position and returns that object's
 *  name. Used for selecting objects with the mouse.
 *
 * References: Pages 606-615 in 'OpenGL Programming guide' ( 7th Edition )
 *             Page 202 in 'C++ GUI programming with Qt4' ( 1st Edition )
 * -------------------------------------------------------------------------- */
int Renderer::objectAtPosition( const QPoint &pos )
{
    /* Data stuctures. */
    const int   maxSize = 512;
    GLuint      hitRecords[ maxSize ];
    GLint       viewport[4];
    GLfloat     whRatio;

    /* Get viewport boundaries. */
    glGetIntegerv( GL_VIEWPORT, viewport );

    /* Set array to be used for the returned hit records. */
    glSelectBuffer( maxSize, hitRecords );

    /* Enter selection mode. */
    glRenderMode( GL_SELECT );

    /* Initialize the name stack. */
    glInitNames();
    glPushName(0);

    /* Save current projection matrix. */
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();

    /* Restrict drawing to a small region of the viewport. */
    glLoadIdentity();
    gluPickMatrix( ( GLdouble )pos.x(), ( GLdouble )( viewport[3] - pos.y() ),
                   5, 5, viewport );
    /* Standard projection matrix. */
    whRatio = GLfloat( width() ) / height();
    glFrustum( -whRatio, whRatio, -1.0, 1.0, 4.0, 20.0 );

    /* Draw scene for picking. */
    draw();

    /* Restore saved projection matrix. */
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();

    /* Return -1 if selection array overflowed. */
    if( !glRenderMode( GL_RENDER ) )
        return -1;

    /* Return the first name from the records. */
    return hitRecords[3];
}

/* --------------------------------------------------------------------------
 *  2.1.14. changeObjectColor
 *
 *  Slot for changing an object's color.
 * -------------------------------------------------------------------------- */
void Renderer::changeObjectColor( int r, int g, int b )
{
    if( m_pChosenObject != NULL )
    {
        m_pChosenObject->setColor( r, g ,b );
        updateGL();
    }
}

/* --------------------------------------------------------------------------
 *  2.1.15. changeObjectPosition
 *
 *  Slot for changing an object's position.
 * -------------------------------------------------------------------------- */
void Renderer::changeObjectPosition( float x, float y, float z )
{
    if( m_pChosenObject != NULL )
    {
        m_pChosenObject->setPosition( (GLfloat)x, (GLfloat)y ,(GLfloat)z );
        updateGL();
    }
}

/* --------------------------------------------------------------------------
 *  2.2. MaterialManager
 *
 *  See renderer.h for more details about this class.
 * -------------------------------------------------------------------------- */

/* Static member */
MaterialManager *MaterialManager::m_pInstance = NULL;

/* --------------------------------------------------------------------------
 *  2.2.1. MaterialManager ( ctor, copy-ctor, assignment op. & dtor )
 *
 * -------------------------------------------------------------------------- */
MaterialManager::MaterialManager() {}
MaterialManager::MaterialManager( const MaterialManager & ) {}
MaterialManager &MaterialManager::operator=( const MaterialManager & ) {}

MaterialManager::~MaterialManager()
{}

/* --------------------------------------------------------------------------
 *  2.2.2. getInstance
 *
 *  Returns the one and only instance to MaterialManager object.
 * -------------------------------------------------------------------------- */
MaterialManager *MaterialManager::getInstance()
{
    if( !m_pInstance )
    {
        m_pInstance = new MaterialManager();
    }
    return m_pInstance;
}

/* --------------------------------------------------------------------------
 *  2.2.3. addMaterial
 *
 *  Adds a new material to the material map.
 * -------------------------------------------------------------------------- */
void MaterialManager::addMaterial( const char *name, const MaterialData &data )
{
    m_Materials.insert( MaterialPair( std::string( name ), data ) );
}

/* --------------------------------------------------------------------------
 *  2.2.4. removeMaterial
 *
 *  Removes a material from the material map.
 * -------------------------------------------------------------------------- */
void MaterialManager::removeMaterial( const char *name )
{
    m_Materials.erase( std::string( name ) );
}

void MaterialManager::removeMaterial( const std::string &name )
{
    m_Materials.erase( name );
}

/* --------------------------------------------------------------------------
 *  2.2.5. clearAllMaterials
 *
 *  Removes all materials from the material map.
 * -------------------------------------------------------------------------- */
void MaterialManager::clearAllMaterials()
{
    while( !m_Materials.empty() )
    {
        MaterialIterator it = m_Materials.begin();
        removeMaterial( (*it).first );
    }
}

/* --------------------------------------------------------------------------
 *  2.2.6. setValue
 *
 *  Sets an attribute value of a specified material. Material must exist in
 *  the material map.
 * -------------------------------------------------------------------------- */
void MaterialManager::setValue( const char *matName, MaterialAttribute attr,
                                const Color4f &color )
{
    MaterialIterator it;
    it = m_Materials.find( std::string( matName ) );
    if( it != m_Materials.end() )
    {
        switch( attr )
        {
        case MAT_AMBIENT:
            (*it).second.ambient = color;
            break;
        case MAT_DIFFUSE:
            (*it).second.diffuse = color;
            break;
        case MAT_SPECULAR:
            (*it).second.specular = color;
            break;
        case MAT_EMISSION:
            (*it).second.emission = color;
            break;
        case MAT_SHININESS:
            (*it).second.shininess = color.r;
        }
    }
}

/* --------------------------------------------------------------------------
 *  2.2.7. getMaterial
 *
 *  Returns a material data from the material map.
 *  Data returned is specified by material name.
 * -------------------------------------------------------------------------- */
const MaterialData &MaterialManager::getMaterial( const char * name )
{
    MaterialData dummy;
    MaterialIterator it = m_Materials.find(std::string( name ) );
    if( it != m_Materials.end() )
    {
        return (*it).second;
    }
    return dummy;
}


/* --------------------------------------------------------------------------
 *  2.3. TextureManager
 *
 *  See renderer.h for more details about this class.
 * -------------------------------------------------------------------------- */

/* Static member */
TextureManager *TextureManager::m_pInstance = NULL;

/* --------------------------------------------------------------------------
 *  2.3.1. TextureManager ( ctor, copy-ctor, assignment op. & dtor )
 *
 * -------------------------------------------------------------------------- */
TextureManager::TextureManager() {}
TextureManager::TextureManager( const TextureManager & ) {}
TextureManager &TextureManager::operator=( const TextureManager & ) {}


/* --------------------------------------------------------------------------
 *  2.3.2. getInstance
 *
 *  Returns the one and only instance to TextureManager object.
 * -------------------------------------------------------------------------- */
TextureManager *TextureManager::getInstance()
{
    if( !m_pInstance )
    {
        m_pInstance = new TextureManager();
    }
    return m_pInstance;
}

GLuint TextureManager::bindTexture( const QPixmap &pixmap )
{
    return m_pRenderer->bindTexture( pixmap, GL_TEXTURE_2D );
}

GLuint TextureManager::createTexture( const char *name, const QImage &image )
{
    GLuint textureId;

    assert( !image.isNull() );

    glEnable( GL_TEXTURE_2D );
    glGenTextures( 1, &textureId );

    /* Create a new texture object and assign the id to it. */
    glBindTexture( GL_TEXTURE_2D, textureId );

    /* Assign filters. */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    /* Define 2D texture */
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(),
                  0, GL_BGRA, GL_UNSIGNED_BYTE, image.bits() );

    Texture tex;
    tex.id        = textureId;
    tex.height    = image.height();
    tex.width     = image.width();

    m_Textures.insert( std::pair< QString, Texture >( QString( name ), tex ) );

    glDisable( GL_TEXTURE_2D );
    return textureId;
}

const Texture *TextureManager::getTexturePtr( const QString &texName )
{
    return &m_Textures[ texName ];
}

/* End of renderer.cpp */
