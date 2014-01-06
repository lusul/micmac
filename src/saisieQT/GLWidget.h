#ifndef _GLWIDGET_H
#define _GLWIDGET_H

#include <cmath>
#include <limits>
#include <iostream>
#include <algorithm>

#include <QtOpenGL/QGLWidget>
#include <QGLContext>
#include <QDebug>

#include "GL/glu.h"

#include <QUrl>
#include <QtGui/QMouseEvent>
#include <QSettings>
#include <QMimeData>
#include <QTime>
#include <QPainter>


#include "Data.h"
#include "Engine.h"
#include "3DTools.h"


#include "3DObject.h"

class GLWidgetSet;

//! View orientation
enum VIEW_ORIENTATION {  TOP_VIEW,      /**< Top view (eye: +Z) **/
                         BOTTOM_VIEW,	/**< Bottom view **/
                         FRONT_VIEW,	/**< Front view **/
                         BACK_VIEW,     /**< Back view **/
                         LEFT_VIEW,     /**< Left view **/
                         RIGHT_VIEW     /**< Right view **/
};

//! Default message positions on screen
enum MessagePosition {  LOWER_LEFT_MESSAGE,
                        LOWER_RIGHT_MESSAGE,
                        LOWER_CENTER_MESSAGE,
                        UPPER_CENTER_MESSAGE,
                        SCREEN_CENTER_MESSAGE
};

//! Temporary Message to display
struct MessageToDisplay
{
    MessageToDisplay():
        color(Qt::white)
    {}

    //! Message
    QString message;

    //! Color
    QColor color;

    //! Message position on screen
    MessagePosition position;
};

class c3DCamera
{
public:
    c3DCamera();
    ~c3DCamera();

    GLdouble*   getModelViewMatrix(){return _mvMatrix;}
    GLdouble*   getProjectionMatrix(){return _projMatrix;}
    GLint*      getGLViewport(){return _glViewport;}

    void        doProjection(QPointF point, float zoom);

    void        orthoProjection();

    void        scaleAndTranslate(float x, float y, float zoom);

    GLdouble    mvMatrix(int i)     { return _mvMatrix[i];   }
    GLdouble    projMatrix(int i)   { return _projMatrix[i]; }

    GLint       ViewPort(int i)     { return _glViewport[i]; }

    GLint       vpWidth()     { return _glViewport[2]; }
    GLint       vpHeight()    { return _glViewport[3]; }

    void        setMatrices();

    void        resetPosition(){m_glPosition[0] = m_glPosition[1] = 0.f;}

    //! 3D point projection in viewport
    void        getProjection(QPointF &P2D, Pt3dr P);

    //! Project a point from window to image
    QPointF     WindowToImage(const QPointF &pt, float zoom);

    //! Project a point from image to window
    QPointF     ImageToWindow(const QPointF &im, float zoom);

    cPolygon    PolygonImageToWindow(cPolygon polygon, float zoom);

    GLfloat     m_glPosition[2];

private:
    GLdouble    *_mvMatrix;
    GLdouble    *_projMatrix;
    GLint       *_glViewport;
};


class GLWidget : public QGLWidget
{
    Q_OBJECT

public:

    //! Default constructor
    GLWidget(int idx, GLWidgetSet *theSet, const QGLWidget *shared);

    //! Destructor
    ~GLWidget(){}

    //! Interaction mode (only in 3D)
    enum INTERACTION_MODE { TRANSFORM_CAMERA,
                            SELECTION
    };

    //! Displays a status message
    /** \param message message (if message is empty, all messages will be cleared)
        \param pos message position on screen
    **/
    virtual void displayNewMessage(const QString& message,
                                   MessagePosition pos = SCREEN_CENTER_MESSAGE,
                                   QColor color = Qt::white);

    //! States if data (cloud, camera or image) is loaded
    bool hasDataLoaded();

    //! Sets camera to a predefined view (top, bottom, etc.)
    void setView(VIEW_ORIENTATION orientation);

    //! Sets current zoom
    void setZoom(float value);

    //! Get current zoom
    float getZoom(){return getParams()->m_zoom;}

    void zoomFit();

    void zoomFactor(int percent);

    //! Switch between move mode and selection mode (only in 3D)
    void setInteractionMode(INTERACTION_MODE mode, bool showmessage);

    bool getInteractionMode(){return m_interactionMode;}

    //! Shows axis or not
    void showAxis(bool show);

    //! Shows ball or not
    void showBall(bool show);

    //! Shows cams or not
    void showCams(bool show);

    //! Shows bounding box or not
    void showBBox(bool show);

    //! Construct help messages
    void constructMessagesList(bool show);

    //! Apply selection to data
    void Select(int mode, bool saveInfos = true);

    //! Delete current polyline
    void clearPolyline();

    //! Undo last action
    void undo();

    //! Undo all past selection actions
    void undoAll();

    //! Get the selection infos stack
    QVector <selectInfos> getSelectInfos(){return _infos;}

    //! Avoid all past actions
    void reset();

    //! Reset view
    void resetView();

    //! Reset rotation matrix
    void resetRotationMatrix();

    //! Reset translation matrix
    void resetTranslationMatrix();

    ViewportParameters* getParams(){return &_params;}

    void setGLData(cGLData* aData, bool showMessage = true, bool doZoom = true);
    cGLData* getGLData(){return m_GLData;}

    void setBackgroundColors(QColor const &col0, QColor const &col1);

    int renderTextLine(MessageToDisplay messageTD, int x, int y, int sizeFont = 10);

    std::list<MessageToDisplay>::iterator GetLastMessage();

public slots:
    void zoom();

    void onWheelEvent(float wheelDelta_deg);

signals:

    //! Signal emitted when files are dropped on the window
    void filesDropped(const QStringList& filenames);

protected:
    void resizeGL(int w, int h);
    void paintGL();
    void mouseDoubleClickEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent* event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    //inherited from QWidget (drag & drop support)
    virtual void dragEnterEvent(QDragEnterEvent* event);
    virtual void dropEvent(QDropEvent* event);

    //! Draw widget gradient background
    void drawGradientBackground();
    
    //! Draw selection polygon
    void drawPolygon();

    //! GL context aspect ratio (width/height)
    float m_glRatio;

    //! Default font
    QFont m_font;

    //! States if messages should be displayed
    bool m_bDrawMessages;

    //! Current interaction mode (with mouse)
    INTERACTION_MODE m_interactionMode;

    bool m_bFirstAction;

    //! List of messages to display
    list<MessageToDisplay> m_messagesToDisplay;

    //! Data to display
    cGLData    *m_GLData;

    //! states if display is 2D or 3D
    bool        m_bDisplayMode2D;

    //! data position in the gl viewport
    //GLfloat     m_glPosition[2];

    QPointF     m_lastMoveImage;
    QPoint      m_lastClickZoom;

    QPointF     m_lastPosImage;
    QPoint      m_lastPosWindow;

private:

    //! Window parameters (zoom, etc.)
    ViewportParameters _params;

    //! selection infos stack
    QVector <selectInfos> _infos;

    void        setProjectionMatrix();
    void        computeFPS(MessageToDisplay &dynMess);

    int         _frameCount;
    int         _previousTime;
    int         _currentTime;    

    QTime       _time;

    GLfloat     _rotationMatrix[16];

    c3DCamera   _g_Cam;

    int         _idx;

    GLWidgetSet* _parentSet;

    QColor      _BGColor0;
    QColor      _BGColor1;
};

#endif  /* _GLWIDGET_H */

