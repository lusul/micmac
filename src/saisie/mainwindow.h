#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#ifndef  WIN32
//#ifndef __APPLE__
//#include "GL/glew.h"
//#endif
//#endif
#include <QMainWindow>
#include <QFutureWatcher>
#include <QtConcurrentRun>
#include <QProgressDialog>
#include <QTimer>
#include <QSignalMapper>

#include "GLWidget.h"
#include "Engine.h"

class GLWidget;

namespace Ui {
class MainWindow;
}

//const QColor colorBG0(30,132,181);
//const QColor colorBG1(70,70,70);

const QColor colorBG0(65,65,60);
const QColor colorBG1(120,115,115);

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(bool mode2D = false, QWidget *parent = 0);
    ~MainWindow();

    //! Checks for loaded data
    bool checkForLoadedData();

    void setPostFix(QString str);

public slots:

    //! Try to load a list of files
    void addFiles(const QStringList& filenames);
    void selectedPoint(uint idC,uint idV,bool select);

//protected slots:

    void doActionDisplayShortcuts();
    void toggleFullScreen(bool);
    void toggleShowAxis(bool);
    void toggleShowBall(bool);
    void toggleShowBBox(bool);
    void toggleShowCams(bool);
    void toggleShowMessages(bool);
    void toggleSelectionMode(bool state);

    void toggle2D3D(bool state);

    void add();
    void selectNone();
    void invertSelected();
    void selectAll();
    void removeFromSelection();

    //default views
    void setFrontView();
    void setBottomView();
    void setTopView();
    void setBackView();
    void setLeftView();
    void setRightView();

    //zoom
    void zoomPlus();
    void zoomMoins();
    void zoomFit();
    void zoomFactor(int aFactor );

    void echoMouseWheelRotate(float);

    void loadPlys();
    void loadCameras();
    void loadImage();
    void closeAll();
    void exportMasks();
    void exportMasksAs();
    void loadAndExport();
    void saveSelectionInfos();

    void openRecentFile();

    void progression();

    void setMode2D(bool mBool);
    bool getMode2D() {return m_bMode2D;}

    cEngine* getEngine(){return m_Engine;}

	void setGamma(float aGamma);

protected:

    //! Connects all QT actions to slots
    void connectActions();  

private:

    void                    createMenus();

    void                    setCurrentFile(const QString &fileName);
    void                    updateRecentFileActions();
    QString                 strippedName(const QString &fullFileName);

    int *                   m_incre;

    Ui::MainWindow*         ui;

    GLWidget*               m_glWidget;

    cEngine*                m_Engine;

    QFutureWatcher<void>    m_FutureWatcher;
    QProgressDialog*        m_ProgressDialog;

    enum { MaxRecentFiles = 3 };
    QAction *               m_recentFileActs[MaxRecentFiles];
    QString                 m_curFile;
    QStringList             m_FilenamesIn;

    QMenu*                  m_RFMenu; //recent files menu

    bool                    m_bMode2D;
};
#endif // MAINWINDOW_H
