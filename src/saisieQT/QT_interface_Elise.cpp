#include "QT_interface_Elise.h"

cQT_Interface::cQT_Interface(cAppli_SaisiePts &appli, MainWindow *QTMainWindow):
    m_QTMainWindow(QTMainWindow),
    _data(NULL)
{
    _cNamePt = new cCaseNamePoint ("CHANGE", eCaseAutoNum);

    mParam = &appli.Param();
    mAppli = &appli;

    mRefInvis = appli.Param().RefInvis().Val();

    for (int aK = 0; aK < m_QTMainWindow->nbWidgets();++aK)
    {
        GLWidget* widget = m_QTMainWindow->getWidget(aK);

        connect(widget,	SIGNAL(addPoint(QPointF)), this,SLOT(addPoint(QPointF)));

        connect(widget,	SIGNAL(movePoint(int)), this,SLOT(movePoint(int)));

        connect(widget,	SIGNAL(selectPoint(int)), this,SLOT(selectPoint(int)));

        connect(widget,	SIGNAL(removePoint(int, int)), this,SLOT(changeState(int,int)));

        connect(widget,	SIGNAL(overWidget(void*)), this,SLOT(changeCurPose(void*)));

        connect(widget->contextMenu(),	SIGNAL(changeState(int,int)), this,SLOT(changeState(int,int)));

        connect(widget->contextMenu(),	SIGNAL(changeName(QString, QString)), this, SLOT(changeName(QString, QString)));

        connect(widget->contextMenu(),	SIGNAL(changeImagesSignal(int, bool)), this, SLOT(changeImages(int, bool)));
     }

    connect(m_QTMainWindow,	SIGNAL(showRefuted(bool)), this, SLOT(SetInvisRef(bool)));

    connect(m_QTMainWindow,	SIGNAL(undoSgnl(bool)), this, SLOT(undo(bool)));

    connect(m_QTMainWindow->threeDWidget(),	SIGNAL(filesDropped(QStringList, bool)), this, SLOT(filesDropped(QStringList, bool)));

    _data = new cData;

    rebuildGlCamera();

    _data->computeBBox();

    m_QTMainWindow->init3DPreview(_data);

    Init();

    connect(m_QTMainWindow,	SIGNAL(imagesAdded(int, bool)), this, SLOT(changeImages(int, bool)));

    connect(m_QTMainWindow,	SIGNAL(removePoint(QString)), this, SLOT(removePoint(QString)));

    connect(m_QTMainWindow,	SIGNAL(setName(QString)), this, SLOT(setAutoName(QString)));

    mAppli->SetInterface(this);

    m_QTMainWindow->setModel(new ModelPointGlobal(0,mAppli),new ModelCImage(0,mAppli));

    updateTables();

    connect(m_QTMainWindow->tableView_PG()->model(),SIGNAL(pGChanged()), this, SLOT(rebuildGlPoints()));
    connect(this,SIGNAL(dataChanged()), m_QTMainWindow->tableView_PG(), SLOT(update()));
    connect(this,SIGNAL(dataChanged()), m_QTMainWindow->tableView_Images(), SLOT(update()));
    connect(this,SIGNAL(dataChanged(cSP_PointeImage*)), this, SLOT(rebuildGlPoints(cSP_PointeImage*)));
    connect(m_QTMainWindow->tableView_PG(),SIGNAL(entered(QModelIndex)), this, SLOT(selectPG(QModelIndex)));

}

void cQT_Interface::SetInvisRef(bool aVal)
{
    mRefInvis = aVal;
}

pair<int, string> cQT_Interface::IdNewPts(cCaseNamePoint *aCNP)
{
    int aCptMax = mAppli->GetCptMax() + 1;

    string aName = aCNP->mName;
    if (aCNP->mTCP == eCaseAutoNum)
    {
        aName = nameFromAutoNum(aCNP, aCptMax);
    }

    return pair<int,string>(aCptMax,aName);
}

void cQT_Interface::updateTables()
{
    ModelPointGlobal* model = (ModelPointGlobal*)m_QTMainWindow->tableView_PG()->model();

    for (int row = mAppli->PG().size(); row <  model->rowCount(); ++row)
    {
        if(model->caseIsSaisie(row))

            m_QTMainWindow->tableView_PG()->hideRow(row);

    }

    m_QTMainWindow->tableView_PG()->hideRow(mAppli->PG().size());

    m_QTMainWindow->resizeTables();

}

eTypePts cQT_Interface::PtCreationMode()
{
    return m_QTMainWindow->getParams()->getPtCreationMode();
}

double cQT_Interface::PtCreationWindowSize()
{
    return m_QTMainWindow->getParams()->getPtCreationWindowSize();
}

void cQT_Interface::addPoint(QPointF point)
{
    if (m_QTMainWindow->currentWidget()->hasDataLoaded())
    {

        cSP_PointGlob * PG = cVirtualInterface::addPoint(transformation(point),ptCurrentCImage());

        if(PG)
        {
            emit dataChanged();
            m_QTMainWindow->tableView_PG()->model()->insertRows(cVirtualInterface::idPointGlobal(PG),1);
            updateTables();
        }
    }
}

void cQT_Interface::removePointGlobal(cSP_PointGlob * pPg)
{
    if (pPg)
    {
        DeletePoint( pPg );

        emit dataChanged();

        m_QTMainWindow->tableView_PG()->hideRow(cVirtualInterface::idPointGlobal(pPg));
    }
}

void cQT_Interface::movePoint(int idPt)
{
    if( idPt >= 0 )
    {
        cSP_PointeImage* aPIm = currentPointeImage(idPt);

        if(aPIm)
        {

            UpdatePoints(aPIm, transformation(getGLPt_CurWidget(idPt)));

            emit dataChanged(aPIm);
        }
    }
}

void cQT_Interface::removePoint(QString aName)
{
    removePointGlobal(mAppli->PGlobOfNameSVP(aName.toStdString()));
}

void cQT_Interface::changeState(int state, int idPt)
{
    eEtatPointeImage aState = (eEtatPointeImage)state;

    if (aState!=eEPI_NonValue && idPt != -1)
    {
        cSP_PointeImage* aPIm = currentPointeImage(idPt);

        if (aPIm)
        {
            if(aState == eEPI_Highlight)
            {
                aPIm->Gl()->HighLighted() = !aPIm->Gl()->HighLighted();
                if(aPIm->Gl()->HighLighted())
                    m_QTMainWindow->threeDWidget()->setTranslation(aPIm->Gl()->PG()->P3D().Val());
            }
            else if (aState == eEPI_Disparu)

                removePointGlobal(aPIm->Gl());

            else
            {
                ChangeState(aPIm, aState);

                float zoom = m_QTMainWindow->currentWidget()->getZoom();

                cPointGlob* pg = aPIm->Gl()->PG();

                for (int i = 0; i < m_QTMainWindow->nbWidgets(); ++i)
                {
                    cImage* image = mAppli->image(idCImage(i));
                    cSP_PointeImage* ptI = image->PointeOfNameGlobSVP(pg->Name());

                    if(ptI && ptI!=aPIm && ptI->Visible())
                    {
                        cOneSaisie* sPt = ptI->Saisie();
                        if(sPt)
                        {
                            QPointF pt(sPt->PtIm().x,image->SzIm().y - sPt->PtIm().y);
                            m_QTMainWindow->getWidget(i)->setZoom(zoom);
                            m_QTMainWindow->getWidget(i)->centerViewportOnImagePosition(pt);

                        }
                    }
                }
            }

            emit dataChanged(aPIm);
        }
    }
}

void cQT_Interface::changeName(QString aOldName, QString aNewName)
{
    string oldName = aOldName.toStdString();
    string newName = aNewName.toStdString();

    cSP_PointeImage * aPIm = ptCurrentCImage()->PointeOfNameGlobSVP(oldName);

    if (aPIm)
    {
        cCaseNamePoint aCNP(newName, eCaseStd);

        for (int aK=0 ; aK< int(mVNameCase.size()) ; aK++)
        {
            cCaseNamePoint & Case = mVNameCase[aK];

            if (Case.mName == newName)
                aCNP = Case;
        }

        //if (aCNP.mFree)
        //{
            for (int aKP=0 ; aKP< int(mAppli->PG().size()) ; aKP++)
            {
                if (mAppli->PG()[aKP]->PG()->Name() == newName)
                {
                    QMessageBox::critical(m_QTMainWindow, "Error", "Point already exists");
                    return;
                }
            }

            mAppli->ChangeName(oldName, newName);
        //}

        emit dataChanged(aPIm);
    }
}

void cQT_Interface::changeImages(int idPt, bool aUseCpt)
{
    int aKW = 0; // id widget

    cSP_PointGlob* PointPrio = 0;

    bool thisWin = (idPt == -2);

    if (idPt >=0)
    {
        cSP_PointeImage* aPIm = currentPointeImage(idPt);
        PointPrio= aPIm->Gl();
    }

    mAppli->SetImagesPriority(PointPrio, aUseCpt);

    vector<cImage *> images = mAppli->images();

    cCmpIm aCmpIm(this);
    sort(images.begin(),images.end(),aCmpIm);

    int max = thisWin ? 1 : min(m_QTMainWindow->nbWidgets(),(int)images.size());

    while (aKW < max)
    {
        cImage * anIm = images[aKW];

        static int aCpt=0;
        aCpt++;
        anIm->CptAff() = aCpt;

        if (!isDisplayed(anIm))
        {
            cGLData* data = getGlData(anIm);

            if (data)
            {
                GLWidget * glW = m_QTMainWindow->getWidget(thisWin ? CURRENT_IDW : aKW);
                glW->setGLData(data, data->stateOption(cGLData::OpShow_Mess));
                glW->setParams(m_QTMainWindow->getParams());
                glW->getHistoryManager()->setFilename(m_QTMainWindow->getEngine()->getFilenamesIn()[aKW]);
            }
        }
        aKW++;
    }

    mAppli->SetImages(images);

    rebuildGlPoints();
}

void cQT_Interface::selectPoint(int idPt)
{
    rebuild3DGlPoints(idPt >= 0 ? currentPointeImage(idPt) : NULL);

    if (idPt >=0)
    {

        int idPG = idPointGlobal(idPt);

        m_QTMainWindow->tableView_PG()->selectRow(idPG);

        populateTableImages(idPG);

    }
}

int cQT_Interface::idPointGlobal(int idSelectGlPoint)
{
    return cVirtualInterface::idPointGlobal(getNameGLPt_CurWidget(idSelectGlPoint));
}

cPoint cQT_Interface::getGLPt_CurWidget(int idPt)
{
    return (*m_QTMainWindow->currentWidget()->getGLData()->polygon())[idPt];
}

string cQT_Interface::getNameGLPt_CurWidget(int idPt)
{
    return getGLPt_CurWidget(idPt).name().toStdString();
}

void cQT_Interface::populateTableImages(int idPG)
{
    ((ModelCImage*)m_QTMainWindow->tableView_Images()->model())->setIdGlobSelect(idPG);

    m_QTMainWindow->tableView_Images()->update();
    m_QTMainWindow->resizeTables();
}

void cQT_Interface::setAutoName(QString name)
{
    mAppli->Param().NameAuto().SetVal( name.toStdString().c_str() );
}


bool cQT_Interface::isDisplayed(cImage* aImage)
{
    QString aName = QString(aImage->Name().c_str());

    for (int aK = 0; aK < m_QTMainWindow->nbWidgets();++aK)    
        if (m_QTMainWindow->getWidget(aK)->hasDataLoaded())
            if (m_QTMainWindow->getWidget(aK)->getGLData()->imageName() == aName)
                return true;

    return false;
}

void cQT_Interface::selectPG(QModelIndex modelIndex)
{
    if(modelIndex.row() < (int)mAppli->PG().size())
    {
        cSP_PointGlob* pg  = mAppli->PG()[modelIndex.row()];

        rebuild3DGlPoints(pg->PG());

        populateTableImages(modelIndex.row());

#if ELISE_QT_VERSION==5
       emit m_QTMainWindow->selectPoint(QString(pg->PG()->Name().c_str()));
#else
       m_QTMainWindow->emitSelectPoint(QString(pg->PG()->Name().c_str()));
#endif

    }
}

void cQT_Interface::undo(bool aBool)
{
    if (aBool)
        mAppli->Undo();
    else
        mAppli->Redo();

    emit dataChanged();
}

void cQT_Interface::changeCurPose(void *widgetGL)
{
    if (((GLWidget*)widgetGL)->hasDataLoaded())
    {
        QString nameImage = ((GLWidget*)widgetGL)->getGLData()->imageName();

        int t = idCImage(nameImage);

        for (int c = 0; c  < m_QTMainWindow->threeDWidget()->getGLData()->camerasCount(); ++c )
            m_QTMainWindow->threeDWidget()->getGLData()->camera(c)->setSelected(false);

        m_QTMainWindow->threeDWidget()->getGLData()->camera(t)->setSelected(true);

        m_QTMainWindow->threeDWidget()->update();
    }
}

void cQT_Interface::filesDropped(const QStringList &filenames, bool setGLData)
{
    if (filenames.size())
    {
        for (int i=0; i< filenames.size();++i)
        {
            if(!QFile(filenames[i]).exists())
            {
                QMessageBox::critical(m_QTMainWindow, "Error", "File does not exist (or bad argument)");
                return;
            }
        }

        QString suffix = QFileInfo(filenames[0]).suffix();

        if (suffix == "ply")
        {
            m_QTMainWindow->loadPly(filenames);
            _data->addCloud(m_QTMainWindow->getEngine()->getData()->getCloud(0));
            m_QTMainWindow->threeDWidget()->getGLData()->clearClouds();
            _data->computeBBox();
            m_QTMainWindow->threeDWidget()->getGLData()->setData(_data,false);
            m_QTMainWindow->threeDWidget()->resetView(false,false,false,true);
            m_QTMainWindow->option3DPreview();
        }
    }
}

int cQT_Interface::idCImage(QString nameImage)
{
    int t = -1;

    for (int i = 0; i < mAppli->nbImages(); ++i)
    {
       QString nameCImage(mAppli->image(i)->Name().c_str());
       if(nameCImage == nameImage)
           t = i;
    }

    return t;
}

cImage * cQT_Interface::ptCurrentCImage()
{
    return cVirtualInterface::ptCImage(idCurrentCImage());
}

cImage *cQT_Interface::ptCImage(QString nameImage)
{
    return cVirtualInterface::ptCImage(idCImage(nameImage));
}

int cQT_Interface::idCurrentCImage()
{
    return idCImage(m_QTMainWindow->currentWidget()->getGLData());
}

int cQT_Interface::idCImage(cGLData* data)
{
    return idCImage(data->imageName());
}

int cQT_Interface::idCImage(int idGlWidget)
{
    return idCImage(m_QTMainWindow->getWidget(idGlWidget)->getGLData());
}

cSP_PointeImage * cQT_Interface::currentPointeImage(int idPoint)
{
    return ptCurrentCImage()->PointeOfNameGlobSVP(getNameGLPt_CurWidget(idPoint));
}


cGLData * cQT_Interface::getGlData(int idWidget)
{
    return (idWidget == -1) ? m_QTMainWindow->currentWidget()->getGLData() : m_QTMainWindow->getWidget(idWidget)->getGLData();
}

cGLData *cQT_Interface::getGlData(cImage *image)
{
    if(!image) return NULL;

    for (int iGd = 0; iGd < m_QTMainWindow->getEngine()->nbGLData(); ++iGd)
    {
        QString nameImage = QString(image->Name().c_str());
            if(nameImage == m_QTMainWindow->getEngine()->getGLData(iGd)->imageName())
                return m_QTMainWindow->getEngine()->getGLData(iGd);
    }

    return NULL;
}

Pt2dr cQT_Interface::transformation(QPointF pt, int idImage)
{
    return Pt2dr(pt.x(),getGlData(idImage)->glImage()._m_image->height() - pt.y());
}

QPointF cQT_Interface::transformation(Pt2dr pt, int idImage)
{
    return QPointF(pt.x,getGlData(idImage)->glImage()._m_image->height() - pt.y);
}

void cQT_Interface::addGlPoint(cSP_PointeImage * aPIm, int idImag)
{
    cOneSaisie *aSom = aPIm->Saisie();
    cSP_PointGlob* aPG = aPIm->Gl();

    QPointF aPt1(0.f,0.f);
    QPointF aPt2(0.f,0.f);

    if (aPG && aPG->HighLighted())
    {
        Pt2dr epi1, epi2;
        if (aPIm->BuildEpipolarLine(epi1, epi2))
        {
            aPt1 = transformation(epi1,idImag);
            aPt2 = transformation(epi2,idImag);
        }
    }

    m_QTMainWindow->getWidget(idImag)->addGlPoint(transformation(aSom->PtIm(),idImag), aSom, aPt1, aPt2, aPG->HighLighted());
}

void cQT_Interface::rebuild3DGlPoints(cPointGlob * selectPtGlob)
{

    vector< cSP_PointGlob * > pGV = mAppli->PG();

    if(pGV.size())
    {
        bool first = _data->getNbClouds() == 0;

        if(!first)
            delete _data->getCloud(0);

        GlCloud *cloud = new GlCloud();

        for (int i = 0; i < (int)pGV.size(); ++i)
        {
            cPointGlob * pg = pGV[i]->PG();

            QColor colorPt = Qt::green;

            if (pg == selectPtGlob)
                colorPt = Qt::blue;
            else if (pGV[i]->HighLighted())
                colorPt = Qt::red;

            cloud->addVertex(GlVertex(pg->P3D().Val(), colorPt));
        }

        if(first)
            _data->addCloud(cloud);
        else
            _data->replaceCloud(cloud);

        _data->computeBBox();

        m_QTMainWindow->threeDWidget()->getGLData()->replaceCloud(_data->getCloud(0));
        m_QTMainWindow->threeDWidget()->resetView(first,false,first,true);
        m_QTMainWindow->option3DPreview();
    }
}

void cQT_Interface::rebuildGlPoints(cSP_PointeImage* aPIm)
{
    rebuild2DGlPoints();

    rebuild3DGlPoints(aPIm);

    Save();
}

void cQT_Interface::rebuild3DGlPoints(cSP_PointeImage* aPIm)
{
    rebuild3DGlPoints(aPIm ? aPIm->Gl()->PG() : NULL);
}

void cQT_Interface::rebuild2DGlPoints()
{
    for (int i = 0; i < m_QTMainWindow->nbWidgets(); ++i)
    {
        if(m_QTMainWindow->getWidget(i)->hasDataLoaded())
        {
            int t = idCImage(i);

            if(t!=-1)
            {
                const vector<cSP_PointeImage *> &  aVP = mAppli->image(t)->VP();

                m_QTMainWindow->getWidget(i)->getGLData()->clearPolygon();

                for (int aK=0 ; aK<int(aVP.size()) ; aK++)
                    if (PtImgIsVisible(*(aVP[aK])))
                    {
                        addGlPoint(aVP[aK], i);
                    }

                m_QTMainWindow->getWidget(i)->update();
            }
        }
    }
}

void cQT_Interface::Init()
{
    InitNbWindows();

    InitVNameCase();
}

void cQT_Interface::rebuildGlCamera()
{
    for (int i = 0; i < mAppli->nbImages(); ++i)
    {
        ElCamera * aCamera = mAppli->image(i)->CaptCam();
        _data->addCamera(aCamera->CS());
    }
}

void cQT_Interface::
AddUndo(cOneSaisie *aSom)
{
    mAppli->AddUndo(*aSom, ptCurrentCImage());
}

cCaseNamePoint *cQT_Interface::GetIndexNamePoint()
{

    QItemSelectionModel *selModel = m_QTMainWindow->tableView_PG()->selectionModel();

    if (selModel->currentIndex().column() != 0)
        return _cNamePt;
    else
    {
        if(_cNamePt)
            delete _cNamePt;

        string aName = selModel->currentIndex().data(Qt::DisplayRole).toString().toStdString();

        cSP_PointGlob * aPt = mAppli->PGlobOfNameSVP(aName);
        if (!aPt)
        {
            _cNamePt = new cCaseNamePoint(aName, eCaseSaisie); //fake pour faire croire � une saisie � la X11
        }
        else
            _cNamePt = new cCaseNamePoint("CHANGE", eCaseAutoNum);
    }

    return _cNamePt;
}
