#ifndef __3DOBJECT__
#define __3DOBJECT__

#include "StdAfx.h"

#ifdef Int
    #undef Int
#endif
#include <QColor>
#include <QGLWidget>
#include <QGLShaderProgram>
#include <QPainter>

#ifdef ELISE_Darwin
    #include "OpenGL/glu.h"
#else
    #ifdef _WIN32
        #include "windows.h"
    #endif
    #include "GL/glu.h"
#endif

#include "Settings.h"

#define QMaskedImage cMaskedImage<QImage>

//! Interaction mode (only in 3D)
enum INTERACTION_MODE {
    TRANSFORM_CAMERA,
    SELECTION
};

enum LINE_STYLE
{
    LINE_NOSTIPPLE,
    LINE_STIPPLE
};

//! Selection mode
enum SELECTION_MODE { SUB,
                      ADD,
                      INVERT,
                      ALL,
                      NONE
                    };

using namespace NS_SaisiePts;

class cObject
{
    public:
        cObject();
        cObject(Pt3dr pt, QColor col);
        virtual ~cObject();

        QString name()          { return _name;     }
        Pt3dr   getPosition()   { return _position; }
        QColor  getColor()      { return _color;    }
        float   getScale()      { return _scale;    }
        bool    isVisible()     { return _bVisible; }
        bool    isSelected()    { return _bSelected;}

        void    setName(QString name)          { _name = name;     }
        void    setPosition(Pt3dr const &aPt)  { _position = aPt;  }
        void    setColor(QColor const &aCol)   { _color = aCol;    }
        void    setScale(float aScale)         { _scale = aScale;  }
        void    setVisible(bool aVis)          { _bVisible = aVis; }
        void    setSelected(bool aSel)         { _bSelected = aSel;}

        cObject & operator = (const cObject &);

    protected:

        QString _name;

        Pt3dr   _position;
        QColor  _color;
        float   _scale;

        float   _alpha;
        bool    _bVisible;
        bool    _bSelected;
};

class cObjectGL : public cObject
{
    public:
        cObjectGL(){}
        virtual ~cObjectGL(){}

        virtual void draw()=0;

        void    setLineWidth(float width) { _lineWidth = width; }

    protected:

        float   _lineWidth;

        void    enableOptionLine();

        void    disableOptionLine();
};

class cPoint : public cObjectGL, public QPointF
{
    public:
    cPoint(QPainter * painter = NULL,
           QPointF pos = QPointF(0.f,0.f),
           QString name = "",
           bool showName   = false,
           int  state = eEPI_NonValue,
           bool isSelected = false,
           QColor color = Qt::red,
           QColor selectionColor = Qt::blue,
           float diameter = 4.f,
           bool  highlight  = false);

        void draw();

        QColor getSelectionColor() { return _selectionColor; }

        void setState(int state)    { _state = state;    }
        void setDiameter(float val) { _diameter = val;   }
        int  state() const          { return _state;     }
        void showName(bool show)    { _bShowName = show; }

        bool highlight() const      { return _highlight; }
        void setHighlight(bool hl)  { _highlight = hl;   }
        void switchHighlight()      { _highlight = !_highlight; }

        void setEpipolar(QPointF pt1, QPointF pt2);

private:

        float   _diameter;
        bool    _bShowName;
        int     _state;
        bool    _highlight;

        QColor  _selectionColor;

        //! Default font
        QFont   _font;

        QPainter *_painter;
        QGLWidget *_widget;

        bool     _bEpipolar;
        QPointF  _epipolar1;
        QPointF  _epipolar2;
};

class cCircle : public cObjectGL
{
    public:
        cCircle(Pt3dr, QColor, float, float, bool, int dim);
        cCircle(int dim);

        void    draw();

    private:

        int     _dim;
};

class cCross : public cObjectGL
{
    public:
        cCross(Pt3dr, QColor, float, float, bool, int dim);

        void    draw();

    private:
        int     _dim;
};

class cBall : public cObjectGL
{
    public:

        cBall(Pt3dr pt = Pt3dr(0.f,0.f,0.f), float scale = 1.f, bool isVis = true, float lineWidth = 1.f);
        ~cBall();

        void    setPosition(Pt3dr const &aPt);
        Pt3dr   getPosition();
        void    setVisible(bool aVis);
        void    setScale(float aScale);

        void    draw();

    private:

        cCircle *_cl0;
        cCircle *_cl1;
        cCircle *_cl2;

        cCross  *_cr0;
        cCross  *_cr1;
        cCross  *_cr2;
};

class cAxis : public cObjectGL
{
    public:
        cAxis(Pt3dr pt = Pt3dr(0.f,0.f,0.f), float scale = 1.f, float lineWidth = 1.f);

        void    draw();
};

class cGrid : public cObjectGL
{
    public:
        cGrid(Pt3dr pt = Pt3dr(0.f,0.f,0.f), float scale = 1.f, int nb = 1.f);

        void    draw();
};

class cBBox : public cObjectGL
{
    public:
        cBBox(Pt3dr pt = Pt3dr(0.f,0.f,0.f), float scale = 1.f, Pt3dr min= Pt3dr(0.f,0.f,0.f), Pt3dr max= Pt3dr(1.f,1.f,1.f), float lineWidth = 1.f);

        void    draw();

        void set(Pt3d<double> min, Pt3d<double> max);

    private:
        Pt3dr   _min;
        Pt3dr   _max;
};

class cCam : public cObjectGL
{
    public:
        cCam(CamStenope *pCam, float scale, bool isVisible = true, float lineWidth = 1.f);

        void    draw();

        void    setpointSize(float size) { _pointSize = size; }

    private:
        float   _pointSize;

        CamStenope *_Cam;
};

class cPolygonHelper;

class cPolygon : public cObjectGL
{
    public:

        cPolygon(QPainter* painter = NULL, float lineWidth = 1.0f, QColor lineColor = Qt::green, QColor pointColor = Qt::red, int style = LINE_NOSTIPPLE);
        cPolygon(QVector <QPointF> points, bool isClosed);

        void    draw();

        void    close();

        bool    isPointInsidePoly(const QPointF& P);

        bool    findNearestPoint(const QPointF &pos, float getRadius = _selectionRadius);

        void    removeNearestOrClose(QPointF pos); //remove nearest point, or close polygon
        void    removeSelectedPoint();

        int     setNearestPointState(const QPointF &pos, int state);
        int     highlightNearestPoint(const QPointF &pos);
        int     getNearestPointIndex(const QPointF &pos);
        QString getNearestPointName(const QPointF &pos);
        QString getSelectedPointName();
        int     getSelectedPointState();

        void    setPointSize(float size) { _pointDiameter = size; }

        void    add(cPoint &pt);
        void    add(QPointF const &pt, bool selected=false);
        void    addPoint(QPointF const &pt);

        void    clear();
        void    clearPoints() { _points.clear(); }

        void    setClosed(bool closed) { _bIsClosed = closed; }
        bool    isClosed(){ return _bIsClosed; }

        int     size(){ return _points.size(); }

        cPoint & operator[](int ak){ return _points[ak]; }
        const cPoint & operator[](int ak) const { return _points[ak]; }

        cPolygon & operator = (const cPolygon &);

        void    insertPoint( int i, const QPointF & value );

        void    insertPoint();

        void    removePoint( int i );

        QVector <QPointF> const getVector();
        void    setVector(QVector <cPoint> const &aPts){ _points = aPts; }
        void    setVector(QVector <QPointF> const &aPts);

        int     idx(){return _idx;}

        void    setPointSelected();
        bool    isPointSelected(){ return _bSelectedPoint; }
        void    resetSelectedPoint();

        cPolygonHelper* helper() { return _helper; }

        void    refreshHelper(QPointF pos, bool insertMode, float zoom);

        int     finalMovePoint();

        void    removeLastPoint();

        void    setPainter(QPainter * painter);

        // Points name
        void    showNames(bool show);
        bool    bShowNames() { return _bShowNames; }

        void    setDefaultName(QString name)    { _defPtName = name; }
        QString getDefaultName()                { return _defPtName; }

        void    rename(QPointF pos, QString name);

        void    showLines(bool show = true);
        bool    isLinear() { return _bShowLines; }

        void    showRefuted(bool show);
        bool    bShowRefuted() { return _bShowRefuted; }

        void    translate(QPointF Tr);

        void    flipY(float height);

        float   getRadius()             { return _selectionRadius; }
        void    setRadius(float val)    { _selectionRadius = val;  }

        void    setParams(cParameters* aParams);

    protected:
        cPolygon(QPainter * painter, float lineWidth, QColor lineColor,  QColor pointColor, bool withHelper, int style = LINE_STIPPLE);

        QVector <cPoint>    _points;
        cPolygonHelper*     _helper;
        QColor              _lineColor;
        int                 _idx;

        QPainter *          _painter;

    private:
        float               _pointDiameter;
        static float        _selectionRadius;

        //!states if polygon is closed
        bool                _bIsClosed;

        //!states if point with index _idx is selected
        bool                _bSelectedPoint;

        //!states if segments should be displayed
        bool                _bShowLines;

        //!states if names should be displayed
        bool                _bShowNames;

        //!states if refuted points should be displayed
        bool                _bShowRefuted;

        int                 _style;
        QVector<qreal>      _dashes;
        QString             _defPtName;
};

class cPolygonHelper : public cPolygon
{
    public:

        cPolygonHelper(  cPolygon* polygon, float lineWidth, QPainter *painter, QColor lineColor = Qt::blue, QColor pointColor = Qt::blue);

        void   build(const cPoint &pos, bool insertMode);

        void   setPoints(cPoint p1, cPoint p2, cPoint p3);

    private:

        cPolygon* _polygon;
};


class cImageGL : public cObjectGL
{
    public:
        cImageGL(float gamma = 1.0f);
        ~cImageGL();

        void    draw(QColor color);

        void    drawQuad(QColor color);

        static  void    drawQuad(GLfloat originX, GLfloat originY, GLfloat glh, GLfloat glw, QColor color = Qt::white);

        void    draw();

        void    setPosition(GLfloat originX, GLfloat originY);
        void    setDimensions(GLfloat glh, GLfloat glw);

        void    PrepareTexture(QImage *pImg);

        void    ImageToTexture(QImage *pImg);

        GLuint* getTexture(){return &_texture;}

        //height and width of original data
        int     width()  {return _size.width();}
        int     height() {return _size.height();}

        bool    isPtInside(QPointF const &pt);

        void    setGamma(float gamma){_gamma = (gamma >= 0) ? gamma : 0;}

        float   getGamma(){ return _gamma;}

        void    incGamma(float dgamma){setGamma(_gamma + dgamma);}

        static  void drawGradientBackground(int w,int h,QColor c1,QColor c2);

private:

        QGLShaderProgram _program;

        int     _matrixLocation;
        int     _texLocation  ;
        int     _gammaLocation;

        GLfloat _originX;
        GLfloat _originY;
        GLfloat _glh;
        GLfloat _glw;

        QSize   _size;

        //! Texture image
        GLuint  _texture;
        float   _gamma;

};

template<class T>
class cMaskedImage : public cObject
{

public:

    cMaskedImage(float gamma = 1.0f):
        _m_image(NULL),
        _m_mask(NULL),
        _m_newMask(true),
        _gamma(gamma)
    {}

    ~cMaskedImage()
    {}

    void deallocImages()
    {
        if(_m_image != NULL)
        {
            _m_image = NULL;
            delete _m_image;
        }
        if(_m_mask != NULL)
        {
            _m_mask = NULL;
            delete _m_mask;
        }
    }

    T           *_m_image;
    T           *_m_mask;

    bool        _m_newMask;
    float       _gamma;

};

class cMaskedImageGL : public cMaskedImage<cImageGL>, virtual public cObjectGL
{

public:

    cMaskedImageGL(){}

    cMaskedImageGL(QMaskedImage &qMaskedImage);

    void setDimensions(float h, float w)
    {
        _m_image->setDimensions(h,w);
        _m_mask->setDimensions(h,w);
    }

    void draw();

};
//====================================================================================

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

#include <QGLWidget>

class cMessages2DGL : public cObjectGL
{
public:

    cMessages2DGL(QGLWidget *glw):
        _bDrawMessages(true),
        m_font(QFont("Arial", 10, QFont::Normal, false)),
        glwid(glw)
    {}

    void draw();

    int renderTextLine(MessageToDisplay messageTD, int x, int y, int sizeFont = 10);

    void displayNewMessage(const QString& message,
                                       MessagePosition pos = SCREEN_CENTER_MESSAGE,
                                       QColor color = Qt::white);

    void constructMessagesList(bool show, int mode, bool m_bDisplayMode2D, bool dataloaded);

    std::list<MessageToDisplay>::iterator GetLastMessage();

    std::list<MessageToDisplay>::iterator GetPenultimateMessage();

    MessageToDisplay& LastMessage();

    void wh(int ww,int hh)
    {
        w=ww;
        h=hh;
    }

    bool drawMessages(){ return _bDrawMessages && size(); }

    void showMessages(bool show) { _bDrawMessages = show; }

    int size(){ return m_messagesToDisplay.size(); }

private:

    bool _bDrawMessages;

    list<MessageToDisplay> m_messagesToDisplay;

    //! Default font
    QFont m_font;

    QGLWidget *glwid;

    int w;
    int h;
};

//====================================================================================

class cData;
class GlCloud;
class MatrixManager;

#include "Data.h"
#include "MatrixManager.h"
#include <QFlags>



class cGLData : cObjectGL
{
public:

    cGLData();

    cGLData(QMaskedImage &qMaskedImage, bool modePt = false, QString ptName = "" );

    cGLData(cData *data);

    ~cGLData();

    void        draw();

    cMaskedImageGL glMaskedImage;

    QImage      *pQMask;

    //! Point list for polygonal selection
    cPolygon    m_polygon;

    bool        is3D()                                  { return Clouds.size() || Cams.size();   }

    bool        isImgEmpty()                            { return glMaskedImage._m_image == NULL; }

    QImage*     getMask()                               { return pQMask;     }

    void        setPolygon(cPolygon const &aPoly)       { m_polygon = aPoly; }

    void        clearPolygon()                          { m_polygon.clear(); }

    bool        isNewMask()                             { return !isImgEmpty() ? glMaskedImage._m_newMask : true; }

    QVector < cCam* > Cams;

    cBall       *pBall;
    cAxis       *pAxis;
    cBBox       *pBbox;
    cGrid       *pGrid;

    QVector < GlCloud* > Clouds;

    void        setDimensionImage(int vW,int vH);

    QString     imageName() { return glMaskedImage.cObjectGL::name(); }

    //info coming from cData
    float       getBBoxMaxSize(){return _diam;}
    void        setBBoxMaxSize(float aS){_diam = aS;}

    Pt3dr       getBBoxCenter(){return _center;}
    void        setBBoxCenter(Pt3dr aCenter){_center = aCenter;} // TODO a verifier : pourquoi le centre cGLData est initialisť avec BBoxCenter

    void        setGlobalCenter(Pt3dr aCenter);

    bool        position2DClouds(MatrixManager &mm,QPointF pos);

    void        editImageMask(int mode, cPolygon &polyg, bool m_bFirstAction);

    void        editCloudMask(int mode, cPolygon &polyg, bool m_bFirstAction, MatrixManager &mm);

    void        replaceCloud(GlCloud* cloud, int id = 0);

    void        setPainter(QPainter *);

    enum Option {
      OpNO          = 0x00,
      OpShow_Ball   = 0x01,
      OpShow_Axis   = 0x02,
      OpShow_BBox   = 0x04,
      OpShow_Mess   = 0x08,
      OpShow_Cams   = 0x10,
      OpShow_Grid   = 0x20
    //  OpShow_next      = 0x40
      // ...
    };

    Q_DECLARE_FLAGS(options, Option)

    options     _options;

    void        GprintBits(size_t const size, void const * const ptr);

    void        setOption(QFlags<Option> option,bool show);

    bool        stateOption(QFlags<Option> option){ return _options & option; }

    bool        mode() { return _modePt; }

    void        setData(cData *data, bool setCam = true);

    bool        incFirstCloud() const;

    void        setIncFirstCloud(bool incFirstCloud);

private:

    void        initOptions()
    {
        _options = options(OpShow_Mess);
    }

    float       _diam;

    Pt3dr       _center;

    bool        _modePt;

    bool        _incFirstCloud;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(cGLData::options)
//====================================================================================

#endif //__3DObject__
