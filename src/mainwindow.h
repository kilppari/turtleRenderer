/* --------------------------------------------------------------------------
 * mainwindow.h
 *
 * Main window widget for the application.
 *
 * 2011 (C) Pekka Mäkinen
 *
 * -------------------------------------------------------------------------- */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include "renderer.h"
//#include "ui_widgets/objectpanel.h"

/* Forward declarations. */
class QAction;
class QLabel;
//class Renderer;
class QDockWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Renderer    *m_pRenderer;
    QDockWidget *m_pDock;

    /* Map for menus. */
    QMap< QString, QMenu* >      m_Menu;
    /* Map for all the actions. */
    QMap< QString, QAction* >   m_Action;

    /* Coordinate label on the status bar. */
    QLabel      *m_pCoordLabel;
public:
    MainWindow();

private:
    void createActions();
    void createMenus();
    void createDockWindows();
    void createStatusBar();
    void createScene(); //Renderer();

private slots:
    void help();
    void updateStatusBar( float x, float y, float z );
    void updateStatusBar( const Vector3f &pos );
    void toggleDock();
};

class QSlider;

class ObjectPanel : public QWidget
{
    Q_OBJECT

private:
    Renderer    *m_pRenderer;
    QSlider     *m_pSliderR;
    QSlider     *m_pSliderG;
    QSlider     *m_pSliderB;

public:
    ObjectPanel( Renderer *renderer, QWidget *parent = 0 );

public slots:
    void setRGBValues( int r, int g, int b );
    void sendRGBValues();

signals:
    void colorChanged( int r, int g, int b);

private:
    void createColorChooser();
};

#endif /* MAINWINDOW_H */
