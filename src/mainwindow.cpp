/* --------------------------------------------------------------------------
 * mainwindow.cpp
 *
 * Implementation of the mainwindow class.
 *
 * 2011 (C) Pekka Mäkinen
 *
 * -------------------------------------------------------------------------- */

/* Includes */
#include "mainwindow.h"
#include "renderer.h"
#include "drawableobjects.h"
#include <QAction>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QHBoxLayout>
#include <QSlider>
#include <QSpinBox>
#include <QDockWidget>
#include <QMessageBox>
#include <cassert>

/* --------------------------------------------------------------------------
 * MainWindow
 *
 * Constructor for MainWindow. Creates the GUI.
 * -------------------------------------------------------------------------- */
MainWindow::MainWindow()
{
    /* Set renderer widget to be the central widget of our program. */
    m_pRenderer = new Renderer;
    setCentralWidget( m_pRenderer );

    createScene();

    /* Create user interface. */
    createActions();
    createMenus();
//    createDockWindows();
    createStatusBar();
}

/* --------------------------------------------------------------------------
 * MainWindow::createActions
 *
 * Create actions and connect signals.
 * -------------------------------------------------------------------------- */
void MainWindow::createActions()
{
    /* Exit action for shutting down the application. */
    m_Action["Quit"] = new QAction( tr( "&Quit" ), this );
    m_Action["Quit"]->setShortcut( tr( "Ctrl+Q" ) );
    m_Action["Quit"]->setStatusTip( tr( "Exit the application" ) );
    connect( m_Action["Quit"], SIGNAL( triggered() ), this, SLOT( close() ) );

    /* Toggleaction for showing/hiding the dock window. */
/*
    m_Action["ToggleObjPanel"] = new QAction( tr( "Color Picker" ), this );
    m_Action["ToggleObjPanel"]->setStatusTip( tr( "Show/hide color picker" ) );
*/
    m_Action["Help"] = new QAction( tr( "&Help" ), this );
    m_Action["Help"]->setShortcut( tr( "Ctrl+H" ) );
    m_Action["Help"]->setStatusTip( tr( "Information about commands." ) );
    connect( m_Action["Help"], SIGNAL( triggered() ), this, SLOT( help() ) );

}

/* --------------------------------------------------------------------------
 * MainWindow::createMenus
 *
 * Create application menus.
 * -------------------------------------------------------------------------- */
void MainWindow::createMenus()
{
    m_Menu["File"] = menuBar()->addMenu( tr( "&File" ) );
    m_Menu["File"]->addAction( m_Action["Quit"] );

//    m_Menu["View"] = menuBar()->addMenu( tr( "&View" ) );
//    m_Menu["View"]->addAction( m_Action["ToggleObjPanel"] );

    m_Menu["Help"] = menuBar()->addMenu( tr( "&Help" ) );
    m_Menu["Help"]->addAction( m_Action["Help"] );
}

/* --------------------------------------------------------------------------
 * MainWindow::createDockWindows
 *
 * Create dock window. This will act as a specific interface for
 * interacting with the renderer.
 * -------------------------------------------------------------------------- */
void MainWindow::createDockWindows()
{
    m_pDock = new QDockWidget( tr( "Color Picker" ) );
    ObjectPanel *objPanel = new ObjectPanel( m_pRenderer, m_pDock );
    m_pDock->setWidget( objPanel );
    m_pDock->setAllowedAreas( Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea );
    addDockWidget( Qt::BottomDockWidgetArea, m_pDock );

    connect( m_Action["ToggleObjPanel"], SIGNAL( triggered() ), this, SLOT( toggleDock() ) );
}

/* --------------------------------------------------------------------------
 * MainWindow::createStatusBar
 *
 * Create status bar for showing coordinates of the rendered
 * objects in Renderer widget.
 * -------------------------------------------------------------------------- */
void MainWindow::createStatusBar()
{
    /* Make label for the coordinates, add it to status bar and
     * connect it with an update signal. */
    m_pCoordLabel = new QLabel( "00, 00, 00" );
    m_pCoordLabel->setAlignment( Qt::AlignCenter );
    m_pCoordLabel->setFixedWidth( 150 );

    statusBar()->addWidget( m_pCoordLabel );

    connect( m_pRenderer, SIGNAL( locationChanged( float, float, float ) ),
             this, SLOT( updateStatusBar( float, float, float) ) );
    connect( m_pRenderer, SIGNAL( locationChanged( const Vector3f & ) ),
             this, SLOT( updateStatusBar( const Vector3f & ) ) );
    updateStatusBar( 0, 0, 0 );
}

void MainWindow::createScene() //Renderer()
{

    RasterMap *dda = new RasterMap( 40, 30, 0.1 );
    dda->setPosition( -0.3, -1, -15 );
    m_pRenderer->attachObject( dda );

    Robot *robot = new Robot();
    robot->setMovable( true );
    robot->setPosition( 0, 2, -13 );
    m_pRenderer->attachObject( robot, true );

    WFObject *wf1 = new WFObject( "bowl.obj" );
    wf1->setMaterial( "Marble" );
    wf1->setTexture( "Marble" );
    wf1->setPosition( -1, -2.4, -14.0 );
    wf1->setMovable( true );
    wf1->setRotatable( true );
    wf1->setRotation( 0, 0, 0 );
    m_pRenderer->attachObject( wf1 );

/*
    ParticleBox *ElasticPBox = new ParticleBox( 1.5, false, 0.9 );
    ElasticPBox->setPosition( 3, -1, -13 );
    m_pRenderer->attachObject( ElasticPBox );

    ParticleBox *InelasticPBox = new ParticleBox( 1.5, true, 0.9 );
    InelasticPBox->setPosition( 1, -1, -13 );
    m_pRenderer->attachObject( InelasticPBox );
*/
    WFObject *wf2 = new WFObject( "brickwall.obj" );
    wf2->setMaterial( "Wall" );
    wf2->setTexture( "Wall" );
    wf2->setPosition( 3.5, -2.0, -15.0 );
    m_pRenderer->attachObject( wf2 );

    WFObject *whiteboard = new WFObject( "whiteboard.obj" );
    whiteboard->setMaterial( "Material" );
    whiteboard->setTexture( "Whiteboard" );
    whiteboard->setPosition( 0, -0.5, -15.1 );
    m_pRenderer->attachObject( whiteboard );

    WFObject *floor = new WFObject( "floor.obj" );
    floor->setMaterial( "Floor" );
    floor->setTexture( "Floor" );
    floor->setPosition( 1.5, -2.5, -14.0 );
    m_pRenderer->attachObject( floor );
}

/* --------------------------------------------------------------------------
 * MainWindow::help ( SLOT )
 *
 * Displays a help-dialog
 * -------------------------------------------------------------------------- */
void MainWindow::help()
{
    QMessageBox::about( this, tr( "Help" ),
                        tr( "<h2>TurtleRenderer</h2>"
                            "<p>2011 (C) Pekka Mäkinen</p>"
                            "Commands:<br/>"
                            "Robot movement: a and d keys: rotate robot.<br/>"
                            " w and s keys: Move forward or backward.<br/>"
                            " q and e keys: rotate robot's head left or right.<br/>"
                            "<br/>Mouse can be also used to drag and move objects." ) );
}

/* --------------------------------------------------------------------------
 * MainWindow::updateStatusBar ( SLOT )
 *
 * Updates status bar with object coordinates.
 * -------------------------------------------------------------------------- */
void MainWindow::updateStatusBar( float x, float y, float z )
{
    m_pCoordLabel->setText( QString( "%1, %2, %3" ).arg( x, 0, 'f', 3 )
                            .arg( y, 0, 'f', 3 ). arg( z, 0, 'f', 3 ) );
}
void MainWindow::updateStatusBar( const Vector3f &pos )
{
    updateStatusBar( pos.x, pos.y, pos.z );
}

/* --------------------------------------------------------------------------
 * MainWindow::toggleDock
 *
 * Switches dock window on/off.
 * -------------------------------------------------------------------------- */
void MainWindow::toggleDock()
{
    if( m_pDock->isVisible() )
        m_pDock->close();
    else
        m_pDock->show();
}

/* --------------------------------------------------------------------------
 * ObjectPanel
 *
 * Constructor for ObjectPanel
 * -------------------------------------------------------------------------- */
ObjectPanel::ObjectPanel( Renderer *renderer, QWidget *parent )
    : QWidget( parent )
{
    m_pRenderer = renderer;
    createColorChooser();
}

/* --------------------------------------------------------------------------
 * setRGBValues SLOT
 *
 * Set values for each color slider. This slot is activated by the Renderer.
 * -------------------------------------------------------------------------- */
void ObjectPanel::setRGBValues( int r, int g, int b )
{
    m_pSliderR->setValue( r );
    m_pSliderG->setValue( g );
    m_pSliderB->setValue( b );
}
/* --------------------------------------------------------------------------
 * sendRGBValues SLOT
 *
 * Emits colorChanged signal with values of each color's slider. Only slider
 * values need to be checked, as they will be automaticaly updated even if
 * user only uses the spinboxes for setting color values.
 * -------------------------------------------------------------------------- */
void ObjectPanel::sendRGBValues()
{
    emit colorChanged( m_pSliderR->value(), m_pSliderG->value(),
                       m_pSliderB->value() );
}

/* --------------------------------------------------------------------------
 * createColorChooser
 *
 * Creates a simple widget for choosing an RGB color value.
 * -------------------------------------------------------------------------- */
void ObjectPanel::createColorChooser()
{
    QLabel   *labelR   = new QLabel( tr( "Red" ) );
    QLabel   *labelG   = new QLabel( tr( "Green" ) );
    QLabel   *labelB   = new QLabel( tr( "Blue" ) );
    QSpinBox *spinBoxR = new QSpinBox;
    QSpinBox *spinBoxG = new QSpinBox;
    QSpinBox *spinBoxB = new QSpinBox;
    m_pSliderR         = new QSlider( Qt::Horizontal );
    m_pSliderG         = new QSlider( Qt::Horizontal );
    m_pSliderB         = new QSlider( Qt::Horizontal );


    labelR->setFixedWidth( 40 );
    labelG->setFixedWidth( 40 );
    labelB->setFixedWidth( 40 );
    spinBoxR->setRange( 0, 255 );
    spinBoxG->setRange( 0, 255 );
    spinBoxB->setRange( 0, 255 );
    m_pSliderR->setRange( 0, 255 );
    m_pSliderG->setRange( 0, 255 );
    m_pSliderB->setRange( 0, 255 );

    /* Connect status changes of spinboxes to corresponding
     * sliders and vice versa. */
    connect( spinBoxR, SIGNAL( valueChanged( int ) ),
             m_pSliderR, SLOT( setValue( int ) ) );
    connect( m_pSliderR, SIGNAL( valueChanged( int ) ),
             spinBoxR, SLOT( setValue( int ) ) );

    connect( spinBoxG, SIGNAL( valueChanged( int ) ),
             m_pSliderG, SLOT( setValue( int ) ) );
    connect( m_pSliderG, SIGNAL( valueChanged( int ) ),
             spinBoxG, SLOT( setValue( int ) ) );

    connect( spinBoxB, SIGNAL( valueChanged( int ) ),
             m_pSliderB, SLOT( setValue( int ) ) );
    connect( m_pSliderB, SIGNAL( valueChanged( int ) ),
             spinBoxB, SLOT( setValue( int ) ) );

    /* Connect the color change signal sent by Renderer. (to this) */
    connect( m_pRenderer, SIGNAL( objectRGB( int, int, int ) ),
             this, SLOT( setRGBValues( int, int, int ) ) );

    /* Connect the color change signal sent by this. (to the renderer) */
    connect( this, SIGNAL( colorChanged( int, int, int) ),
             m_pRenderer, SLOT( changeObjectColor( int, int, int ) ) );

    /* Call sendRGBValues() if any of the sliders change state. */
    connect( m_pSliderR, SIGNAL( valueChanged( int ) ),
             this, SLOT( sendRGBValues() ) );
    connect( m_pSliderG, SIGNAL( valueChanged( int ) ),
             this, SLOT( sendRGBValues() ) );
    connect( m_pSliderB, SIGNAL( valueChanged( int ) ),
             this, SLOT( sendRGBValues() ) );

    spinBoxR->setValue( 0 );
    spinBoxG->setValue( 0 );
    spinBoxB->setValue( 0 );

    /* Create layout for the widget. */
    QHBoxLayout *layoutR = new QHBoxLayout;
    QHBoxLayout *layoutG = new QHBoxLayout;
    QHBoxLayout *layoutB = new QHBoxLayout;
    layoutR->addWidget( labelR );
    layoutR->addWidget( m_pSliderR );
    layoutR->addWidget( spinBoxR );
    layoutG->addWidget( labelG );
    layoutG->addWidget( m_pSliderG );
    layoutG->addWidget( spinBoxG );
    layoutB->addWidget( labelB );
    layoutB->addWidget( m_pSliderB );
    layoutB->addWidget( spinBoxB );
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout( layoutR );
    layout->addLayout( layoutG );
    layout->addLayout( layoutB );

    setLayout( layout );
}

/* End of mainwindow.cpp */
