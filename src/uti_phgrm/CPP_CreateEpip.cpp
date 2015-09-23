/*Header-MicMac-eLiSe-25/06/2007

    MicMac : Multi Image Correspondances par Methodes Automatiques de Correlation
    eLiSe  : ELements of an Image Software Environnement

    www.micmac.ign.fr


    Copyright : Institut Geographique National
    Author : Marc Pierrot Deseilligny
    Contributors : Gregoire Maillet, Didier Boldo.

[1] M. Pierrot-Deseilligny, N. Paparoditis.
    "A multiresolution and optimization-based image matching approach:
    An application to surface reconstruction from SPOT5-HRS stereo imagery."
    In IAPRS vol XXXVI-1/W41 in ISPRS Workshop On Topographic Mapping From Space
    (With Special Emphasis on Small Satellites), Ankara, Turquie, 02-2006.

[2] M. Pierrot-Deseilligny, "MicMac, un lociel de mise en correspondance
    d'images, adapte au contexte geograhique" to appears in
    Bulletin d'information de l'Institut Geographique National, 2007.

Francais :

   MicMac est un logiciel de mise en correspondance d'image adapte
   au contexte de recherche en information geographique. Il s'appuie sur
   la bibliotheque de manipulation d'image eLiSe. Il est distibue sous la
   licences Cecill-B.  Voir en bas de fichier et  http://www.cecill.info.


English :

    MicMac is an open source software specialized in image matching
    for research in geographic information. MicMac is built on the
    eLiSe image library. MicMac is governed by the  "Cecill-B licence".
    See below and http://www.cecill.info.

Header-MicMac-eLiSe-25/06/2007*/

#include "StdAfx.h"



cCpleEpip * StdCpleEpip
          (
             std::string  aDir,
             std::string  aNameOri,
             std::string  aNameIm1,
             std::string  aNameIm2
          )
{
    if (aNameIm1 > aNameIm2) ElSwap(aNameIm1,aNameIm2);
    cInterfChantierNameManipulateur * anICNM = cInterfChantierNameManipulateur::BasicAlloc(aDir);

    std::string aNameCam1 =  anICNM->Assoc1To1("NKS-Assoc-Im2Orient@-"+aNameOri,aNameIm1,true);
    std::string aNameCam2 =  anICNM->Assoc1To1("NKS-Assoc-Im2Orient@-"+aNameOri,aNameIm2,true);

    CamStenope * aCam1 = CamStenope::StdCamFromFile(true,aNameCam1,anICNM);
    CamStenope * aCam2 = CamStenope::StdCamFromFile(true,aNameCam2,anICNM);
    return new cCpleEpip (aDir,1,*aCam1,aNameIm1,*aCam2,aNameIm2);

}

class cApply_CreateEpip_main
{
   public :
      cApply_CreateEpip_main(int argc,char ** argv);

     cInterfChantierNameManipulateur * mICNM;


      int    mDegre;
      int    mNbZ ;
      int    mNbXY ;
      double mLengthMin ;
      double mStepReech ;
      bool   mForceGen;

      cBasicGeomCap3D *  mGenI1;
      cBasicGeomCap3D *  mGenI2;
      std::string        mName1;
      std::string        mName2;

      void DoEpipGen();

      Pt2dr DirEpipIm2(cBasicGeomCap3D * aG1,cBasicGeomCap3D * aG2,ElPackHomologue & aPack,bool AddToP1);

      void Ressample(cBasicGeomCap3D * aG1,EpipolaireCoordinate & E1,double aStep);
};

Pt2dr  cApply_CreateEpip_main::DirEpipIm2(cBasicGeomCap3D * aG1,cBasicGeomCap3D * aG2,ElPackHomologue & aPack,bool AddToP1)
{
    Pt2dr aSz =  Pt2dr(aG1->SzBasicCapt3D());
 

    Pt2dr aSomTens2(0,0);
    double aIProf = aG1->GetVeryRoughInterProf();
    double aEps = 5e-4;

    double aLenghtSquare = ElMin(mLengthMin,sqrt((aSz.x*aSz.y) / (mNbXY*mNbXY)));


    int aNbX = ElMax(1+3*mDegre,round_up(aSz.x /aLenghtSquare));
    int aNbY = ElMax(1+3*mDegre,round_up(aSz.y /aLenghtSquare));


    std::cout << "NBBBB " << aNbX << " " << aNbY << "\n";
     

    for (int aKX=0 ; aKX<= aNbX ; aKX++)
    {
        double aPdsX = ElMax(aEps,ElMin(1-aEps,aKX /double(aNbX)));
        for (int aKY=0 ; aKY<= aNbY ; aKY++)
        {
            double aPdsY = ElMax(aEps,ElMin(1-aEps,aKY/double(aNbY)));
            Pt2dr aPIm1 = aSz.mcbyc(Pt2dr(aPdsX,aPdsY));
            if (aG1->CaptHasData(aPIm1))
            {
                Pt3dr aPT1;
                Pt3dr aC1;
                aG1->GetCenterAndPTerOnBundle(aC1,aPT1,aPIm1);

                std::vector<Pt2dr> aVPIm2;
                for (int aKZ = -mNbZ ; aKZ <= mNbZ ; aKZ++)
                {
                     Pt3dr aPT2 = aC1 + (aPT1-aC1) * (1+(aIProf*aKZ) / mNbZ);
                     if (aG1->PIsVisibleInImage(aPT2) && aG2->PIsVisibleInImage(aPT2))
                     {
                        aVPIm2.push_back(aG2->Ter2Capteur(aPT2));
                        ElCplePtsHomologues aCple(aPIm1,aVPIm2.back(),1.0);
                        if (! AddToP1) 
                           aCple.SelfSwap();
                        aPack.Cple_Add(aCple);
                     }
                }
                if (aVPIm2.size() >=2)
                {
                    Pt2dr aDir2 = vunit(aVPIm2.back()-aVPIm2[0]);
                    aSomTens2 = aSomTens2 + aDir2 * aDir2; // On double l'angle pour en faire un tenseur
                }
            }
        }
    }
    Pt2dr aRT = Pt2dr::polar(aSomTens2,0.0);
    return Pt2dr::FromPolar(1.0,aRT.y/2.0);
}

class cTmpReechEpip
{
     public :
        cTmpReechEpip(const std::string &,Box2dr aBoxImIn,ElDistortion22_Gen * anEpi,Box2dr aBoxOut,double aStep,const std::string & aNameOut);
     private :
        Box2dr                 mBoxImIn;
        ElDistortion22_Gen *   mEpi;
        double                 mStep;
        Pt2dr                  mP0;
        Pt2di                  mSzEpi;
        Pt2di                  mSzRed;

        Pt2dr ToFullEpiCoord(const Pt2dr & aP)
        {
            return mP0 + aP * mStep;
        }
        Pt2dr ToFullEpiCoord(const Pt2di & aP) {return ToFullEpiCoord(Pt2dr(aP));}

        Im2D_Bits<1> mRedIMasq;
        TIm2DBits<1> mRedTMasq;

        Im2D_REAL4         mRedImX;
        TIm2D<REAL4,REAL8> mRedTImX;
        Im2D_REAL4         mRedImY;
        TIm2D<REAL4,REAL8> mRedTImY;
};


cTmpReechEpip::cTmpReechEpip
(
        const std::string & aNameOri,
        Box2dr aBoxImIn,
        ElDistortion22_Gen * anEpi,
        Box2dr aBox,
        double aStep,
        const std::string & aNameOut
) :
    mBoxImIn(aBoxImIn),
    mEpi    (anEpi),
    mStep   (aStep),
    mP0     (aBox._p0),
    mSzEpi  (aBox.sz()),
    mSzRed  (round_up (aBox.sz() / aStep) + Pt2di(1,1)),
    mRedIMasq  (mSzRed.x,mSzRed.y,0),
    mRedTMasq  (mRedIMasq),
    mRedImX    (mSzRed.x,mSzRed.y),
    mRedTImX   (mRedImX),
    mRedImY    (mSzRed.x,mSzRed.y),
    mRedTImY   (mRedImY)
{

    int aNbSC = 5;

    cSinCardApodInterpol1D aKer(cSinCardApodInterpol1D::eTukeyApod,aNbSC,aNbSC/2,1e-4,false);
    cTabIM2D_FromIm2D<REAL4>   aSCI (&aKer,1000,false);



    Pt2di aPInd;

    for (aPInd.x=0 ; aPInd.x<mSzRed.x ; aPInd.x++)
    {
       for (aPInd.y=0 ; aPInd.y<mSzRed.y ; aPInd.y++)
       {
          bool Ok= false;
          Pt2dr aPEpi = ToFullEpiCoord(aPInd);
          Pt2dr aPIm =  anEpi->Inverse(aPEpi);
          if ((aPIm.x>mBoxImIn._p0.x) && (aPIm.y>mBoxImIn._p0.y) && (aPIm.x<mBoxImIn._p1.x) && (aPIm.y<mBoxImIn._p1.y))
          {
               Pt2dr aPEpi2 = anEpi->Direct(aPIm);
               if (euclid(aPEpi-aPEpi2) < 1e-2)
               {
                    Ok= true;
                    mRedTMasq.oset(aPInd,Ok);
               }
          }
          mRedTImX.oset(aPInd,aPIm.x);
          mRedTImY.oset(aPInd,aPIm.y);
       }
    }
    ELISE_COPY(mRedIMasq.all_pts(),dilat_d8(mRedIMasq.in(0),4),mRedIMasq.out());


    Tiff_Im aTifOri = Tiff_Im::StdConvGen(aNameOri.c_str(),-1,true);
    Tiff_Im aTifEpi
            (
                aNameOut.c_str(),
                mSzEpi,
                aTifOri.type_el(),
                Tiff_Im::No_Compr,
                aTifOri.phot_interp()
            );
    std::string aNameMasq = StdPrefix(aNameOut)+"_Masq.tif";
    Tiff_Im aTifMasq
            (
                aNameMasq.c_str(),
                mSzEpi,
                GenIm::bits1_msbf,
                Tiff_Im::No_Compr,
                Tiff_Im::BlackIsZero
            );





    int aNbBloc=2000;
    int aBrd = aNbSC+3;
    Pt2di aSzBrd(aBrd,aBrd);

    for (int aX0=0 ; aX0<mSzEpi.x ; aX0+=aNbBloc)
    {
         int aX1 = ElMin(aX0+aNbBloc,mSzEpi.x);
         for (int aY0=0 ; aY0<mSzEpi.y ; aY0+=aNbBloc)
         {
             int aY1 = ElMin(aY0+aNbBloc,mSzEpi.y);

             Pt2di aP0Epi(aX0,aY0);
             Pt2di aSzBloc(aX1-aX0,aY1-aY0);

             TIm2D<REAL4,REAL8> aTImX(aSzBloc);
             TIm2D<REAL4,REAL8> aTImY(aSzBloc);
             TIm2DBits<1>       aTImMasq(aSzBloc,0);

             Pt2dr aInfIm(1e20,1e20);
             Pt2dr aSupIm(-1e20,-1e20);
             bool  NonVide= false;

             for (int anX =aX0 ; anX<aX1  ; anX++)
             {
                 for (int anY =aY0 ; anY<aY1  ; anY++)
                 {
                     Pt2dr aIndEpi (anX/mStep , anY/mStep);
                     Pt2di aPIndLoc (anX-aX0,anY-aY0);
                     if (mRedTMasq.get(round_down(aIndEpi)))
                     {
                        double aXIm = mRedTImX.getr(aIndEpi,-1,true);
                        double aYIm = mRedTImY.getr(aIndEpi,-1,true);

                        if ((aXIm>0) && (aYIm>0))
                        {
                            // aTImMasq.oset(aPIndLoc,1);
                            aTImX.oset(aPIndLoc,aXIm);
                            aTImY.oset(aPIndLoc,aYIm);

                            aInfIm  = Inf(aInfIm,Pt2dr(aXIm,aYIm));
                            aSupIm  = Sup(aSupIm,Pt2dr(aXIm,aYIm));
                            NonVide= true;
                        }
                     }
                 }
             }
             if (NonVide)
             {
                 Pt2di aP0BoxIm = Sup(Pt2di(0,0),Pt2di(round_down(aInfIm) - aSzBrd));
                 Pt2di aP1BoxIm = Inf(aTifOri.sz(),Pt2di(round_down(aSupIm) + aSzBrd));
                 Pt2di aSzIm = aP1BoxIm - aP0BoxIm;

                 // std::vector<Im2D_REAL4>  aVIm;

                 std::vector<Im2D_REAL4>  aVIm= aTifOri.VecOfImFloat(aSzIm);

                 ELISE_COPY
                 (
                     rectangle(Pt2di(0,0),aSzIm),
                     trans(aTifOri.in(),aP0BoxIm),
                     StdOut(aVIm)
                 );

                 std::vector<Im2D_REAL4>  aVImEpi = aTifEpi.VecOfImFloat(aSzBloc);
                 ELISE_ASSERT(aVImEpi.size()==aVIm.size(),"Incohe in nb chan, cTmpReechEpip::cTmpReechEpip");

                 for (int aKIm=0 ; aKIm <int(aVImEpi.size()) ; aKIm++)
                 {
                      TIm2D<REAL4,REAL8> aImEpi(aVImEpi[aKIm]);
                      REAL4 ** aDataOri = aVIm[aKIm].data();
                      for (int anX =0 ; anX<aSzBloc.x ; anX++)
                      {
                           for (int anY =0 ; anY<aSzBloc.y ; anY++)
                           {

                               Pt2di aIndEpi(anX,anY);
                               aImEpi.oset(aIndEpi,0);
                               Pt2di anIndEpiGlob  = aIndEpi + aP0Epi;

                               Pt2dr aIndEpiRed (anIndEpiGlob.x/mStep , anIndEpiGlob.y/mStep);
                               if (mRedTMasq.get(round_down(aIndEpiRed),0))
                               {
                                   double aXIm = mRedTImX.getr(aIndEpiRed,-1,true);
                                   double aYIm = mRedTImY.getr(aIndEpiRed,-1,true);
                                   Pt2dr aPImLoc = Pt2dr(aXIm,aYIm) - Pt2dr(aP0BoxIm);
                                   double aV= 128;
                                   if ((aPImLoc.x>aNbSC+1) && (aPImLoc.y>aNbSC+1) && (aPImLoc.x<aSzIm.x-aNbSC-2) && (aPImLoc.y<aSzIm.y-aNbSC-2))
                                   {
                                       aTImMasq.oset(aIndEpi,1);
                                       aV = aSCI.GetVal(aDataOri,aPImLoc);
                                       // aV= 255;
                                   }
                                   aImEpi.oset(aIndEpi,aV);
                               }
                           }
                      }
                 }
                 ELISE_COPY
                 (
                     rectangle(aP0Epi,aP0Epi+aSzBloc),
                     Tronque(aTifEpi.type_el(),trans(StdInput(aVImEpi),-aP0Epi)),
                     aTifEpi.out()
                 );
             }
             ELISE_COPY
             (
                 rectangle(aP0Epi,aP0Epi+aSzBloc),
                 trans(aTImMasq._the_im.in(0),-aP0Epi),
                 aTifMasq.out()
             );
             std::cout << "ReechDONE " <<  aX0 << " "<< aY0 << "\n";
         }
    }
}





void cApply_CreateEpip_main::DoEpipGen()
{
      mLengthMin = 500.0;
      mStepReech = 10.0;
      mNbZ       = 1;
      mNbXY      = 100;
      ElPackHomologue aPack;
      Pt2dr aDir2 =  DirEpipIm2(mGenI1,mGenI2,aPack,true);
      Pt2dr aDir1 =  DirEpipIm2(mGenI2,mGenI1,aPack,false);

      std::cout << "Compute Epip\n";
      CpleEpipolaireCoord * aCple = CpleEpipolaireCoord::PolynomialFromHomologue(false,aPack,mDegre,aDir1,aDir2);

      EpipolaireCoordinate & e1 = aCple->EPI1();
      EpipolaireCoordinate & e2 = aCple->EPI2();

      Pt2dr aInf1(1e20,1e20),aSup1(-1e20,-1e20);
      Pt2dr aInf2(1e20,1e20),aSup2(-1e20,-1e20);

      double aErrMax = 0.0;
      double aErrMoy = 0.0;
      int    mNbP = 0;
      double aX2mX1 = 0.0;

      CpleEpipolaireCoord * aCple3 = 0;
      CpleEpipolaireCoord * aCple7 = 0;
      if (0)
      {
         aCple3 = CpleEpipolaireCoord::PolynomialFromHomologue(false,aPack,3,aDir1,aDir2);
         aCple7 = CpleEpipolaireCoord::PolynomialFromHomologue(false,aPack,7,aDir1,aDir2);
      }

      double aErrMaxDir1 = 0.0;
      double aErrMaxInv1 = 0.0;

      for (ElPackHomologue::const_iterator itC=aPack.begin() ; itC!= aPack.end() ; itC++)
      {
           Pt2dr aP1 = e1.Direct(itC->P1());
           Pt2dr aP2 = e2.Direct(itC->P2());
           aInf1 = Inf(aInf1,aP1);
           aSup1 = Sup(aSup1,aP1);
           aInf2 = Inf(aInf2,aP2);
           aSup2 = Sup(aSup2,aP2);

           aX2mX1 += aP2.x - aP1.x;

           double anErr = ElAbs(aP1.y-aP2.y);
           mNbP++;
           aErrMax = ElMax(anErr,aErrMax);
           aErrMoy += anErr;

           if (aCple3)
           {
               Pt2dr aQ1D3 = aCple3->EPI1().Direct(itC->P1());
               Pt2dr aQ1D7 = aCple7->EPI1().Direct(itC->P1());
               double aDQ1 = euclid(aQ1D3-aQ1D7);
               aErrMaxDir1 = ElMax(aDQ1,aErrMaxDir1);

               Pt2dr aR1D3 =  aCple3->EPI1().Inverse(aQ1D3);
               Pt2dr aR1D7 =  aCple7->EPI1().Inverse(aQ1D7);
               double aDR1 = euclid(aR1D3-aR1D7);
               aErrMaxInv1 = ElMax(aDR1,aErrMaxInv1);

           }

      }
      if (aCple3)
      {
         std::cout << "MAX ER " << aErrMaxDir1 << " " << aErrMaxInv1 << "\n";
      }

      aX2mX1 /= mNbP;

      double aInfY = ElMax(aInf1.y,aInf2.y);
      double aSupY = ElMax(aSup1.y,aSup2.y);
      aInf1.y = aInf2.y = aInfY;
      aSup1.y = aSup2.y = aSupY;

      std::cout << aInf1 << " " <<  aSup1 << "\n";
      std::cout << aInf2 << " " <<  aSup2 << "\n";
      std::cout << "DIR " << aDir1 << " " << aDir2 << " X2-X1 " << aX2mX1<< "\n";
      std::cout << "Epip Rect Accuracy, Moy " << aErrMoy/mNbP << " Max " << aErrMax << "\n";


      cTmpReechEpip aReech1(mName1,Box2dr(Pt2dr(0,0),Pt2dr(mGenI1->SzBasicCapt3D())),&e1,Box2dr(aInf1,aSup1),mStepReech,"ImEpi1.tif");
      std::cout << "DONE IM1 \n";
      cTmpReechEpip aReech2(mName2,Box2dr(Pt2dr(0,0),Pt2dr(mGenI2->SzBasicCapt3D())),&e2,Box2dr(aInf2,aSup2),mStepReech,"ImEpi2.tif");
      std::cout << "DONE IM2 \n";

      std::cout << "DONNE REECH TMP \n";

//  ::cTmpReechEpip(cBasicGeomCap3D * aGeom,EpipolaireCoordinate * anEpi,Box2dr aBox,double aStep) :
}


    // std::string  aNameEpi = Is1?"ImEpi1.tif":"ImEpi2.tif";



cApply_CreateEpip_main::cApply_CreateEpip_main(int argc,char ** argv) :
   mForceGen (false)
{
    Tiff_Im::SetDefTileFile(50000);
    std::string aDir= ELISE_Current_DIR;
    std::string anOri;
    double  aScale=1.0;

    bool Gray = true;
    bool Cons16B = true;
    bool InParal = true;
    bool DoIm = true;
    std::string aNameHom;


    ElInitArgMain
    (
    argc,argv,
    LArgMain()  << EAMC(mName1,"Name first image", eSAM_IsExistFile)
                << EAMC(mName2,"Name second image", eSAM_IsExistFile)
                << EAMC(anOri,"Name orientation", eSAM_IsExistDirOri),
    LArgMain()  << EAM(aScale,"Scale",true)
                << EAM(aDir,"Dir",true,"directory (Def=current)", eSAM_IsDir)
                    << EAM(Gray,"Gray",true,"One channel gray level image (Def=true)", eSAM_IsBool)
                    << EAM(Cons16B,"16B",true,"Maintain 16 Bits images if avalaible (Def=true)", eSAM_IsBool)
                    << EAM(InParal,"InParal",true,"Compute in parallel (Def=true)", eSAM_IsBool)
                    << EAM(DoIm,"DoIm",true,"Compute image (def=true !!)", eSAM_IsBool)
                    << EAM(aNameHom,"NameH",true,"Extension to compute Hom point in epi coord (def=none)", eSAM_NoInit)
                    << EAM(mDegre,"Degre",true,"Degre of polynom to correct epi (def=1-, ,2,3)")
                    << EAM(mForceGen,"FG",true,"Force generik epip even with stenope cam")
    );

    if (!MMVisualMode)
    {
    if (mName1 > mName2) ElSwap(mName1,mName2);



    int aNbChan = Gray ? 1 : - 1;

    cTplValGesInit<std::string>  aTplFCND;
    mICNM = cInterfChantierNameManipulateur::StdAlloc
                                               (
                                                   argc,argv,
                                                   aDir,
                                                   aTplFCND
                                               );
     mICNM->CorrecNameOrient(anOri);


     mGenI1 = mICNM->StdCamGenOfNames(anOri,mName1);
     mGenI2 = mICNM->StdCamGenOfNames(anOri,mName2);

     if ((mGenI1->DownCastCS()==0) || (mGenI2->DownCastCS()==0) || mForceGen)
     {
         if (! EAMIsInit(&mDegre)) mDegre = 9;
         DoEpipGen();
         return;
     }

     std::string   aKey =  + "NKS-Assoc-Im2Orient@-" + anOri;

     std::string aNameOr1 = mICNM->Assoc1To1(aKey,mName1,true);
     std::string aNameOr2 = mICNM->Assoc1To1(aKey,mName2,true);

     // std::cout << "RREEEEEEEEEEEEEEead cam \n";
     CamStenope * aCam1 = CamStenope::StdCamFromFile(true,aNameOr1,mICNM);
     // std::cout << "EPISZPPPpp " << aCam1->SzPixel() << "\n";

     CamStenope * aCam2 = CamStenope::StdCamFromFile(true,aNameOr2,mICNM);

     Tiff_Im aTif1 = Tiff_Im::StdConvGen(aDir+mName1,aNbChan,Cons16B);
     Tiff_Im aTif2 = Tiff_Im::StdConvGen(aDir+mName2,aNbChan,Cons16B);



      // aCam1->SetSz(aTif1.sz(),true);
      // aCam2->SetSz(aTif2.sz(),true);

// for (int aK=0; aK<13 ; aK++) std::cout << "SSSssssssssssssssssssiize !!!!\n"; getchar();

  //  Test commit


     cCpleEpip aCplE
               (
                    aDir,
                    aScale,
                    *aCam1,mName1,
                    *aCam2,mName2
               );

     const char * aCarHom = 0;
     if (EAMIsInit(&aNameHom))
        aCarHom = aNameHom.c_str();

     std::cout << "TimeEpi-0 \n";
     ElTimer aChrono;
     aCplE.ImEpip(aTif1,aNameOr1,true,InParal,DoIm,aCarHom,mDegre);
     std::cout << "TimeEpi-1 " << aChrono.uval() << "\n";
     aCplE.ImEpip(aTif2,aNameOr2,false,InParal,DoIm,aCarHom,mDegre);
     std::cout << "TimeEpi-2 " << aChrono.uval() << "\n";

     aCplE.SetNameLock("End");
     aCplE.LockMess("End cCpleEpip::ImEpip");


     return ;

    }
    else return ;
}


int CreateEpip_main(int argc,char ** argv)
{
     cApply_CreateEpip_main(argc,argv);

     return EXIT_SUCCESS;
}


/*Footer-MicMac-eLiSe-25/06/2007

Ce logiciel est un programme informatique servant �  la mise en
correspondances d'images pour la reconstruction du relief.

Ce logiciel est régi par la licence CeCILL-B soumise au droit français et
respectant les principes de diffusion des logiciels libres. Vous pouvez
utiliser, modifier et/ou redistribuer ce programme sous les conditions
de la licence CeCILL-B telle que diffusée par le CEA, le CNRS et l'INRIA
sur le site "http://www.cecill.info".

En contrepartie de l'accessibilité au code source et des droits de copie,
de modification et de redistribution accordés par cette licence, il n'est
offert aux utilisateurs qu'une garantie limitée.  Pour les mêmes raisons,
seule une responsabilité restreinte pèse sur l'auteur du programme,  le
titulaire des droits patrimoniaux et les concédants successifs.

A cet égard  l'attention de l'utilisateur est attirée sur les risques
associés au chargement,  �  l'utilisation,  �  la modification et/ou au
développement et �  la reproduction du logiciel par l'utilisateur étant
donné sa spécificité de logiciel libre, qui peut le rendre complexe �
manipuler et qui le réserve donc �  des développeurs et des professionnels
avertis possédant  des  connaissances  informatiques approfondies.  Les
utilisateurs sont donc invités �  charger  et  tester  l'adéquation  du
logiciel �  leurs besoins dans des conditions permettant d'assurer la
sécurité de leurs systèmes et ou de leurs données et, plus généralement,
�  l'utiliser et l'exploiter dans les mêmes conditions de sécurité.

Le fait que vous puissiez accéder �  cet en-tête signifie que vous avez
pris connaissance de la licence CeCILL-B, et que vous en avez accepté les
termes.
Footer-MicMac-eLiSe-25/06/2007*/
