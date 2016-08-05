#ifndef INITOUTIL_H
#define INITOUTIL_H

#include <stdio.h>
#include "StdAfx.h"
#include "Triangle.h"
#include "Pic.h"

/* ** PlyFile.h est maintenante inclus dans StdAfx.f du MicMac, dans include/general */
/*
 * *IL FAULT MISE INCLUDE DU OPENCV AVANT INCLUDE DU StdAfx.h
 * IL FAULT DESACTIVE L'OPTION WITH_HEADER_PRECOMP DANS MICMAC CMAKE
 * Si il trouve pas OpenCV sharedlib => export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
 */

typedef struct CplPic
{
    pic * pic1;
    pic * pic2;
}CplPic;

extern void dispTriSurImg(Tri2d TriMaitre, pic * ImgMaitre ,Tri2d Tri2nd, pic * Img2nd, Pt2dr centre, double size, vector<Pt2dr> & listPtsInteret, bool dispAllPtsInteret = false);
extern Video_Win * display_image( Im2D<U_INT1,INT4> *ImgIm2D,
                    string nameFenetre, Video_Win *thisVWin, double zoomF=1);
extern void display_2image( Im2D<U_INT1,INT4> *Img1,
                            Im2D<U_INT1,INT4> *Img2,
                            Video_Win *Win1, Video_Win *Win2,
                            double zoomF);
extern Video_Win * draw_polygon_onVW(vector<Pt2dr> pts, Video_Win* VW, Pt3di color=Pt3di(0,255,0), bool isFerme = true);
extern Video_Win * draw_polygon_onVW(Tri2d &aTri, Video_Win* VW, Pt3di color=Pt3di(0,255,0), bool isFerme = true);
extern Video_Win* draw_polygon_onVW(Pt2dr ptHGCaree, int szCaree, Video_Win* VW, Pt3di color=Pt3di(0,255,0), bool isFerme = true);
extern vector<double> parse_dParam(vector<string> dParam);


extern std::string intToString ( int number );
class InitOutil
{
	public:
        InitOutil           (string aFullPattern, string aOriInput,
                             string aTypeD, vector<double> aParamD,
                             string aHomolOutput,
                             int SzPtCorr, int SzAreaCorr,
                             double corl_seuil_glob, double corl_seuil_pt,
                             bool disp, bool aCplPicExistHomol);
        PlyFile* read_file  (string pathPlyFileS);
        vector<pic*> load_Im();
        vector<triangle *> load_tri ();
        void reprojectAllTriOnAllImg();
        void initHomoPackVide(bool creatLikeHomoPackInit);
        void initAll(string pathPlyFileS);
        vector<pic*> getmPtrListPic(){return mPtrListPic;}
        vector<triangle*> getmPtrListTri(){return mPtrListTri;}
		
        string getPrivMember(string aName);
        cInterfChantierNameManipulateur * getPrivmICNM(){return mICNM;}
        vector<double> getParamD() {return mParamD;}

        void addToExistHomolFile(   pic * pic1,
                                    pic * pic2,
                                    vector<ElCplePtsHomologues> ptsHomo,
                                    string aHomolOut);
        void writeToHomolFile(   pic * pic1,
                                 pic * pic2,
                                 vector<ElCplePtsHomologues> ptsHomo,
                                 string aHomolOut);
        vector<CplPic> getmCplHomolExist(){return mCplHomolExist;}
        vector<CplPic> loadCplPicExistHomol();

        int mSzPtCorr;
        int mSzAreaCorr;
        double mCorl_seuil_glob;
        double mCorl_seuil_pt;
        bool mDisp;
	private:
        string mOriInput;
        string mFullPattern;
        string mDirImages, mPatIm;
        string mHomolOutput, mNameHomol, mKHOut, mKHOutDat, mKHIn;
        cInterfChantierNameManipulateur * mICNM;
        string mTypeD; vector<double> mParamD;

        vector<Face*> mFaceList;         /* intensite, list vert_ind, n° vertex ind*/
        vector<Vertex*> mVertexList;    /* 3-space position of a vertex */
        PlyFile *mPly;

        vector<string> mSetIm;
        vector<pic*> mPtrListPic;
        vector<triangle*> mPtrListTri;
        vector<CplPic> mCplHomolExist;
        bool mCplPicExistHomol;
};


#endif
