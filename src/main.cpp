/* --------------------------------------------------------------------------
 *
 * main.cpp
 *
 * An application that demonstrates some drawing with OpenGL.
 *
 * More information about the implementation can be found in the other source
 * files.
 *
 * 2011 (C) Pekka Mäkinen
 *
 * -------------------------------------------------------------------------- */

/* Used libraries. */
#include <QApplication>
#include <iostream>
#include <QtOpenGL>

/* My implementations. */
#include "mainwindow.h"

using namespace std;

int main( int argc, char *argv[] )
{
    QApplication app( argc, argv );
    if( !QGLFormat::hasOpenGL() )
    {
        cerr << "This system has no OpenGL support" << endl;
        return 1;
    }
    MainWindow mainWin;
    mainWin.setWindowTitle( QObject::tr( "TurtleRenderer" ) );
    mainWin.resize( 600, 400 );
    mainWin.show();

    return app.exec();
}

/* End of main.cpp */
