#include "QT_interface_Elise.h"

cQT_Interface::cQT_Interface(cAppli_SaisiePts &appli, MainWindow *QTMainWindow):
    m_QTMainWindow(QTMainWindow),
    _data(NULL)
{
    mParam = &appli.Param();
    mAppli = &appli;

    mRefInvis = appli.Param().RefInvis().Val();

    for (int aK = 0; aK < m_QTMainWindow->nbWidgets();++aK)
    {
        connect(m_QTMainWindow->getWidget(aK),	SIGNAL(addPoint(QPointF)), this,SLOT(addPoint(QPointF)));

        connect(m_QTMainWindow->getWidget(aK),	SIGNAL(movePoint(int)), this,SLOT(movePoint(int)));

        connect(m_QTMainWindow->getWidget(aK),	SIGNAL(selectPoint(int)), this,SLOT(selectPoint(int)));

        connect(m_QTMainWindow->getWidget(aK),	SIGNAL(removePoint(int, int)), this,SLOT(changeState(int,int)));

        connect(m_QTMainWindow->getWidget(aK),	SIGNAL(overWidget(void*)), this,SLOT(changeCurPose(void*)));

        connect(m_QTMainWindow->getWidget(aK)->contextMenu(),	SIGNAL(changeState(int,int)), this,SLOT(changeState(int,int)));

        connect(m_QTMainWindow->getWidget(aK)->contextMenu(),	SIGNAL(changeName(QString, QString)), this,SLOT(changeName(QString, QString)));

        connect(m_QTMainWindow,	SIGNAL(showRefuted(bool)), this,SLOT(SetInvisRef(bool)));

        connect(m_QTMainWindow->threeDWidget(),	SIGNAL(filesDropped(QStringList)), this,SLOT(filesDropped(QStringList)));
    }

    _data = new cData;

    rebuildGlCamera();

    _data->computeBBox();

    m_QTMainWindow->threeDWidget()->setGLData(new cGLData(_data));
    m_QTMainWindow->threeDWidget()->getGLData()->setIncFirstCloud(true);
    option3DPreview();

    Init();
}

void cQT_Interface::SetInvisRef(bool aVal)
{
    mRefInvis = aVal;
}

std::pair<int, string> cQT_Interface::IdNewPts(cCaseNamePoint *aCNP)
{
   int aCptMax = mAppli->GetCptMax() + 1;

   std::string aName = aCNP->mName;
   if (aCNP->mTCP == eCaseAutoNum)
   {
      std::string nameAuto = mParam->NameAuto().Val();
      aName = nameAuto + ToString(aCptMax);
      aCNP->mName = nameAuto + ToString(aCptMax+1);
   }

   if (aCNP->mTCP == eCaseSaisie)
   {
         //mWEnter->raise();
         //ELISE_COPY(mWEnter->all_pts(),P8COL::yellow,mWEnter->odisc());

         // std::cin >> aName ;
         //aName = mWEnter->GetString(Pt2dr(5,15),mWEnter->pdisc()(P8COL::black),mWEnter->pdisc()(P8COL::yellow));
         //mWEnter->lower();
   }

   //mMenuNamePoint->W().lower();

   // std::cout << "cAppli_SaisiePts::IdNewPts " << aCptMax << " " << aName << "\n";
   //std::pair aRes(
   return std::pair<int,std::string>(aCptMax,aName);

}

int cQT_Interface::cImageIdxFromName(QString nameImage)
{
    int t = -1;

    for (int i = 0; i < mAppli->nbImages(); ++i)
    {
       QString nameCImage(mAppli->images(i)->Name().c_str());
       if(nameCImage == nameImage)
           t = i;
    }

    return t;
}

void cQT_Interface::addPoint(QPointF point)
{
    Pt2dr aPGlob(transformation(point));

    cCaseNamePoint aCNP("CHANGE",eCaseAutoNum);
    //TODO : aCNP *= GetIndexNamePoint();

    QString nameImage = m_QTMainWindow->currentWidget()->getGLData()->imageName();

    int t = cImageIdxFromName(nameImage);

    if(t != -1)
        mAppli->images(t)->CreatePGFromPointeMono(aPGlob,eNSM_Pts,-1,&aCNP);

    rebuildGlPoints();
}

cPoint cQT_Interface::selectedPt(int idPt)
{
    return m_QTMainWindow->currentWidget()->getGLData()->m_polygon[idPt];
}

string cQT_Interface::selectedPtName(int idPt)
{
    return selectedPt(idPt).name().toStdString();
}

void cQT_Interface::movePoint(int idPt)
{
    if(idPt >= 0 )
    {
        cSP_PointeImage* aPIm = currentPointeImage(idPt);

        if(aPIm)
        {
            mAppli->AddUndo(*(aPIm->Saisie()), currentcImage());

            aPIm->Saisie()->PtIm() = transformation(selectedPt(idPt));
            //Redraw();
            aPIm->Gl()->ReCalculPoints();

            rebuildGlPoints(aPIm);
        }
    }
}

void cQT_Interface::selectPoint(int idPt)
{
    rebuild3DGlPoints(idPt >= 0 ? currentPointeImage(idPt) : NULL);
}

void cQT_Interface::changeState(int state, int idPt)
{
    eEtatPointeImage aState = (eEtatPointeImage)state;

    if (aState!=eEPI_NonValue && idPt != -1)
    {
        cSP_PointeImage* aPIm = currentPointeImage(idPt);

        if (aPIm)
        {
            if(aState == NS_SaisiePts::eEPI_Highlight)

                aPIm->Gl()->HighLighted() = true;

            else if (aState == NS_SaisiePts::eEPI_Deleted)

                DeletePoint( aPIm->Gl() );

            else
            {
                mAppli->AddUndo(*(aPIm->Saisie()),currentcImage());
                aPIm->Saisie()->Etat() = aState;
                aPIm->Gl()->ReCalculPoints();                
            }

            rebuildGlPoints(aPIm);
        }
    }
}

void cQT_Interface::changeName(QString aOldName, QString aNewName)
{
    string oldName = aOldName.toStdString();
    string newName = aNewName.toStdString();

    cSP_PointeImage * aPIm = currentcImage()->PointeOfNameGlobSVP(oldName);

    if (aPIm)
    {
        cCaseNamePoint aCNP(newName, eCaseStd);

        for (int aK=0 ; aK< int(mVNameCase.size()) ; aK++)
        {
            cCaseNamePoint & Case = mVNameCase[aK];

            if (Case.mName == newName)
                aCNP = Case;
        }

        if (aCNP.mFree)
        {
            for (int aKP=0 ; aKP< int(mAppli->PG().size()) ; aKP++)
            {
                if (mAppli->PG()[aKP]->PG()->Name() == newName)
                {
                    QMessageBox::critical(m_QTMainWindow, "Error", "Point already exists");
                    return;
                }
            }

            mAppli->ChangeName(oldName, newName);
        }

        rebuildGlPoints(aPIm);
    }
}

void cQT_Interface::changeCurPose(void *widgetGL)
{
    QString nameImage = ((GLWidget*)widgetGL)->getGLData()->imageName();

    int t = cImageIdxFromName(nameImage);

    for (int c = 0; c  < m_QTMainWindow->threeDWidget()->getGLData()->Cams.size(); ++c )
        m_QTMainWindow->threeDWidget()->getGLData()->Cams[c]->setSelected(false);

    m_QTMainWindow->threeDWidget()->getGLData()->Cams[t]->setSelected(true);

    m_QTMainWindow->threeDWidget()->update();
}

void cQT_Interface::filesDropped(const QStringList &filenames)
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
            m_QTMainWindow->threeDWidget()->getGLData()->Clouds.clear();
            _data->computeBBox();
            m_QTMainWindow->threeDWidget()->getGLData()->setData(_data,false);
            m_QTMainWindow->threeDWidget()->resetView(false,false,false,true);
            option3DPreview();
        }
    }
}

cSP_PointeImage * cQT_Interface::currentPointeImage(int idPoint)
{
    return currentcImage()->PointeOfNameGlobSVP(selectedPtName(idPoint));
}

cImage * cQT_Interface::currentcImage()
{
    int t = currentcImageIdx();

    return mAppli->images(t);
}

int cQT_Interface::currentcImageIdx()
{
    return cImageIdxFromGL(m_QTMainWindow->currentWidget()->getGLData());
}

int cQT_Interface::cImageIdxFromGL(cGLData* data)
{
    return cImageIdxFromName(data->imageName());
}

int cQT_Interface::cImageIdx(int idGl)
{
    return cImageIdxFromGL(m_QTMainWindow->getWidget(idGl)->getGLData());
}

cGLData * cQT_Interface::getGlData(int idImage)
{
    cGLData * data = (idImage == -1) ? m_QTMainWindow->currentWidget()->getGLData() : m_QTMainWindow->getWidget(idImage)->getGLData();

    return data;
}

Pt2dr cQT_Interface::transformation(QPointF pt, int idImage)
{
    Pt2dr newPt(pt.x(),getGlData(idImage)->glMaskedImage._m_image->height() - pt.y());
    return newPt;
}

QPointF cQT_Interface::transformation(Pt2dr pt, int idImage)
{
    QPointF newPt(pt.x,getGlData(idImage)->glMaskedImage._m_image->height() - pt.y);
    return newPt;
}

void cQT_Interface::addGlPoint(cSP_PointeImage * aPIm, int i)
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
            aPt1 = transformation(epi1,i);
            aPt2 = transformation(epi2,i);
        }
    }

    m_QTMainWindow->getWidget(i)->addGlPoint(transformation(aSom->PtIm(),i), aSom, aPt1, aPt2, aPG->HighLighted());
}

void cQT_Interface::rebuild3DGlPoints(cSP_PointeImage* aPIm)
{
    std::vector< cSP_PointGlob * > pGV = mAppli->PG();

    cPointGlob * selectPtGlob = aPIm ? aPIm->Gl()->PG() : NULL;

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
        option3DPreview();
    }
}

void cQT_Interface::rebuild2DGlPoints()
{
    for (int i = 0; i < m_QTMainWindow->nbWidgets(); ++i)
    {
        if(m_QTMainWindow->getWidget(i)->hasDataLoaded())
        {
            int t = cImageIdx(i);

            if(t!=-1)
            {
                const std::vector<cSP_PointeImage *> &  aVP = mAppli->images(t)->VP();

                m_QTMainWindow->getWidget(i)->getGLData()->clearPolygon();

                for (int aK=0 ; aK<int(aVP.size()) ; aK++)
                    if (WVisible(*(aVP[aK])))
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

void cQT_Interface::rebuildGlPoints(cSP_PointeImage* aPIm)
{
    rebuild2DGlPoints();

    rebuild3DGlPoints(aPIm);

    Save();
}

bool cQT_Interface::WVisible(cSP_PointeImage & aPIm)
{
    const cOneSaisie  & aSom = *(aPIm.Saisie());
    eEtatPointeImage aState = aSom.Etat();

    return aPIm.Visible() && Visible(aState);
}

void cQT_Interface::rebuildGlCamera()
{
    for (int i = 0; i < mAppli->nbImages(); ++i)
    {
        ElCamera * aCamera = mAppli->images(i)->CaptCam();
        _data->addCamera(aCamera->CS());
    }
}

void cQT_Interface::option3DPreview()
{
    m_QTMainWindow->threeDWidget()->setOption(cGLData::OpShow_BBox | cGLData::OpShow_Cams);
    m_QTMainWindow->threeDWidget()->setOption(cGLData::OpShow_Ball | cGLData::OpShow_Mess | cGLData::OpShow_BBox,false);
}


cCaseNamePoint *cQT_Interface::GetIndexNamePoint()
{
   /* for (int aK=0 ; aK<int(mVNameCase.size()) ; aK++)
    {
        cCaseNamePoint & aCNP = mVNameCase[aK];
        mMenuNamePoint->StringCase(aPCase,aCNP.mFree ? aCNP.mName : "***" ,true);
    }


    Pt2di aKse = mMenuNamePoint->Pt2Case(Pt2di(aClk._pt));
    cCaseNamePoint * aRes =  &(mVNameCase[aKse.y]);

    if (! aRes->mFree)*/ return 0;

    //return aRes;
}
