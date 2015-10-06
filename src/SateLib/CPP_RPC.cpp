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
#include <algorithm>
#include "hassan/reechantillonnage.h"
#include "RPC.h"

//Important note:
//pt.x is either the column in image space or the longitude in geographic coordinates or the easting  in projected coordinates
//pt.y is either the row    in image space or the latitude  in geographic coordinates or the northing in projected coordinates

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                             Function for RPC3D                                             //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//RPC2Grid transforms a loaded RPC to a .GRI (and GRIBin) file
int RPC::RPC2Grid(int nbLayers, int altiMin, int altiMax, std::string refineCoef, std::string aNameIm, double stepPixel, double stepCarto, std::string targetSyst, std::string inputSyst, bool binaire)
{
    //Creation d'un dossier pour les fichiers intermediaires
    ELISE_fp::MkDirSvp("processing");

    // fichier GRID en sortie
    std::string aNameFileGrid = StdPrefix(aNameIm) + ".GRI";

    std::vector<double> vAltitude;
    for (int i = 0; i<nbLayers; ++i)
        vAltitude.push_back(altiMin + i*(altiMax - altiMin) / (nbLayers - 1));

	if (refineCoef == "")
	{
		refineCoef = "processing/refineCoef.txt";

		//Creation du fichier de coef par defaut (grille non affinee)
		std::ofstream ficWrite(refineCoef.c_str());
		ficWrite << std::setprecision(15);
		ficWrite << 0 << " " << 1 << " " << 0 << " " << 0 << " " << 0 << " " << 1 << " " << std::endl;
	}

    //recuperation des coefficients pour affiner le modele
    std::vector<double> vRefineCoef;
    std::ifstream ficRead(refineCoef.c_str());
    while (!ficRead.eof() && ficRead.good())
    {
        double a0, a1, a2, b0, b1, b2;
        ficRead >> a0 >> a1 >> a2 >> b0 >> b1 >> b2;

        if (ficRead.good())
        {
            vRefineCoef.push_back(a0);
            vRefineCoef.push_back(a1);
            vRefineCoef.push_back(a2);
            vRefineCoef.push_back(b0);
            vRefineCoef.push_back(b1);
            vRefineCoef.push_back(b2);
        }
    }
    std::cout << "coef " << vRefineCoef[0] << " " << vRefineCoef[1] << " " << vRefineCoef[2]
    << " " << vRefineCoef[3] << " " << vRefineCoef[4] << " " << vRefineCoef[5] << " " << std::endl;


    //Test si le modele est affine pour l'appellation du fichier de sortie
    bool refine = false;
    double noRefine[] = { 0, 1, 0, 0, 0, 1 };

    for (int i = 0; i<6; i++)
    {
        if (vRefineCoef[i] != noRefine[i])
            refine = true;
    }

    if (refine)
    {
        //Effacement du fichier de coefficients (affinite=identite) par defaut
        if (ifstream(refineCoef.c_str())) ELISE_fp::RmFile(refineCoef.c_str());

        //New folder
        std::string dir = "refine_" + StdPrefix(aNameIm);
        ELISE_fp::MkDirSvp(dir);

        std::cout << "Model is affine" << std::endl;
        aNameFileGrid = dir + ELISE_CAR_DIR + aNameFileGrid;
    }

    clearing(aNameFileGrid, refine);
    createGrid(aNameFileGrid, aNameIm,
        stepPixel, stepCarto,
        vAltitude, targetSyst, inputSyst, vRefineCoef);

    if (binaire)
    {
        string cmd = MMDir() + "bin/mm3d Gri2Bin " + aNameFileGrid + " " + aNameFileGrid + "Bin";
        system_call(cmd.c_str());
    }
    return EXIT_SUCCESS;
}

//From Image coordinates to geographic
Pt3dr RPC::DirectRPC(Pt3dr Pimg)const
{
    Pt3dr PimgNorm;
    //Converting into normalized coordinates
    PimgNorm.x = (Pimg.x - samp_off) / samp_scale;
    PimgNorm.y = (Pimg.y - line_off) / line_scale;
    PimgNorm.z = (Pimg.z - height_off) / height_scale;

    //Applying direct RPC
    Pt3dr PgeoNorm = DirectRPCNorm(PimgNorm);

    //Converting into real coordinates
    Pt3dr Pgeo;
	Pgeo.x = PgeoNorm.x * long_scale + long_off;
	Pgeo.y = PgeoNorm.y * lat_scale + lat_off;
    Pgeo.z = PgeoNorm.z * height_scale + height_off;

    return Pgeo;
}

Pt3dr RPC::DirectRPCNorm(Pt3dr PimgNorm)const
    {
    double X = PimgNorm.x, Y = PimgNorm.y, Z = PimgNorm.z;
	double vecteurD[] = { 1, X, Y, Z, Y*X, X*Z, Y*Z, X*X, Y*Y, Z*Z, X*Y*Z, X*X*X, Y*Y*X, X*Z*Z, X*X*Y, Y*Y*Y, Y*Z*Z, X*X*Z, Y*Y*Z, Z*Z*Z };
	//double vecteurD[] = { 1, Y, X, Z, X*Y, Y*Z, X*Z, Y*Y, X*X, Z*Z, Y*X*Z, Y*Y*Y, X*X*Y, Y*Z*Z, Y*Y*X, X*X*X, X*Z*Z, Y*Y*Z, X*X*Z, Z*Z*Z };\\Mistake in pleiades doc section C.3.1

    double long_den = 0.;
    double long_num = 0.;
    double lat_den = 0.;
    double lat_num = 0.;

    for (int i = 0; i<20; i++)
	{
		long_num += vecteurD[i] * direct_samp_num_coef[i];
		long_den += vecteurD[i] * direct_samp_den_coef[i];
        lat_num += vecteurD[i] * direct_line_num_coef[i];
        lat_den += vecteurD[i] * direct_line_den_coef[i];
    }

    //Final computation
    Pt3dr PgeoNorm;
    if ((lat_den != 0) && (long_den != 0))
    {
		PgeoNorm.x = (long_num / long_den);
		PgeoNorm.y = (lat_num / lat_den);
        PgeoNorm.z = Z;
    }
    else
    {
        std::cout << "Computing error - denominator = 0" << std::endl;
    }
    return PgeoNorm;
}

//From geographic (LONG, LAT, Z) coordinates to image
Pt3dr RPC::InverseRPC(Pt3dr Pgeo, std::vector<double> vRefineCoef)const
{
    Pt3dr PgeoNorm;
    //Converting into normalized coordinates
    PgeoNorm.x = (Pgeo.x - long_off) / long_scale;
    PgeoNorm.y = (Pgeo.y - lat_off) / lat_scale;
    PgeoNorm.z = (Pgeo.z - height_off) / height_scale;

    //Applying inverse RPC
    Pt3dr PimNorm = InverseRPCNorm(PgeoNorm);

    ///Converting into Real Coordinates
    Pt3dr Pimg;
    Pimg.x = PimNorm.x * samp_scale + samp_off;
    Pimg.y = PimNorm.y * line_scale + line_off;
    Pimg.z = Pgeo.z;

    //If the 'refining' coefficients are available
    Pt3dr PimgRefined;
    if(vRefineCoef.size())
       PimgRefined = ptRefined(Pimg, vRefineCoef);
    else
       PimgRefined = Pimg;

    return PimgRefined;
}

Pt3dr RPC::InverseRPCNorm(Pt3dr PgeoNorm)const
{
	double X = PgeoNorm.x, Y = PgeoNorm.y, Z = PgeoNorm.z; 
	double vecteurD[] = { 1, X, Y, Z, Y*X, X*Z, Y*Z, X*X, Y*Y, Z*Z, X*Y*Z, X*X*X, Y*Y*X, X*Z*Z, X*X*Y, Y*Y*Y, Y*Z*Z, X*X*Z, Y*Y*Z, Z*Z*Z };
    double samp_den = 0.;
    double samp_num = 0.;
    double line_den = 0.;
    double line_num = 0.;

    for (int i = 0; i<20; i++)
    {
        line_num += vecteurD[i] * inverse_line_num_coef[i];
        line_den += vecteurD[i] * inverse_line_den_coef[i];
        samp_num += vecteurD[i] * inverse_samp_num_coef[i];
        samp_den += vecteurD[i] * inverse_samp_den_coef[i];
    }
    //Final computation
    Pt3dr PimgNorm;
    if ((samp_den != 0) && (line_den != 0))
    {
        PimgNorm.x = (samp_num / samp_den);
        PimgNorm.y = (line_num / line_den);
        PimgNorm.z = PgeoNorm.z;
    }
    else
    {
        std::cout << "Computing error - denominator = 0" << std::endl;
    }
    return PimgNorm;
}

void RPC::ReadDimap(std::string const &filename)
{
    direct_samp_num_coef.clear();
    direct_samp_den_coef.clear();
    direct_line_num_coef.clear();
    direct_line_den_coef.clear();

    inverse_samp_num_coef.clear();
    inverse_samp_den_coef.clear();
    inverse_line_num_coef.clear();
    inverse_line_den_coef.clear();

    cElXMLTree tree(filename.c_str());

    {
        std::list<cElXMLTree*> noeuds = tree.GetAll(std::string("Direct_Model"));
        std::list<cElXMLTree*>::iterator it_grid, fin_grid = noeuds.end();


        std::string coefSampN = "SAMP_NUM_COEFF";
        std::string coefSampD = "SAMP_DEN_COEFF";
        std::string coefLineN = "LINE_NUM_COEFF";
        std::string coefLineD = "LINE_DEN_COEFF";

        for (int c = 1; c<21; c++)
        {
            std::stringstream ss;
            ss << "_" << c;
            coefSampN.append(ss.str());
            coefSampD.append(ss.str());
            coefLineN.append(ss.str());
            coefLineD.append(ss.str());
            for (it_grid = noeuds.begin(); it_grid != fin_grid; ++it_grid)
            {
                double value;
                std::istringstream buffer((*it_grid)->GetUnique(coefSampN.c_str())->GetUniqueVal());
                buffer >> value;
                direct_samp_num_coef.push_back(value);
                std::istringstream buffer2((*it_grid)->GetUnique(coefSampD.c_str())->GetUniqueVal());
                buffer2 >> value;
                direct_samp_den_coef.push_back(value);
                std::istringstream buffer3((*it_grid)->GetUnique(coefLineN.c_str())->GetUniqueVal());
                buffer3 >> value;
                direct_line_num_coef.push_back(value);
                std::istringstream buffer4((*it_grid)->GetUnique(coefLineD.c_str())->GetUniqueVal());
                buffer4 >> value;
                direct_line_den_coef.push_back(value);
            }
            coefSampN = coefSampN.substr(0, 14);
            coefSampD = coefSampD.substr(0, 14);
            coefLineN = coefLineN.substr(0, 14);
            coefLineD = coefLineD.substr(0, 14);
        }
        for (it_grid = noeuds.begin(); it_grid != fin_grid; ++it_grid)
        {
            std::istringstream buffer((*it_grid)->GetUnique("ERR_BIAS_X")->GetUniqueVal());
            buffer >> dirErrBiasX;
            std::istringstream bufferb((*it_grid)->GetUnique("ERR_BIAS_Y")->GetUniqueVal());
            bufferb >> dirErrBiasY;
        }
    }

    {
        std::list<cElXMLTree*> noeudsInv = tree.GetAll(std::string("Inverse_Model"));
        std::list<cElXMLTree*>::iterator it_gridInd, fin_gridInd = noeudsInv.end();

        std::string coefSampN = "SAMP_NUM_COEFF";
        std::string coefSampD = "SAMP_DEN_COEFF";
        std::string coefLineN = "LINE_NUM_COEFF";
        std::string coefLineD = "LINE_DEN_COEFF";

        for (int c = 1; c<21; c++)
        {
            double value;
            std::stringstream ss;
            ss << "_" << c;
            coefSampN.append(ss.str());
            coefSampD.append(ss.str());
            coefLineN.append(ss.str());
            coefLineD.append(ss.str());
            for (it_gridInd = noeudsInv.begin(); it_gridInd != fin_gridInd; ++it_gridInd)
            {
                std::istringstream bufferInd((*it_gridInd)->GetUnique(coefSampN.c_str())->GetUniqueVal());
                bufferInd >> value;
                inverse_samp_num_coef.push_back(value);
                std::istringstream bufferInd2((*it_gridInd)->GetUnique(coefSampD.c_str())->GetUniqueVal());
                bufferInd2 >> value;
                inverse_samp_den_coef.push_back(value);
                std::istringstream bufferInd3((*it_gridInd)->GetUnique(coefLineN.c_str())->GetUniqueVal());
                bufferInd3 >> value;
                inverse_line_num_coef.push_back(value);
                std::istringstream bufferInd4((*it_gridInd)->GetUnique(coefLineD.c_str())->GetUniqueVal());
                bufferInd4 >> value;
                inverse_line_den_coef.push_back(value);
            }
            coefSampN = coefSampN.substr(0, 14);
            coefSampD = coefSampD.substr(0, 14);
            coefLineN = coefLineN.substr(0, 14);
            coefLineD = coefLineD.substr(0, 14);
        }
        for (it_gridInd = noeudsInv.begin(); it_gridInd != fin_gridInd; ++it_gridInd)
        {
            std::istringstream buffer((*it_gridInd)->GetUnique("ERR_BIAS_ROW")->GetUniqueVal());
            buffer >> indirErrBiasRow;
            std::istringstream bufferb((*it_gridInd)->GetUnique("ERR_BIAS_COL")->GetUniqueVal());
            bufferb >> indirErrBiasCol;
        }
    }

    {
        std::list<cElXMLTree*> noeudsRFM = tree.GetAll(std::string("RFM_Validity"));
        std::list<cElXMLTree*>::iterator it_gridRFM, fin_gridRFM = noeudsRFM.end();

        {
            std::list<cElXMLTree*> noeudsInv = tree.GetAll(std::string("Direct_Model_Validity_Domain"));
            std::list<cElXMLTree*>::iterator it_gridInd, fin_gridInd = noeudsInv.end();


            for (it_gridInd = noeudsInv.begin(); it_gridInd != fin_gridInd; ++it_gridInd)
            {
                std::istringstream bufferInd((*it_gridInd)->GetUnique("FIRST_ROW")->GetUniqueVal());
                bufferInd >> first_row;
                std::istringstream bufferInd2((*it_gridInd)->GetUnique("FIRST_COL")->GetUniqueVal());
                bufferInd2 >> first_col;
                std::istringstream bufferInd3((*it_gridInd)->GetUnique("LAST_ROW")->GetUniqueVal());
                bufferInd3 >> last_row;
                std::istringstream bufferInd4((*it_gridInd)->GetUnique("LAST_COL")->GetUniqueVal());
                bufferInd4 >> last_col;
            }
        }


        {
            std::list<cElXMLTree*> noeudsInv = tree.GetAll(std::string("Inverse_Model_Validity_Domain"));
            std::list<cElXMLTree*>::iterator it_gridInd, fin_gridInd = noeudsInv.end();

            for (it_gridInd = noeudsInv.begin(); it_gridInd != fin_gridInd; ++it_gridInd)
            {
                std::istringstream bufferInd((*it_gridInd)->GetUnique("FIRST_LON")->GetUniqueVal());
                bufferInd >> first_lon;
                std::istringstream bufferInd2((*it_gridInd)->GetUnique("FIRST_LAT")->GetUniqueVal());
                bufferInd2 >> first_lat;
                std::istringstream bufferInd3((*it_gridInd)->GetUnique("LAST_LON")->GetUniqueVal());
                bufferInd3 >> last_lon;
                std::istringstream bufferInd4((*it_gridInd)->GetUnique("LAST_LAT")->GetUniqueVal());
                bufferInd4 >> last_lat;
            }
        }
        for (it_gridRFM = noeudsRFM.begin(); it_gridRFM != fin_gridRFM; ++it_gridRFM)
        {
            std::istringstream buffer((*it_gridRFM)->GetUnique("LONG_SCALE")->GetUniqueVal());
            buffer >> long_scale;
            std::istringstream buffer2((*it_gridRFM)->GetUnique("LONG_OFF")->GetUniqueVal());
            buffer2 >> long_off;
            std::istringstream buffer3((*it_gridRFM)->GetUnique("LAT_SCALE")->GetUniqueVal());
            buffer3 >> lat_scale;
            std::istringstream buffer4((*it_gridRFM)->GetUnique("LAT_OFF")->GetUniqueVal());
            buffer4 >> lat_off;
            std::istringstream buffer5((*it_gridRFM)->GetUnique("HEIGHT_SCALE")->GetUniqueVal());
            buffer5 >> height_scale;
            std::istringstream buffer6((*it_gridRFM)->GetUnique("HEIGHT_OFF")->GetUniqueVal());
            buffer6 >> height_off;
            std::istringstream buffer7((*it_gridRFM)->GetUnique("SAMP_SCALE")->GetUniqueVal());
            buffer7 >> samp_scale;
            std::istringstream buffer8((*it_gridRFM)->GetUnique("SAMP_OFF")->GetUniqueVal());
            buffer8 >> samp_off;
            std::istringstream buffer9((*it_gridRFM)->GetUnique("LINE_SCALE")->GetUniqueVal());
            buffer9 >> line_scale;
            std::istringstream buffer10((*it_gridRFM)->GetUnique("LINE_OFF")->GetUniqueVal());
            buffer10 >> line_off;
        }
    }
    IS_DIR_INI=true;
    IS_INV_INI=true;
}

vector<Pt2dr> RPC::empriseCarto(vector<Pt2dr> Pgeo, std::string targetSyst, std::string inputSyst)const
{
    std::ofstream fic("processing/conv_ptGeo.txt");
    fic << std::setprecision(15);
    for (unsigned int i = 0; i<Pgeo.size(); i++)
    {
        fic << Pgeo[i].x << " " << Pgeo[i].y << endl;
    }
    // transformation in the ground coordinate system
    std::string command;
    command = g_externalToolHandler.get("cs2cs").callName() + " " + inputSyst + " +to " + targetSyst + " processing/conv_ptGeo.txt > processing/conv_ptCarto.txt";
    int res = system(command.c_str());
    ELISE_ASSERT(res == 0, " error calling cs2cs in ptGeo2Carto ");
    // loading the coordinate of the converted point
    vector<double> PtsCartox, PtsCartoy;
    std::ifstream fic2("processing/conv_ptCarto.txt");
    while (!fic2.eof() && fic2.good())
    {
        double X, Y, Z;
        fic2 >> X >> Y >> Z;
        if (fic2.good())
        {
            PtsCartox.push_back(X);
            PtsCartoy.push_back(Y);
        }
    }

    vector<Pt2dr> anEmpriseCarto;
    anEmpriseCarto.push_back(Pt2dr(*std::min_element(PtsCartox.begin(), PtsCartox.end()), *std::min_element(PtsCartoy.begin(), PtsCartoy.end())));
    anEmpriseCarto.push_back(Pt2dr(*std::max_element(PtsCartox.begin(), PtsCartox.end()), *std::max_element(PtsCartoy.begin(), PtsCartoy.end())));

    return anEmpriseCarto;
}

Pt3dr RPC::ptRefined(Pt3dr Pimg, std::vector<double> vRefineCoef)const
{
    //Pour calculer les coordonnees affinees d'un point
    Pt3dr pImgRefined;
	//Test for applying 5th degree polynomials
	/*double aPXx2 = 1.68917122212249e-007, aPXx3 = 1.45693052459939e-010, aPXx4 = -5.24280302379418e-014, aPXx5 = 4.68073194617742e-018;
	double aPXy2 = 2.03369518300494e-006, aPXy3 = -8.19927700050208e-010, aPXy4 = 1.38977078492289e-013, aPXy5 = -7.88660852340546e-018;
	double aSX0 = 0.300943960195233, aSX1 = 296.310168220886, aSX2 = 0.757953969235237; aSX0 = 0;
	double aSY0 = 0, aSY1 = 2100, aSY2 = -M_PI / 3;
	double aSY3 = 0, aSY4 = 300, aSY5 = -M_PI / 3;
	pImgRefined.x = vRefineCoef[0] + Pimg.x * vRefineCoef[1] + Pimg.y * vRefineCoef[2] +
		+aPXx2*pow(Pimg.x, 2) + aPXx3*pow(Pimg.x, 3) + aPXx4*pow(Pimg.x, 4) + aPXx5*pow(Pimg.x, 5) +
		+aPXy2*pow(Pimg.y, 2) + aPXy3*pow(Pimg.y, 3) + aPXy4*pow(Pimg.y, 4) + aPXy5*pow(Pimg.y, 5);// +
			//aSX0 * sin(2 * M_PI  * Pimg.y / aSX1 + aSX2);
	pImgRefined.y = vRefineCoef[3] + Pimg.x * vRefineCoef[4] + Pimg.y * vRefineCoef[5] + aSY0 * sin(2 * M_PI  * Pimg.x / aSY1 + aSY2);
	*/
	pImgRefined.x = vRefineCoef[0] + Pimg.x * vRefineCoef[1] + Pimg.y * vRefineCoef[2];
	pImgRefined.y = vRefineCoef[3] + Pimg.x * vRefineCoef[4] + Pimg.y * vRefineCoef[5];
    pImgRefined.z = Pimg.z;

    return pImgRefined;
}

void RPC::createDirectGrid(double ulcSamp, double ulcLine,
    double stepPixel,
    int nbSamp, int  nbLine,
    std::vector<double> const &vAltitude,
    std::vector<Pt2dr> &vPtCarto, std::string targetSyst, std::string inputSyst,
    std::vector<double> vRefineCoef)const
{
    vPtCarto.clear();
    // On cree un fichier de points geographiques pour les transformer avec proj4
    {
        std::ofstream fic("processing/direct_ptGeo.txt");
        fic << std::setprecision(15);
        for (size_t i = 0; i<vAltitude.size(); ++i)
        {
            double altitude = vAltitude[i];
			for (int l = 0; l<nbLine; ++l)
            {
				for (int c = 0; c<nbSamp; ++c)
                {
                    Pt3dr Pimg(ulcSamp + c * stepPixel, ulcLine + l * stepPixel, altitude);

                    //pour affiner les coordonnees
                    Pt3dr PimgRefined = ptRefined(Pimg, vRefineCoef);

                    Pt3dr Pgeo = DirectRPC(PimgRefined);
                    fic << Pgeo.x << " " << Pgeo.y << std::endl;
                }
            }
        }
    }
    // transformation in the ground coordinate system
    std::string command;
    command = g_externalToolHandler.get("cs2cs").callName() + " " + inputSyst + " +to " + targetSyst + " processing/direct_ptGeo.txt > processing/direct_ptCarto.txt";
    int res = system(command.c_str());
    if (res != 0) std::cout << "error calling cs2cs in createDirectGrid" << std::endl;
    // loading points
    std::ifstream fic("processing/direct_ptCarto.txt");
    while (!fic.eof() && fic.good())
    {
        double X, Y, Z;
        fic >> X >> Y >> Z;
        if (fic.good())
            vPtCarto.push_back(Pt2dr(X, Y));
    }
    std::cout << "Number of points in direct grid : " << vPtCarto.size() << std::endl;
}

void RPC::createInverseGrid(double ulcX, double ulcY, int nbrSamp, int nbrLine,
    double stepCarto, std::vector<double> const &vAltitude,
    std::vector<Pt3dr> &vPtImg, std::string targetSyst, std::string inputSyst,
    std::vector<double> vRefineCoef)const
{
    vPtImg.clear();

    //Creation of a file with points in cartographic coordinates to be transformed with proj4
    {
        std::ofstream fic("processing/inverse_ptCarto.txt");
        fic << std::setprecision(15);
        for (int l = 0; l<nbrLine; ++l)
        {
            double Y = ulcY - l*stepCarto;
            for (int c = 0; c<nbrSamp; ++c)
            {
                double X = ulcX + c*stepCarto;
                fic << X << " " << Y << std::endl;
            }
        }
    }
    // convert to geographic coordinates
    std::string command;
    command = g_externalToolHandler.get("cs2cs").callName() + " " + targetSyst + " +to " + inputSyst + " -f %.12f processing/inverse_ptCarto.txt >processing/inverse_ptGeo.txt";
    int res = system(command.c_str());
    ELISE_ASSERT(res == 0, "error calling cs2cs in createinverseGrid");
    for (size_t i = 0; i<vAltitude.size(); ++i)
    {
        double altitude = vAltitude[i];
        // loading points
        std::ifstream fic("processing/inverse_ptGeo.txt");
        while (!fic.eof() && fic.good())
        {
            double lon, lat, Z;
            fic >> lon >> lat >> Z;
            if (fic.good())
            {
                vPtImg.push_back(InverseRPC(Pt3dr(lon, lat, altitude), vRefineCoef));
            }
        }
    }
    std::cout << "Number of points in inverse grid : " << vPtImg.size() << std::endl;
}

void RPC::createGrid(std::string const &nomGrid, std::string const &nomImage,
    double stepPixel, double stepCarto,
    std::vector<double> vAltitude, std::string targetSyst, std::string inputSyst,
    std::vector<double> vRefineCoef)
{
    double firstSamp = first_col;
    double firstLine = first_row;
    double lastSamp = last_col;
    double lastLine = last_row;

    //Direct nbr Lignes et colonnes + step last ligne et colonne
    int nbLine, nbSamp;
    nbLine = (lastLine - firstLine) / stepPixel + 1;
    nbSamp = (lastSamp - firstSamp) / stepPixel + 1;

    std::vector<Pt2dr> vPtCarto;
    createDirectGrid(firstSamp, firstLine, stepPixel, nbSamp, nbLine, vAltitude, vPtCarto, targetSyst, inputSyst, vRefineCoef);

    // Estimation of the validity domaine in cartographic coordinates
    vector<Pt2dr> cornersGeo;
    cornersGeo.push_back(Pt2dr(first_lon, first_lat));
    cornersGeo.push_back(Pt2dr(first_lon, last_lat));
    cornersGeo.push_back(Pt2dr(last_lon, last_lat));
    cornersGeo.push_back(Pt2dr(last_lon, first_lat));
    vector<Pt2dr> anEmpriseCarto = empriseCarto(cornersGeo, targetSyst, inputSyst);

    //Corners of the validity domaine of the inverse RPC
    Pt2dr urc(anEmpriseCarto[1].x, anEmpriseCarto[1].y);
    Pt2dr llc(anEmpriseCarto[0].x, anEmpriseCarto[0].y);
    std::cout << "Corners of the area : " << llc << " " << urc << std::endl;

    //inverse nbr Lignes et colonnes + step last ligne et colonne
    int nbrLine, nbrSamp;
    nbrSamp = (urc.x - llc.x) / stepCarto + 1;
    nbrLine = (urc.y - llc.y) / stepCarto + 1;

    std::vector<Pt3dr> vPtImg;
    //Calcul des coefficients de l'affinite pour la transformation inverse
    std::vector<double> vRefineCoefInv;

    double A0 = vRefineCoef[0];
    double A1 = vRefineCoef[1];
    double A2 = vRefineCoef[2];
    double B0 = vRefineCoef[3];
    double B1 = vRefineCoef[4];
    double B2 = vRefineCoef[5];

    double det = A1*B2 - A2*B1;

    double IA0 = -A0;
    double IA1 = B2 / det;
    double IA2 = -A2 / det;
    double IB0 = -B0;
    double IB1 = -B1 / det;
    double IB2 = A1 / det;

    vRefineCoefInv.push_back(IA0);
    vRefineCoefInv.push_back(IA1);
    vRefineCoefInv.push_back(IA2);
    vRefineCoefInv.push_back(IB0);
    vRefineCoefInv.push_back(IB1);
    vRefineCoefInv.push_back(IB2);

    createInverseGrid(llc.x, urc.y, nbrSamp, nbrLine, stepCarto, vAltitude, vPtImg,
        targetSyst, inputSyst, vRefineCoefInv);

    //Creating grid and writing flux

    std::ofstream writeGrid(nomGrid.c_str());
    writeGrid << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    writeGrid << "<trans_coord_grid version=\"5\" name=\"\">" << std::endl;
    //creation of the date
    time_t t = time(0);
    struct tm * timeInfo = localtime(&t);
    std::string date;
    std::stringstream ssdate;
    ssdate << timeInfo->tm_year + 1900;
    double adate[] = { (double)timeInfo->tm_mon, (double)timeInfo->tm_mday,
        (double)timeInfo->tm_hour, (double)timeInfo->tm_min, (double)timeInfo->tm_sec };
    std::vector<double> vdate(adate, adate + 5);
    // Formating the date
    for (int ida = 0; ida<5; ida++)
    {
        std::stringstream ssdateTempo;
        std::string dateTempo;
        ssdateTempo << vdate[ida];
        dateTempo = ssdateTempo.str();
        if (dateTempo.length() == 2)
            ssdate << dateTempo;
        else ssdate << 0 << dateTempo;
    }
    date = ssdate.str();
    writeGrid << "\t<date>" << date << "</date>" << std::endl;

    writeGrid << "\t<trans_coord name=\"\">" << std::endl;
    writeGrid << "\t\t<trans_sys_coord name=\"\">" << std::endl;
    writeGrid << "\t\t\t<sys_coord name=\"sys1\">" << std::endl;
    writeGrid << "\t\t\t\t<sys_coord_plani name=\"sys1\">" << std::endl;
    writeGrid << "\t\t\t\t\t<code>" << nomImage << "</code>" << std::endl;
    writeGrid << "\t\t\t\t\t<unit>" << "p" << "</unit>" << std::endl;
    writeGrid << "\t\t\t\t\t<direct>" << "0" << "</direct>" << std::endl;
    writeGrid << "\t\t\t\t\t<sub_code>" << "*" << "</sub_code>" << std::endl;
    writeGrid << "\t\t\t\t\t<vertical>" << nomImage << "</vertical>" << std::endl;
    writeGrid << "\t\t\t\t</sys_coord_plani>" << std::endl;
    writeGrid << "\t\t\t\t<sys_coord_alti name=\"sys1\">" << std::endl;
    writeGrid << "\t\t\t\t\t<code>" << "Unused in MicMac" << "</code>" << std::endl;
    writeGrid << "\t\t\t\t\t<unit>" << "m" << "</unit>" << std::endl;
    writeGrid << "\t\t\t\t</sys_coord_alti>" << std::endl;
    writeGrid << "\t\t\t</sys_coord>" << std::endl;

    writeGrid << "\t\t\t<sys_coord name=\"sys2\">" << std::endl;
    writeGrid << "\t\t\t\t<sys_coord_plani name=\"sys2\">" << std::endl;
    writeGrid << "\t\t\t\t\t<code>" << "Unused in MicMac" << "</code>" << std::endl;
    writeGrid << "\t\t\t\t\t<unit>" << "m" << "</unit>" << std::endl;
    writeGrid << "\t\t\t\t\t<direct>" << "1" << "</direct>" << std::endl;
    writeGrid << "\t\t\t\t\t<sub_code>" << "*" << "</sub_code>" << std::endl;
    writeGrid << "\t\t\t\t\t<vertical>" << "Unused in MicMac" << "</vertical>" << std::endl;
    writeGrid << "\t\t\t\t</sys_coord_plani>" << std::endl;
    writeGrid << "\t\t\t\t<sys_coord_alti name=\"sys2\">" << std::endl;
    writeGrid << "\t\t\t\t\t<code>" << "Unused in MicMac" << "</code>" << std::endl;
    writeGrid << "\t\t\t\t\t<unit>" << "m" << "</unit>" << std::endl;
    writeGrid << "\t\t\t\t</sys_coord_alti>" << std::endl;
    writeGrid << "\t\t\t</sys_coord>" << std::endl;
    writeGrid << "\t\t</trans_sys_coord>" << std::endl;
    writeGrid << "\t\t<category>" << "1" << "</category>" << std::endl;
    writeGrid << "\t\t<type_modele>" << "2" << "</type_modele>" << std::endl;
    writeGrid << "\t\t<direct_available>" << "1" << "</direct_available>" << std::endl;
    writeGrid << "\t\t<inverse_available>" << "1" << "</inverse_available>" << std::endl;
    writeGrid << "\t</trans_coord>" << std::endl;

    // For the direct grid
    writeGrid << "\t<multi_grid version=\"1\" name=\"1-2\" >" << std::endl;
    writeGrid << "\t\t<upper_left>" << std::setprecision(15) << firstSamp << "  " << std::setprecision(15) << firstLine << "</upper_left>" << std::endl;
    writeGrid << "\t\t<columns_interval>" << stepPixel << "</columns_interval>" << std::endl;
    writeGrid << "\t\t<rows_interval>" << "-" << stepPixel << "</rows_interval>" << std::endl;
    writeGrid << "\t\t<columns_number>" << nbSamp << "</columns_number>" << std::endl;
    writeGrid << "\t\t<rows_number>" << nbLine << "</rows_number>" << std::endl;
    writeGrid << "\t\t<components_number>" << "2" << "</components_number>" << std::endl;
    std::vector<Pt2dr>::const_iterator it = vPtCarto.begin();

    for (size_t i = 0; i<vAltitude.size(); ++i)
    {
        std::stringstream ssAlti;
        std::string sAlti;
        ssAlti << std::setprecision(15) << vAltitude[i];
        sAlti = ssAlti.str();
        writeGrid << "\t\t\t<layer value=\"" << sAlti << "\">" << std::endl;

        for (int l = 0; l<nbLine; ++l)
        {
            for (int c = 0; c<nbSamp; ++c)
            {
                Pt2dr const &PtCarto = (*it);
                ++it;
                std::stringstream ssCoord;
                std::string  sCoord;
                ssCoord << std::setprecision(15) << PtCarto.x << "   " << std::setprecision(15) << PtCarto.y;
                sCoord = ssCoord.str();
                writeGrid << "\t\t\t" << sCoord << std::endl;
            }
        }
        writeGrid << "\t\t\t</layer>" << std::endl;
    }
    writeGrid << "\t</multi_grid>" << std::endl;

    // For the inverse grid
    writeGrid << "\t<multi_grid version=\"1\" name=\"2-1\" >" << std::endl;
    writeGrid << "\t\t<upper_left>" << std::setprecision(15) << vPtCarto[0].x << "  " << std::setprecision(15) << vPtCarto[0].y << "</upper_left>" << std::endl;
    writeGrid << "\t\t<columns_interval>" << std::setprecision(15) << stepCarto << "</columns_interval>" << std::endl;
    writeGrid << "\t\t<rows_interval>" << std::setprecision(15) << stepCarto << "</rows_interval>" << std::endl;
    writeGrid << "\t\t<columns_number>" << nbrSamp << "</columns_number>" << std::endl;
    writeGrid << "\t\t<rows_number>" << nbrLine << "</rows_number>" << std::endl;
    writeGrid << "\t\t<components_number>" << "2" << "</components_number>" << std::endl;
    std::vector<Pt3dr>::const_iterator it2 = vPtImg.begin();

    for (size_t i = 0; i<vAltitude.size(); ++i)
    {
        std::stringstream ssAlti;
        std::string sAlti;
        ssAlti << std::setprecision(15) << vAltitude[i];
        sAlti = ssAlti.str();
        writeGrid << "\t\t\t<layer value=\"" << sAlti << "\">" << std::endl;

        for (int l = 0; l<nbrLine; ++l)
        {
            for (int c = 0; c<nbrSamp; ++c)
            {
                Pt3dr const &PtImg = (*it2);
                ++it2;
                std::stringstream ssCoordInv;
                std::string  sCoordInv;
                ssCoordInv << std::setprecision(15) << PtImg.x << "   "
                    << std::setprecision(15) << PtImg.y ;
                sCoordInv = ssCoordInv.str();
                writeGrid << "\t\t\t" << sCoordInv << std::endl;
            }
        }
        writeGrid << "\t\t\t</layer>" << std::endl;
    }
    writeGrid << "\t</multi_grid>" << std::endl;

    writeGrid << "</trans_coord_grid>" << std::endl;
}

void RPC::WriteAirbusRPC(std::string aFileOut)
{
    std::ofstream fic(aFileOut.c_str());
    fic << std::setprecision(16);

    fic << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>" << endl;
    fic << "<Dimap_Document>" << endl;
    fic << "\t<Metadata_Identification>" << endl;
    fic << "\t\t<METADATA_FORMAT version=\"2.0\">DIMAP</METADATA_FORMAT>" << endl;
    fic << "\t\t<METADATA_PROFILE>PHR_SENSOR</METADATA_PROFILE>" << endl;
    fic << "\t\t<METADATA_SUBPROFILE>RPC</METADATA_SUBPROFILE>" << endl;
    fic << "\t\t<METADATA_LANGUAGE>en</METADATA_LANGUAGE>" << endl;
    fic << "\t</Metadata_Identification>" << endl;
    fic << "\t<Rational_Function_Model>" << endl;
    fic << "\t\t<Resource_Reference>" << endl;
    fic << "\t\t\t<RESOURCE_TITLE version=\"2.1\">NITF</RESOURCE_TITLE>" << endl;
    fic << "\t\t\t<RESOURCE_ID>RPC00B</RESOURCE_ID>" << endl;
    fic << "\t\t</Resource_Reference>" << endl;
    fic << "\t\t<Global_RFM>" << endl;
    fic << "\t\t\t<Direct_Model>" << endl;

        for (int i = 0; i<20; i++)
        {
            fic << "\t\t\t\t<SAMP_NUM_COEFF_" << i + 1 << ">" << direct_samp_num_coef[i] << "</SAMP_NUM_COEFF_" << i + 1 << ">" << endl;
        }
        for (int i = 0; i<20; i++)
        {
            fic << "\t\t\t\t<SAMP_DEN_COEFF_" << i + 1 << ">" << direct_samp_den_coef[i] << "</SAMP_DEN_COEFF_" << i + 1 << ">" << endl;
        }
        for (int i = 0; i<20; i++)
        {
            fic << "\t\t\t\t<LINE_NUM_COEFF_" << i + 1 << ">" << direct_line_num_coef[i] << "</LINE_NUM_COEFF_" << i + 1 << ">" << endl;
        }
        for (int i = 0; i<20; i++)
        {
            fic << "\t\t\t\t<LINE_DEN_COEFF_" << i + 1 << ">" << direct_line_den_coef[i] << "</LINE_DEN_COEFF_" << i + 1 << ">" << endl;
        }
        fic << "\t\t\t\t<ERR_BIAS_X>" << dirErrBiasX << "</ERR_BIAS_X>" << endl;
        fic << "\t\t\t\t<ERR_BIAS_Y>" << dirErrBiasY << "</ERR_BIAS_Y>" << endl;
    fic << "\t\t\t</Direct_Model>" << endl;
    fic << "\t\t\t<Inverse_Model>" << endl;
        for (int i = 0; i<20; i++)
        {
            fic << "\t\t\t\t<SAMP_NUM_COEFF_" << i + 1 << ">" << inverse_samp_num_coef[i] << "</SAMP_NUM_COEFF_" << i + 1 << ">" << endl;
        }
        for (int i = 0; i<20; i++)
        {
            fic << "\t\t\t\t<SAMP_DEN_COEFF_" << i + 1 << ">" << inverse_samp_den_coef[i] << "</SAMP_DEN_COEFF_" << i + 1 << ">" << endl;
        }
        for (int i = 0; i<20; i++)
        {
            fic << "\t\t\t\t<LINE_NUM_COEFF_" << i + 1 << ">" << inverse_line_num_coef[i] << "</LINE_NUM_COEFF_" << i + 1 << ">" << endl;
        }
        for (int i = 0; i<20; i++)
        {
            fic << "\t\t\t\t<LINE_DEN_COEFF_" << i + 1 << ">" << inverse_line_den_coef[i] << "</LINE_DEN_COEFF_" << i + 1 << ">" << endl;
        }
        fic << "\t\t\t\t<ERR_BIAS_ROW>" << indirErrBiasRow << "</ERR_BIAS_ROW>" << endl;
        fic << "\t\t\t\t<ERR_BIAS_COL>" << indirErrBiasCol << "</ERR_BIAS_COL>" << endl;
    fic << "\t\t\t</Inverse_Model>" << endl;

    fic << "\t\t\t<RFM_Validity>" << endl;
        fic << "\t\t\t\t<Direct_Model_Validity_Domain>" << endl;
        fic << "\t\t\t\t\t<FIRST_ROW>" << first_row << "</FIRST_ROW>" << endl;
        fic << "\t\t\t\t\t<FIRST_COL>" << first_col << "</FIRST_COL>" << endl;
        fic << "\t\t\t\t\t<LAST_ROW>" << last_row << "</LAST_ROW>" << endl;
        fic << "\t\t\t\t\t<LAST_COL>" << last_col << "</LAST_COL>" << endl;
        fic << "\t\t\t\t</Direct_Model_Validity_Domain>" << endl;
        fic << "\t\t\t\t<Inverse_Model_Validity_Domain>" << endl;
        fic << "\t\t\t\t\t<FIRST_LON>" << first_lon << "</FIRST_LON>" << endl;
        fic << "\t\t\t\t\t<FIRST_LAT>" << first_lat << "</FIRST_LAT>" << endl;
        fic << "\t\t\t\t\t<LAST_LON>" << last_lon << "</LAST_LON>" << endl;
        fic << "\t\t\t\t\t<LAST_LAT>" << last_lat << "</LAST_LAT>" << endl;
        fic << "\t\t\t\t</Inverse_Model_Validity_Domain>" << endl;
        fic << "\t\t\t\t<LONG_SCALE>" << long_scale << "</LONG_SCALE>" << endl;
        fic << "\t\t\t\t<LONG_OFF>" << long_off << "</LONG_OFF>" << endl;
        fic << "\t\t\t\t<LAT_SCALE>" << lat_scale << "</LAT_SCALE>" << endl;
        fic << "\t\t\t\t<LAT_OFF>" << lat_off << "</LAT_OFF>" << endl;
        fic << "\t\t\t\t<HEIGHT_SCALE>" << height_scale << "</HEIGHT_SCALE>" << endl;
        fic << "\t\t\t\t<HEIGHT_OFF>" << height_off << "</HEIGHT_OFF>" << endl;
        fic << "\t\t\t\t<SAMP_SCALE>" << samp_scale << "</SAMP_SCALE>" << endl;
        fic << "\t\t\t\t<SAMP_OFF>" << samp_off << "</SAMP_OFF>" << endl;
        fic << "\t\t\t\t<LINE_SCALE>" << line_scale << "</LINE_SCALE>" << endl;
        fic << "\t\t\t\t<LINE_OFF>" << line_off << "</LINE_OFF>" << endl;
    fic << "\t\t\t</RFM_Validity>" << endl;

    fic << "\t\t</Global_RFM>" << endl;
    fic << "\t</Rational_Function_Model>" << endl;
    fic << "</Dimap_Document>";
}

void RPC::ReconstructValidity()
{
    ReconstructValidity2D();
    ReconstructValidity3D();
}

void RPC::ReconstructValidity2D()
{
   first_row = -1 * line_scale + line_off;
   first_col = -1 * samp_scale + samp_off;
   last_row = 1 * line_scale + line_off;
   last_col = 1 * samp_scale + samp_off;
}

void RPC::ReconstructValidity3D()
{
    first_lon = -1 * long_scale + long_off;
    first_lat = -1 * lat_scale + lat_off;
    first_height = -1 * height_scale + height_off;
    last_lon = 1 * long_scale + long_off;
    last_lat = 1 * lat_scale + lat_off;
    last_height = 1 * height_scale + height_off;
}

void RPC::Validity2Dto3D(RPC2D aRPC2D)
{
	first_lon = aRPC2D.first_lon;
	first_lat = aRPC2D.first_lat;
	first_col = aRPC2D.first_col;
	first_row = aRPC2D.first_row;
	last_lon = aRPC2D.last_lon;
	last_lat = aRPC2D.last_lat;
	last_col = aRPC2D.last_col;
	last_row = aRPC2D.last_row;
	long_scale = aRPC2D.long_scale;
	lat_scale = aRPC2D.lat_scale;
	samp_scale = aRPC2D.samp_scale;
	line_scale = aRPC2D.line_scale;
	long_off = aRPC2D.long_off;
	lat_off = aRPC2D.lat_off;
	samp_off = aRPC2D.samp_off;
	line_off = aRPC2D.line_off;
	height_scale = aRPC2D.height_scale;
	height_off = aRPC2D.height_off;
}

void RPC::ComputeNormFactors(double aHMin, double aHMax)
{
	vector<double> aPtsGeoX, aPtsGeoY, aPtsImX, aPtsImY;
	cout << "ASTERPtsIm.size() : " << ASTERPtsIm.size() << endl;
	for (u_int i = 0; i < ASTERPtsIm.size(); i++)
	{
		aPtsImX.push_back(ASTERPtsIm[i].x);
		aPtsImY.push_back(ASTERPtsIm[i].y);

		//convert to geodetic
		//WGS84 ellipsoid
		double a = 6378137;
		double b = (1 - 1 / 298.257223563)*a;
		double e2 = 1 - (b * b) / (a * a);
		double WGSCorFact = 0.99330562;

        /**/// OLD good enough since z=0
		Pt3dr aPtGeo;
		Pt3dr aPtECEF = ASTERPtsECEF[i];
		double r = sqrt(aPtECEF.x*aPtECEF.x + aPtECEF.y*aPtECEF.y + aPtECEF.z*aPtECEF.z);
		aPtGeo.y = asin(aPtECEF.z / r) * 180 / M_PI; //degrees
		aPtGeo.x = acos(aPtECEF.x / (r*cos(aPtGeo.y * M_PI / 180))) * 180 / M_PI;//degrees
		if (aPtECEF.y < 0)//"Western emisphere"
			aPtGeo.x = -aPtGeo.x;
        //cout << "OLD :" << aPtGeo.x << endl;
		//to geodetic
		aPtGeo.y = atan(tan(aPtGeo.y *M_PI / 180) / WGSCorFact) * 180 / M_PI;
        /**/

		/*/NEW GEODESY
		Pt3dr aPtGeo;
		Pt3dr aPtECEF = ASTERPtsECEF[i];
		// Computing longitude (true transformation)
		aPtGeo.x = atan(aPtECEF.y / aPtECEF.x) * 180 / M_PI; //degrees
		if (aPtECEF.y < 0 && aPtECEF.x < 0)//"Long=[-90->-180]"
			aPtGeo.x = aPtGeo.x - 180;
		if (aPtECEF.y > 0 && aPtECEF.x < 0)//"Long=[90->180]"
			aPtGeo.x = aPtGeo.x + 180;

		//Computing latitude (estimation)
		double r = sqrt(aPtECEF.x*aPtECEF.x + aPtECEF.y*aPtECEF.y + aPtECEF.z*aPtECEF.z);
		double p = sqrt(aPtECEF.x*aPtECEF.x + aPtECEF.y*aPtECEF.y);
		double latNow = atan(p / aPtECEF.z);//rad geocentric to initialize estimation
		//loop
		double Rn;
		double h;
		for (u_int i = 0; i < 10; i++)// converge after 10 loops (even after 4 but for safety)
		{
			Rn = a / sqrt(1 - e2*sin(latNow)*sin(latNow));
			h = p / cos(latNow) - Rn;
			latNow = atan(aPtECEF.z / p * 1 / (1 - e2*Rn / (Rn + h)));
		}
		aPtGeo.y = latNow;

		//Computing Ellipsoid height
		Rn = a / sqrt(1 - e2*sin(aPtGeo.y)*sin(aPtGeo.y));
		aPtGeo.z = p / cos(aPtGeo.y) - Rn;
		//Latitude rad to degrees
		aPtGeo.y = aPtGeo.y * 180 / M_PI;
		//END NEW GEODESY
		*/

		//Filling vectors
		aPtsGeoX.push_back(aPtGeo.x);
		aPtsGeoY.push_back(aPtGeo.y);
	}

	/*
	for (u_int i = 0; i < aMatPtsIm.size(); i++)
	{
		for (u_int j = 0; j < aMatPtsIm[0].size(); j++)
		{
			aPtsImX.push_back(aMatPtsIm[i][j].x);
			aPtsImY.push_back(aMatPtsIm[i][j].y);

			//convert to geodetic
			//WGS84 ellipsoid
			//double a = 6378137;
			//double b = (1 - 1 / 298.257223563)*a;
			double WGSCorFact = 0.99330562;

			Pt3dr aPtGeo;
			Pt3dr aPtECEF=aMatPtsECEF[i][j];
			double r = sqrt(aPtECEF.x*aPtECEF.x + aPtECEF.y*aPtECEF.y + aPtECEF.z*aPtECEF.z);
			aPtGeo.y = asin(aPtECEF.z / r) * 180 / M_PI; //degrees
			aPtGeo.x = acos(aPtECEF.x / (r*cos(aPtGeo.y * M_PI / 180))) * 180 / M_PI;//degrees
			if (aPtECEF.y < 0)//"Western emisphere"
				aPtGeo.x = -aPtGeo.x;
			//to geodetic
			aPtGeo.y = atan(tan(aPtGeo.y *M_PI / 180) / WGSCorFact) * 180 / M_PI;

			//Filling vectors
			aPtsGeoX.push_back(aPtGeo.x);
			aPtsGeoY.push_back(aPtGeo.y);
		}
	}
	*/
	//Find Mins and Maxs
	Pt2dr aPtGeoMin(*std::min_element(aPtsGeoX.begin(), aPtsGeoX.end()), *std::min_element(aPtsGeoY.begin(), aPtsGeoY.end()));
	Pt2dr aPtGeoMax(*std::max_element(aPtsGeoX.begin(), aPtsGeoX.end()), *std::max_element(aPtsGeoY.begin(), aPtsGeoY.end()));
	Pt2dr aPtImMin(*std::min_element(aPtsImX.begin(), aPtsImX.end()), *std::min_element(aPtsImY.begin(), aPtsImY.end()));
	Pt2dr aPtImMax(*std::max_element(aPtsImX.begin(), aPtsImX.end()), *std::max_element(aPtsImY.begin(), aPtsImY.end()));
	first_lon = aPtGeoMin.x;
	first_lat = aPtGeoMin.y;
	first_height = aHMin;
	first_col = aPtImMin.x;
	first_row = aPtImMin.y;
	last_lon = aPtGeoMax.x;
	last_lat = aPtGeoMax.y;
	last_height = aHMax;
	last_col = aPtImMax.x;
	last_row = aPtImMax.y;
	//Compute scales and offsets
	long_scale = (aPtGeoMax.x - aPtGeoMin.x) / 2;
	lat_scale = (aPtGeoMax.y - aPtGeoMin.y) / 2;
	samp_scale = (aPtImMax.x - aPtImMin.x) / 2;
	line_scale = (aPtImMax.y - aPtImMin.y) / 2;
	long_off = (aPtGeoMax.x + aPtGeoMin.x) / 2;;
	lat_off = (aPtGeoMax.y + aPtGeoMin.y) / 2;
	samp_off = (aPtImMax.x + aPtImMin.x) / 2;
	line_off = (aPtImMax.y + aPtImMin.y) / 2;
	height_scale = (aHMax - aHMin) / 2;
	height_off = (aHMax + aHMin) / 2;
}

/*
vector<Pt3dr> RPC::GenerateRandNormGrid(u_int gridSize)
{
	//Generating a gridSize*gridSize grid on the normalized space with random normalized heights
	vector<Pt3dr> aGridNorm;
	srand(time(NULL));//Initiate the rand value
	for (u_int i = 0; i <= gridSize; i++)
	{
		for (u_int j = 0; j <= gridSize; j++)
		{
			Pt3dr aPt;
			aPt.x = (double(i) - (gridSize / 2)) / (gridSize / 2);
			aPt.y = (double(j) - (gridSize / 2)) / (gridSize / 2);
			aPt.z = double(rand() % 2000 - 1000) / 1000;
			aGridNorm.push_back(aPt);
		}
	}

	return aGridNorm;
}
*/

//this GenerateRandNormGrid allows to create rectangular grids
vector<Pt3dr> RPC::GenerateRandNormGrid(const Pt2di &aGridSz)
{
    //Generating a grid on the normalized space with random normalized heights
    vector<Pt3dr> aGridNorm;

    srand(time(NULL));//Initiate the rand value
    int aR, aC;
    for (aR = 0; aR <= aGridSz.x; aR++)
    {
        for (aC = 0; aC <= aGridSz.y; aC++)
	{
	    Pt3dr aPt;
	    aPt.x = (double(aR) - (aGridSz.x / 2)) / (aGridSz.x / 2);
	    aPt.y = (double(aC) - (aGridSz.y / 2)) / (aGridSz.y / 2);
	    aPt.z = double(rand() % 2000 - 1000) / 1000;
	    aGridNorm.push_back(aPt);
	}
    }

    return(aGridNorm);
}

//this GenerateNormGrid generates a regular grid
vector<Pt3dr> RPC::GenerateNormGrid(const Pt3di &aGridSz)
{
    vector<Pt3dr> aGridNorm;

    double aZS = double(1)/aGridSz.z;

    int aR, aC, aH;
    for (aR = 0; aR <= aGridSz.x; aR++)
    {
        for (aC = 0; aC <= aGridSz.y; aC++)
	{
            for(aH = 0; aH < aGridSz.z; aH++ )
	    {
	        Pt3dr aPt;
	        aPt.x = (double(aR) - (aGridSz.x / 2)) / (aGridSz.x / 2);
	        aPt.y = (double(aC) - (aGridSz.y / 2)) / (aGridSz.y / 2);
	        aPt.z = aZS*aH;
	        
		aGridNorm.push_back(aPt);
	    }
	}
    }

    return(aGridNorm);
}

//Use lattice points and satellite positions to generate points to be inputed in GCP2Direct and GCP2Inverse
vector<vector<Pt3dr> > RPC::GenerateNormLineOfSightGrid(int nbLayers, double aHMin, double aHMax)
{
	//WGS84 ellipsoid
	double a = 6378137;
	double b = (1 - 1 / 298.257223563)*a;
	double e2 = 1 - (b * b) / (a * a);
	double WGSCorFact = 0.99330562;

	vector<vector<Pt3dr> > aMatPtsNorm;
	vector<Pt3dr> aVectPtsGeo, aVectPtsIm;
	//int nbLatticePts = aMatPtsIm.size()*aMatPtsIm[0].size();

	for (u_int i = 0; i < ASTERPtsIm.size(); i++){

			//Image point 3D coordinates object created (identical for all grid levels)
		Pt3dr aPtIm; aPtIm.x = ASTERPtsIm[i].x; aPtIm.y = ASTERPtsIm[i].y;

			//Line of Sight LOS computed
		Pt3dr aLOS = ASTERSatPos[i] - ASTERPtsECEF[i];
			//Norming aLOS
			aLOS = aLOS / sqrt(aLOS.x*aLOS.x + aLOS.y*aLOS.y + aLOS.z*aLOS.z);

			//for each layer of grid
			for (u_int height = aHMin; height <= aHMax; height = height + (aHMax-aHMin)/(nbLayers-1))
			{
				//ECEF coord points are computed
				Pt3dr aPtECEF = ASTERPtsECEF[i] + aLOS*height;

				//Coordinates are transformed from ECEF to geodetic

				//NEW
				Pt3dr aPtGeo;
				// Computing longitude (true transformation)
				aPtGeo.x = atan(aPtECEF.y / aPtECEF.x) * 180 / M_PI; //degrees
				if (aPtECEF.y < 0 && aPtECEF.x < 0)//"Long=[-90->-180]"
					aPtGeo.x = aPtGeo.x - 180;
                if (aPtECEF.y > 0 && aPtECEF.x < 0)//"Long=[90->180]"
					aPtGeo.x = aPtGeo.x + 180;

				//Computing latitude (estimation) according to ALG0012 in http://geodesie.ign.fr/contenu/fichiers/documentation/algorithmes/notice/NTG_80.pdf
				double r = sqrt(aPtECEF.x*aPtECEF.x + aPtECEF.y*aPtECEF.y + aPtECEF.z*aPtECEF.z);
				double p = sqrt(aPtECEF.x*aPtECEF.x + aPtECEF.y*aPtECEF.y);
				double latNow = atan(p / aPtECEF.z);//rad geocentric to initialize estimation
				//loop
				double Rn;
				double h;
				for (u_int i = 0; i < 10; i++)// converge after 10 loops (even after 4 but for safety)
				{
					Rn = a / sqrt(1 - e2*sin(latNow)*sin(latNow));
					h = p / cos(latNow) - Rn;
					latNow = atan(aPtECEF.z / p * 1 / (1 - e2*Rn / (Rn + h)));
				}
				aPtGeo.y = latNow;

				//Computing Ellipsoid height
				Rn = a / sqrt(1 - e2*sin(aPtGeo.y)*sin(aPtGeo.y));
				aPtGeo.z = p / cos(aPtGeo.y) - Rn;
				//Latitude rad to degrees
				aPtGeo.y = aPtGeo.y * 180 / M_PI;

				aVectPtsGeo.push_back(aPtGeo);

				/* OLD
				Pt3dr aPtGeo;
				double r = sqrt(aPtECEF.x*aPtECEF.x + aPtECEF.y*aPtECEF.y + aPtECEF.z*aPtECEF.z);
				aPtGeo.y = asin(aPtECEF.z / r) * 180 / M_PI; //degrees
				aPtGeo.x = acos(aPtECEF.x / (r*cos(aPtGeo.y * M_PI / 180))) * 180 / M_PI;//degrees
				if (aPtECEF.y < 0)//"Western emisphere"
					aPtGeo.x = -aPtGeo.x;
				aPtGeo.z = r - sqrt(a*a*b*b / (a*a*sin(aPtGeo.y * M_PI / 180)*sin(aPtGeo.y * M_PI / 180) + b*b*cos(aPtGeo.y * M_PI / 180)*cos(aPtGeo.y * M_PI / 180)));//(distance from point to earth center)-(distance from ellipsoide to earth center)
				//to geodetic
				aPtGeo.y = atan(tan(aPtGeo.y *M_PI / 180) / WGSCorFact) * 180 / M_PI;

				aVectPtsGeo.push_back(aPtGeo);
				*/

				//Image point 3D coordinates are recorded
				aPtIm.z = aPtGeo.z;
				aVectPtsIm.push_back(aPtIm);
			}
	}

	//Normalization
	for (u_int i = 0; i < aVectPtsIm.size(); i++)
	{

		aVectPtsGeo[i].x = (aVectPtsGeo[i].x - long_off) / long_scale;
		aVectPtsGeo[i].y = (aVectPtsGeo[i].y - lat_off) / lat_scale;
		aVectPtsGeo[i].z = (aVectPtsGeo[i].z - height_off) / height_scale;

		aVectPtsIm[i].x = (aVectPtsIm[i].x - samp_off) / samp_scale;
		aVectPtsIm[i].y = (aVectPtsIm[i].y - line_off) / line_scale;
		aVectPtsIm[i].z = (aVectPtsIm[i].z - height_off) / height_scale;
	}



	aMatPtsNorm.push_back(aVectPtsGeo);
	aMatPtsNorm.push_back(aVectPtsIm);

	return aMatPtsNorm;
}


//Take GCPs in normalized space to compute f in ground=f(image)
void RPC::GCP2Direct(vector<Pt3dr> aGridGeoNorm, vector<Pt3dr> aGridImNorm)
{

	//Cleaning potential data in RPC object
	direct_samp_num_coef.clear();
	direct_samp_den_coef.clear();
	direct_line_num_coef.clear();
	direct_line_den_coef.clear();

    //Parameters too get parameters of P1 and P2 in ---  lon=P1(row,column,Z)/P2(row,column,Z)  --- where (row,column,Z) are image coordinates (idem for lat)
    //To simplify notations : Column->X and Row->Y
    //Function is 0=Poly1(Y,X,Z)-long*Poly2(Y,X,Z) with poly 3rd degree (up to X^3,Y^3,Z^3,XXY,XXZ,XYY,XZZ,YYZ,YZZ)
    //First param (cst) of Poly2=1 to avoid sol=0

    L2SysSurResol aSysLon(39), aSysLat(39);

    //For all lattice points
    for (u_int i = 0; i<aGridGeoNorm.size(); i++){

        //Simplifying notations
        double X = aGridImNorm[i].x;
        double Y = aGridImNorm[i].y;
        double Z = aGridImNorm[i].z;
        double lon = aGridGeoNorm[i].x;
        double lat = aGridGeoNorm[i].y;

		double aEqLon[39] = {
			1, X, Y, Z, X*Y, X*Z, Y*Z, X*X, Y*Y, Z*Z, Y*X*Z, X*X*X, X*Y*Y, X*Z*Z, Y*X*X, Y*Y*Y, Y*Z*Z, X*X*Z, Y*Y*Z, Z*Z*Z,
			-lon*X, -lon*Y, -lon*Z, -lon*X*Y, -lon*X*Z, -lon*Y*Z, -lon*X*X, -lon*Y*Y, -lon*Z*Z, -lon*Y*X*Z, -lon*X*X*X, -lon*X*Y*Y, -lon*X*Z*Z, -lon*Y*X*X, -lon*Y*Y*Y, -lon*Y*Z*Z, -lon*X*X*Z, -lon*Y*Y*Z, -lon*Z*Z*Z
		};
		aSysLon.AddEquation(1, aEqLon, lon);


		double aEqLat[39] = {
			1, X, Y, Z, X*Y, X*Z, Y*Z, X*X, Y*Y, Z*Z, Y*X*Z, X*X*X, X*Y*Y, X*Z*Z, Y*X*X, Y*Y*Y, Y*Z*Z, X*X*Z, Y*Y*Z, Z*Z*Z,
			-lat*X, -lat*Y, -lat*Z, -lat*X*Y, -lat*X*Z, -lat*Y*Z, -lat*X*X, -lat*Y*Y, -lat*Z*Z, -lat*Y*X*Z, -lat*X*X*X, -lat*X*Y*Y, -lat*X*Z*Z, -lat*Y*X*X, -lat*Y*Y*Y, -lat*Y*Z*Z, -lat*X*X*Z, -lat*Y*Y*Z, -lat*Z*Z*Z
		};
		aSysLat.AddEquation(1, aEqLat, lat);
    }

    //Computing the result
    bool Ok;
    Im1D_REAL8 aSolLon = aSysLon.GSSR_Solve(&Ok);
    Im1D_REAL8 aSolLat = aSysLat.GSSR_Solve(&Ok);
    double* aDataLat = aSolLat.data();
    double* aDataLon = aSolLon.data();

    //Copying Data in RPC object
    //Numerators
    for (int i = 0; i<20; i++)
    {
        direct_samp_num_coef.push_back(aDataLon[i]);
        direct_line_num_coef.push_back(aDataLat[i]);
    }
    //Denominators (first one = 1)
    direct_line_den_coef.push_back(1);
    direct_samp_den_coef.push_back(1);
    for (int i = 20; i<39; i++)
    {
        direct_samp_den_coef.push_back(aDataLon[i]);
        direct_line_den_coef.push_back(aDataLat[i]);
    }
}

//Take GCPs in normalized space to compute f in image=f(ground)
void RPC::GCP2Inverse(vector<Pt3dr> aGridGeoNorm, vector<Pt3dr> aGridImNorm)
{

	//Cleaning potential data in RPC object
	inverse_samp_num_coef.clear();
	inverse_samp_den_coef.clear();
	inverse_line_num_coef.clear();
	inverse_line_den_coef.clear();

	//Parameters too get parameters of P1 and P2 in ---  Column=P1(long,lat,Z)/P2(long,lat,Z)  --- where (long,lat,Z) are geodetic coordinates (idem for row and P3/P4)
	//To simplify notations : long->X and lat->Y
	//Function is 0=Poly1(X,Y,Z)-column*Poly2(X,Y,Z) with poly 3rd degree (up to X^3,Y^3,Z^3,XXY,XXZ,XYY,XZZ,YYZ,YZZ)
	//First param (cst) of Poly2=1 to avoid sol=0

	L2SysSurResol aSysCol(39), aSysRow(39);

	//For all lattice points
	for (u_int i = 0; i<aGridGeoNorm.size(); i++){

		//Simplifying notations
		double X = aGridGeoNorm[i].x;
		double Y = aGridGeoNorm[i].y;
		double Z = aGridGeoNorm[i].z;
		double Col = aGridImNorm[i].x;
		double Row = aGridImNorm[i].y;

		double aEqCol[39] = {
			1, X, Y, Z, X*Y, X*Z, Y*Z, X*X, Y*Y, Z*Z, Y*X*Z, X*X*X, X*Y*Y, X*Z*Z, Y*X*X, Y*Y*Y, Y*Z*Z, X*X*Z, Y*Y*Z, Z*Z*Z,
			-Col*X, -Col*Y, -Col*Z, -Col*X*Y, -Col*X*Z, -Col*Y*Z, -Col*X*X, -Col*Y*Y, -Col*Z*Z, -Col*Y*X*Z, -Col*X*X*X, -Col*X*Y*Y, -Col*X*Z*Z, -Col*Y*X*X, -Col*Y*Y*Y, -Col*Y*Z*Z, -Col*X*X*Z, -Col*Y*Y*Z, -Col*Z*Z*Z
		};
		aSysCol.AddEquation(1, aEqCol, Col);


		double aEqRow[39] = {
			1, X, Y, Z, X*Y, X*Z, Y*Z, X*X, Y*Y, Z*Z, Y*X*Z, X*X*X, X*Y*Y, X*Z*Z, Y*X*X, Y*Y*Y, Y*Z*Z, X*X*Z, Y*Y*Z, Z*Z*Z,
			-Row*X, -Row*Y, -Row*Z, -Row*X*Y, -Row*X*Z, -Row*Y*Z, -Row*X*X, -Row*Y*Y, -Row*Z*Z, -Row*Y*X*Z, -Row*X*X*X, -Row*X*Y*Y, -Row*X*Z*Z, -Row*Y*X*X, -Row*Y*Y*Y, -Row*Y*Z*Z, -Row*X*X*Z, -Row*Y*Y*Z, -Row*Z*Z*Z
		};
		aSysRow.AddEquation(1, aEqRow, Row);
	}

	//Computing the result
	bool Ok;
	Im1D_REAL8 aSolCol = aSysCol.GSSR_Solve(&Ok);
	Im1D_REAL8 aSolRow = aSysRow.GSSR_Solve(&Ok);
	double* aDataCol = aSolCol.data();
	double* aDataRow = aSolRow.data();

	//Copying Data in RPC object
	//Numerators
	for (int i = 0; i<20; i++)
	{
		inverse_samp_num_coef.push_back(aDataCol[i]);
		inverse_line_num_coef.push_back(aDataRow[i]);
	}
	//Denominators (first one = 1)
	inverse_line_den_coef.push_back(1);
	inverse_samp_den_coef.push_back(1);
	for (int i = 20; i<39; i++)
	{
		inverse_samp_den_coef.push_back(aDataCol[i]);
		inverse_line_den_coef.push_back(aDataRow[i]);
	}
}

void RPC::ChSysRPC(const cSystemeCoord & aChSys)
{

    //generate the grid in (original) geodetic CS
    int aK1, aK2, aK3, aSampl = 50;
    Pt3dr aP;
    Pt3dr aStep(double(abs(first_lon-last_lon))/aSampl,
		        double(abs(first_lat-last_lat))/aSampl,
		        double(abs(first_height-last_height))/aSampl);
    
    std::vector<Pt3dr> aGridOrg, aGridCarto, aGridImg;

    std::string aTmpIn = "Proj4InputRPC.txt";
    FILE * aFPin = FopenNN(aTmpIn,"w","RPC::ChSysRPC");

    for(aK1=0; aK1<aSampl; aK1++)
       for(aK2=0; aK2<aSampl; aK2++)
	      for(aK3=0; aK3<aSampl; aK3++)
	      {
		     aP = Pt3dr(first_lon+aStep.x*aK1,
			            first_lat+aStep.y*aK2,
			            first_height+aStep.z*aK3);
	        
		     aGridOrg.push_back(aP);

             fprintf(aFPin,"%.20f %.20f %.20f\n",aP.x,aP.y,aP.z);
	    }
    ElFclose(aFPin);

    //convert to carto
    std::string aTmpOut = "Proj4OutputRPC.txt";
    
    std::string aCom =  g_externalToolHandler.get("cs2cs").callName() + " " +
	                "+proj=longlat +datum=WGS84" + " +to " + 
			        aChSys.BSC()[0].AuxStr()[0] + " " + aTmpIn + 
			        " > " + aTmpOut;

    VoidSystem(aCom.c_str());

    ELISE_fp aFOut(aTmpOut.c_str(),ELISE_fp::READ);

    char * aLine;
    while ((aLine = aFOut.std_fgets()))
    {
        int aNb = sscanf(aLine,"%lf %lf %lf",&aP.x,&aP.y,&aP.z);
	    ELISE_ASSERT(aNb==3,"Bad Nb value RPC::ChSysRPC, internal error");

	    aGridCarto.push_back(aP);
    }
    aFOut.close();

    ELISE_fp::RmFile(aTmpOut);
    ELISE_fp::RmFile(aTmpIn);

    //normalise the geodetic cs
    for(aK1=0; aK1<int(aGridOrg.size()); aK1++)
    {
        aGridOrg.at(aK1).x = (aGridOrg.at(aK1).x - long_off)/long_scale;
        aGridOrg.at(aK1).y = (aGridOrg.at(aK1).y - lat_off)/lat_scale;
        aGridOrg.at(aK1).z = (aGridOrg.at(aK1).z - height_off)/height_scale;
    }
   
    //get carto cs normalising parameters & validating zone
    double X_min = aGridCarto.at(0).x, X_max = X_min, 
	Y_min = aGridCarto.at(0).y, Y_max = Y_min, 
	Z_min = aGridCarto.at(0).z, Z_max = Z_min;

    for(aK1=1; aK1<int(aGridCarto.size()); aK1++)
    {
        if(aGridCarto.at(aK1).x > X_max)
	       X_max = aGridCarto.at(aK1).x;
	
 	    if(aGridCarto.at(aK1).x < X_min)
	       X_min = aGridCarto.at(aK1).x;

	    if(aGridCarto.at(aK1).y < Y_min)
	       Y_min = aGridCarto.at(aK1).y;
 
        if(aGridCarto.at(aK1).y > Y_max)
	       Y_max = aGridCarto.at(aK1).y;

	    if(aGridCarto.at(aK1).z < Z_min)
	       Z_min = aGridCarto.at(aK1).z;

        if(aGridCarto.at(aK1).z > Z_max)
	       Z_max = aGridCarto.at(aK1).z;
    }
    
    first_lon = X_min;
    last_lon = X_max;
    first_lat = Y_min;
    last_lat = Y_max;
    first_height = Z_min;
    last_height = Z_max;

    long_scale = (X_max - X_min)/2;
    lat_scale = (Y_max - Y_min)/2;
    height_scale = (Z_max - Z_min)/2;
    
    long_off = X_min + long_scale;
    lat_off = Y_min + lat_scale;
    height_off = Z_min + height_scale;

    for(aK1=0; aK1<int(aGridCarto.size()); aK1++)
    {
        aGridCarto.at(aK1).x = (aGridCarto.at(aK1).x - lat_off)/lat_scale;
        aGridCarto.at(aK1).y = (aGridCarto.at(aK1).y - long_off)/long_scale;
        aGridCarto.at(aK1).z = (aGridCarto.at(aK1).z - height_off)/height_scale;
    }


    //back project norm geodetic grid to image space
    for(aK1=0; aK1<int(aGridOrg.size()); aK1++)
        aGridImg.push_back(InverseRPCNorm(aGridOrg.at(aK1)));    
    
    //learn inverse projection function for xy and XYZ_carto_norm
    GCP2Inverse(aGridOrg, aGridImg);

    //learn direct projection function for xy and XYZ_carto_norm
    GCP2Direct(aGridOrg, aGridImg);    

    
}

void RPC::ReadRPB(std::string const &filename)
{
    std::ifstream RPBfile(filename.c_str());
    ELISE_ASSERT(RPBfile.good(), " RPB file not found ");
    std::string line;
    std::string a, b;
    //Pass 6 lines
    for (u_int i = 0; i < 6; i++)
        std::getline(RPBfile, line);
    //Line Offset
    {
        std::getline(RPBfile, line);
        std::istringstream iss(line);
        iss >> a >> b >> line_off;
    }
    //Samp Offset
    {
        std::getline(RPBfile, line);
        std::istringstream iss(line);
        iss >> a >> b >> samp_off;
    }
    //Lat Offset
    {
        std::getline(RPBfile, line);
        std::istringstream iss(line);
        iss >> a >> b >> lat_off;
    }
    //Lon Offset
    {
        std::getline(RPBfile, line);
        std::istringstream iss(line);
        iss >> a >> b >> long_off;
    }
    //Height Offset
    {
        std::getline(RPBfile, line);
        std::istringstream iss(line);
        iss >> a >> b >> height_off;
    }
    //Line Scale
    {
        std::getline(RPBfile, line);
        std::istringstream iss(line);
        iss >> a >> b >> line_scale;
    }
    //Samp Scale
    {
        std::getline(RPBfile, line);
        std::istringstream iss(line);
        iss >> a >> b >> samp_scale;
    }
    //Lat Scale
    {
        std::getline(RPBfile, line);
        std::istringstream iss(line);
        iss >> a >> b >> lat_scale;
    }
    //Lon Scale
    {
        std::getline(RPBfile, line);
        std::istringstream iss(line);
        iss >> a >> b >> long_scale;
    }
    //Height Scale
    {
        std::getline(RPBfile, line);
        std::istringstream iss(line);
        iss >> a >> b >> height_scale;
    }
    double aCoef;
    //inverse_line_num_coef
    {
        std::getline(RPBfile, line);
        for (u_int i = 0; i < 20; i++)
        {
            std::getline(RPBfile, line);
            std::istringstream iss(line);
            iss >> aCoef;
            inverse_line_num_coef.push_back(aCoef);
        }
    }
    //inverse_line_den_coef
    {
        std::getline(RPBfile, line);
        for (u_int i = 0; i < 20; i++)
        {
            std::getline(RPBfile, line);
            std::istringstream iss(line);
            iss >> aCoef;
            inverse_line_den_coef.push_back(aCoef);
        }
    }
    //inverse_samp_num_coef
    {
        std::getline(RPBfile, line);
        for (u_int i = 0; i < 20; i++)
        {
            std::getline(RPBfile, line);
            std::istringstream iss(line);
            iss >> aCoef;
            inverse_samp_num_coef.push_back(aCoef);
        }
    }
    //inverse_samp_den_coef
    {
        std::getline(RPBfile, line);
        for (u_int i = 0; i < 20; i++)
        {
            std::getline(RPBfile, line);
            std::istringstream iss(line);
            iss >> aCoef;
            inverse_samp_den_coef.push_back(aCoef);
        }
    }

    IS_INV_INI=true;
}

void RPC::ReadXML(std::string const &filename)
{

    cElXMLTree tree(filename.c_str());

    cElXMLTree* nodes = tree.GetUnique(std::string("NUMROWS"));
    first_row = 0;
    last_row = std::atof(nodes->GetUniqueVal().c_str()) - 1;


    nodes = tree.GetUnique(std::string("NUMCOLUMNS"));
    first_col = 0;
    last_col = std::atof(nodes->GetUniqueVal().c_str()) - 1;


    nodes = tree.GetUnique(std::string("NUMTILES"));
    mNumTile = std::atoi(nodes->GetUniqueVal().c_str());
   
    
    nodes = tree.GetUnique(std::string("ERRBIAS"));
    indirErrBiasRow = std::atof(nodes->GetUniqueVal().c_str());
    indirErrBiasCol = dirErrBiasX;


    nodes = tree.GetUnique(std::string("LINEOFFSET"));
    line_off = std::atof(nodes->GetUniqueVal().c_str());


    nodes = tree.GetUnique(std::string("SAMPOFFSET"));
    samp_off = std::atof(nodes->GetUniqueVal().c_str());
    

    nodes = tree.GetUnique(std::string("LATOFFSET"));
    lat_off = std::atof(nodes->GetUniqueVal().c_str());


    nodes = tree.GetUnique(std::string("LONGOFFSET"));
    long_off = std::atof(nodes->GetUniqueVal().c_str());
    

    nodes = tree.GetUnique(std::string("HEIGHTOFFSET"));
    height_off = std::atof(nodes->GetUniqueVal().c_str());
   

    nodes = tree.GetUnique(std::string("LINESCALE"));
    line_scale = std::atof(nodes->GetUniqueVal().c_str());


    nodes = tree.GetUnique(std::string("SAMPSCALE"));
    samp_scale = std::atof(nodes->GetUniqueVal().c_str());
    
    
    nodes = tree.GetUnique(std::string("LATSCALE"));
    lat_scale = std::atof(nodes->GetUniqueVal().c_str());
    
    
    nodes = tree.GetUnique(std::string("LONGSCALE"));
    long_scale = std::atof(nodes->GetUniqueVal().c_str());


    nodes = tree.GetUnique(std::string("HEIGHTSCALE"));
    height_scale = std::atof(nodes->GetUniqueVal().c_str());
    

    //RPC coefficients
    inverse_line_num_coef.resize(20);
    inverse_line_den_coef.resize(20);
    inverse_samp_num_coef.resize(20);
    inverse_samp_den_coef.resize(20);

    nodes = tree.GetUnique(std::string("LINENUMCOEF"));
    {std::istringstream iss;
     iss.str(nodes->GetUniqueVal());

     iss >> inverse_line_num_coef.at(0) >> inverse_line_num_coef.at(1) 
	 >> inverse_line_num_coef.at(2) >> inverse_line_num_coef.at(3)
	 >> inverse_line_num_coef.at(4) >> inverse_line_num_coef.at(5)
	 >> inverse_line_num_coef.at(6) >> inverse_line_num_coef.at(7)
	 >> inverse_line_num_coef.at(8) >> inverse_line_num_coef.at(9)
	 >> inverse_line_num_coef.at(10) >> inverse_line_num_coef.at(11)
	 >> inverse_line_num_coef.at(12) >> inverse_line_num_coef.at(13)
	 >> inverse_line_num_coef.at(14) >> inverse_line_num_coef.at(15)
	 >> inverse_line_num_coef.at(16) >> inverse_line_num_coef.at(17)
	 >> inverse_line_num_coef.at(18) >> inverse_line_num_coef.at(19);

    }

    nodes = tree.GetUnique(std::string("LINEDENCOEF"));
    {std::istringstream iss;
     iss.str(nodes->GetUniqueVal());

     iss >> inverse_line_den_coef.at(0) >> inverse_line_den_coef.at(1)
	 >> inverse_line_den_coef.at(2) >> inverse_line_den_coef.at(3)
         >> inverse_line_den_coef.at(4) >> inverse_line_den_coef.at(5)
	 >> inverse_line_den_coef.at(6) >> inverse_line_den_coef.at(7)
	 >> inverse_line_den_coef.at(8) >> inverse_line_den_coef.at(9)
	 >> inverse_line_den_coef.at(10) >> inverse_line_den_coef.at(11)
	 >> inverse_line_den_coef.at(12) >> inverse_line_den_coef.at(13)
	 >> inverse_line_den_coef.at(14) >> inverse_line_den_coef.at(15)
	 >> inverse_line_den_coef.at(16) >> inverse_line_den_coef.at(17)
	 >> inverse_line_den_coef.at(18) >> inverse_line_den_coef.at(19);	 
    }

    nodes = tree.GetUnique(std::string("SAMPNUMCOEF"));
    {std::istringstream iss;
     iss.str(nodes->GetUniqueVal());

     iss >> inverse_samp_num_coef.at(0) >> inverse_samp_num_coef.at(1)
	 >> inverse_samp_num_coef.at(2) >> inverse_samp_num_coef.at(3)
	 >> inverse_samp_num_coef.at(4) >> inverse_samp_num_coef.at(5)
	 >> inverse_samp_num_coef.at(6) >> inverse_samp_num_coef.at(7)
	 >> inverse_samp_num_coef.at(8) >> inverse_samp_num_coef.at(9)
	 >> inverse_samp_num_coef.at(10) >> inverse_samp_num_coef.at(11)
	 >> inverse_samp_num_coef.at(12) >> inverse_samp_num_coef.at(13)
	 >> inverse_samp_num_coef.at(14) >> inverse_samp_num_coef.at(15)
	 >> inverse_samp_num_coef.at(16) >> inverse_samp_num_coef.at(17)
	 >> inverse_samp_num_coef.at(18) >> inverse_samp_num_coef.at(19);

    }

    nodes = tree.GetUnique(std::string("SAMPDENCOEF"));
    {std::istringstream iss;
     iss.str(nodes->GetUniqueVal());

     iss >> inverse_samp_den_coef.at(0) >> inverse_samp_den_coef.at(1)
	 >> inverse_samp_den_coef.at(2) >> inverse_samp_den_coef.at(3)
	 >> inverse_samp_den_coef.at(4) >> inverse_samp_den_coef.at(5)
	 >> inverse_samp_den_coef.at(6) >> inverse_samp_den_coef.at(7)
	 >> inverse_samp_den_coef.at(8) >> inverse_samp_den_coef.at(9)
	 >> inverse_samp_den_coef.at(10) >> inverse_samp_den_coef.at(11)
	 >> inverse_samp_den_coef.at(12) >> inverse_samp_den_coef.at(13)
	 >> inverse_samp_den_coef.at(14) >> inverse_samp_den_coef.at(15)
	 >> inverse_samp_den_coef.at(16) >> inverse_samp_den_coef.at(17)
	 >> inverse_samp_den_coef.at(18) >> inverse_samp_den_coef.at(19);
    
    }
     
    cElXMLTree* nodesFilOne; 
    
    nodes = tree.GetUnique(std::string("BAND_P"));
    nodesFilOne = nodes->GetUnique("ULLON");
    first_lon = std::atof((nodesFilOne->GetUniqueVal()).c_str());

    nodesFilOne = nodes->GetUnique("URLON");
    last_lon = std::atof((nodesFilOne->GetUniqueVal()).c_str());
    
    nodesFilOne = nodes->GetUnique("ULLAT");
    last_lat = std::atof((nodesFilOne->GetUniqueVal()).c_str());

    nodesFilOne = nodes->GetUnique("LLLAT");
    first_lat = std::atof((nodesFilOne->GetUniqueVal()).c_str());

/*    std::vector<double> aHMM;

    nodesFilOne = nodes->GetUnique("ULHAE");
    aHMM.push_back( std::atof((nodesFilOne->GetUniqueVal()).c_str()) );

    nodesFilOne = nodes->GetUnique("URHAE");
    aHMM.push_back( std::atof((nodesFilOne->GetUniqueVal()).c_str()) );

    nodesFilOne = nodes->GetUnique("LRHAE");
    aHMM.push_back( std::atof((nodesFilOne->GetUniqueVal()).c_str()) );

    nodesFilOne = nodes->GetUnique("LLHAE");
    aHMM.push_back( std::atof((nodesFilOne->GetUniqueVal()).c_str()) );

    first_height = (*std::min_element(aHMM.begin(), aHMM.end()));
    last_height  = (*std::max_element(aHMM.begin(), aHMM.end())); */
    first_height = -1*height_scale + height_off;
    last_height = 1*height_scale + height_off;

    IS_INV_INI = true; 
}

//Read AsterMetaDataXML
void RPC::AsterMetaDataXML(std::string filename)
{

	//Read Lattice points in image coordinates
	cElXMLTree tree(filename.c_str());

	int NbLattice = tree.GetUnique("NbLattice")->GetUniqueValInt();
	cout << "Number of lattice points (im) : " << NbLattice << endl;


	std::string LatticePoint = "LatticePoint_";
	for (int c = 1; c <= NbLattice; c++)
	{
		std::stringstream ss;
		ss << c;
		LatticePoint.append(ss.str());
		//cout << LatticePoint << endl;
		std::stringstream aStream(tree.GetUnique(LatticePoint.c_str())->GetUniqueVal());
		double x, y;
		aStream >> x >> y;
		Pt2dr aLattice(x, y);
		//cout << aLattice << endl;
		ASTERPtsIm.push_back(aLattice);
		LatticePoint = LatticePoint.substr(0, 13);
	}
	//cout << ASTERPtsIm << endl;


	//Read Lattice points in ECEF coordinates

	int NbECEF = tree.GetUnique("NbECEF")->GetUniqueValInt();
	cout << "Number of lattice points (ECEF) : " << NbECEF << endl;


	std::string ECEF = "ECEF_";
	for (int c = 1; c <= NbECEF; c++)
	{
		std::stringstream ss;
		ss << c;
		ECEF.append(ss.str());
		//cout << ECEF << endl;
		std::stringstream aStream(tree.GetUnique(ECEF.c_str())->GetUniqueVal());
		double x, y, z;
		aStream >> x >> y >> z;
		Pt3dr aECEF(x, y, z);
		//cout << aECEF << endl;
		ASTERPtsECEF.push_back(aECEF);
		ECEF = ECEF.substr(0, 5);
	}
	//cout << ASTERPtsECEF << endl;

	//Read Satelite positions

	int NbSatPos = tree.GetUnique("NbSatPos")->GetUniqueValInt();
	cout << "Number of Satellite positions : " << NbSatPos << endl;


	std::string SatPos = "SatPos_";
	for (int c = 1; c <= NbSatPos; c++)
	{
		std::stringstream ss;
		ss << c;
		SatPos.append(ss.str());
		//cout << SatPos << endl;
		std::stringstream aStream(tree.GetUnique(SatPos.c_str())->GetUniqueVal());
		double x, y, z;
		aStream >> x >> y >> z;
		Pt3dr aSatPos(x, y, z);
		//cout << aSatPos << endl;
		for (u_int j = 0; j < 11; j++)
			ASTERSatPos.push_back(aSatPos); //pushed once per column (11 times)
		SatPos = SatPos.substr(0, 7);
	}
}


void RPC::ReadASCII(std::string const &filename)
{
    std::ifstream ASCIIfi(filename.c_str());
    ELISE_ASSERT(ASCIIfi.good(), " ASCII file not found ");
    
    std::string line;
    std::string a, b;
    int aC;
    double aCoefTmp;


    //Line Offset
    {std::istringstream iss;
    std::getline(ASCIIfi, line);
    iss.str(line);
    iss >> a >> line_off >> b;}

    //Samp Offset
    {std::istringstream iss;
    std::getline(ASCIIfi, line);
    iss.str(line);
    iss >> a >> samp_off >> b;}    

    //Lat Offset
    {std::istringstream iss;
    std::getline(ASCIIfi, line);
    iss.str(line);
    iss >> a >> lat_off >> b;}

    //Lon Offset 
    {std::istringstream iss;
    std::getline(ASCIIfi, line);
    iss.str(line);
    iss >> a >> long_off >> b;}

    //Height Offset 
    {std::istringstream iss;
    std::getline(ASCIIfi, line);
    iss.str(line);
    iss >> a >> height_off >> b;}

    //Line Scale
    {std::istringstream iss;
    std::getline(ASCIIfi, line);
    iss.str(line);
    iss >> a >> line_scale >> b;}

    //Sample Scale
    {std::istringstream iss;
    std::getline(ASCIIfi, line);
    iss.str(line);
    iss >> a >> samp_scale >> b;}

    //Lat Scale
    {std::istringstream iss;
    std::getline(ASCIIfi, line);
    iss.str(line);
    iss >> a >> lat_scale >> b;}

    //Lon Scale
    {std::istringstream iss;
    std::getline(ASCIIfi, line);
    iss.str(line);
    iss >> a >> long_scale >> b;}

    //Height Scale
    {std::istringstream iss;
    std::getline(ASCIIfi, line);
    iss.str(line);
    iss >> a >> height_scale >> b;}

    first_height = -1*height_scale + height_off;
    last_height = 1*height_scale + height_off;

    //Inverse_line_num_coef
    for(aC=0; aC<20; aC++)
    {
		std::istringstream iss;
		std::getline(ASCIIfi, line);
		iss.str(line);
        iss >> a >> aCoefTmp;
    	inverse_line_num_coef.push_back(aCoefTmp);
    }

    //Inverse_line_den_coef 
    for(aC=0; aC<20; aC++)
    {
		std::istringstream iss;
        std::getline(ASCIIfi, line);
		iss.str(line);
		iss >> a >> aCoefTmp;
		inverse_line_den_coef.push_back(aCoefTmp);
    }

    //Inverse_samp_num_coef
    for(aC=0; aC<20; aC++)
    {
		std::istringstream iss;
        std::getline(ASCIIfi, line);
		iss.str(line);
		iss >> a >>  aCoefTmp;
		inverse_samp_num_coef.push_back(aCoefTmp);
    }

    //Inverse_samp_den_coef 
    for(aC=0; aC<20; aC++)
    {
		std::istringstream iss;
        std::getline(ASCIIfi, line);
		iss.str(line);
		iss >> a >> std::skipws >> aCoefTmp;
		inverse_samp_den_coef.push_back(aCoefTmp);
    }

    IS_INV_INI=true;
}

int RPC::ReadASCIIMetaData(std::string const &metafilename, std::string const &filename)
{

    std::ifstream MetaFi(metafilename.c_str());
    ELISE_ASSERT(MetaFi.good(), " ASCII metadata file not found in RPC::ReadASCIIMetaData");

    bool aMetaIsFound=false;

    std::string line=" ";
    std::string a, b, c, d;
    std::vector<double> avLat, avLon;

    std::string aToMatchOne = "Product";
    std::string aToMatchTwo = "Metadata";
    std::string aToMatchThree = "Component";
    std::string aToMatchFour = "File";
    std::string aToMatchFive = "Name:";
    std::string aToMatchSix = "Columns:";
    std::string aToMatchSev = "Coordinate:";


    while(MetaFi.good())
    {
		std::getline(MetaFi, line);
		std::istringstream iss;
		iss.str(line);
		iss >> a >> b >> c;
		if( a==aToMatchOne &&
	    	b==aToMatchThree &&
	    	c==aToMatchTwo )
		{
	    	std::getline(MetaFi, line);
	    	std::istringstream iss2;    
	    	iss2.str(line);
	    	iss2 >> a >> b >> c >> d;
	
	    	while(MetaFi.good())
	    	{
				//iterate to line "Component File Name:"
	        	if( !((a==aToMatchThree) &&
                     (b==aToMatchFour) &&
                     (c==aToMatchFive)))
	        	{
		    		std::getline(MetaFi, line);
		    		std::istringstream iss3;
		    		iss3.str(line);
		    		iss3 >> a >> b >> c >> d;
	        	}
				else
				{

		    		//check if the filenames correspond
		    		if(d.substr(0,d.length()-4)==filename.substr(0,filename.length()-4))
		    		{

						while(MetaFi.good())
						{

			    			//find
						// the Columns and Rows
						// the coords of the corners
			    			std::getline(MetaFi, line);
				    		std::istringstream iss4;
			    			iss4.str(line);
			    			iss4 >> a >> b >> c;


			    			//columns
			    			if(a==aToMatchSix)
			    			{
			        			this->first_col=0;
                    			this->last_col=std::atof(b.c_str())-1;	
			    
			        			//rows
			        			std::getline(MetaFi, line);
			        			std::istringstream iss5;
			        			iss5.str(line);
			        			iss5 >> a >> b >> c;

			        			this->first_row=0;
			        			this->last_row=std::atof(b.c_str())-1;

								aMetaIsFound=true;

								MetaFi.close();

								return EXIT_SUCCESS;
			    			}
						else if(a==aToMatchSev)
						{
						    //corner1
						    std::getline(MetaFi, line);
						    {std::istringstream issl0;
						    issl0.str(line);
						    issl0 >> a >> b >> c;}
                                                    std::cout << b << std::endl;
							
						    avLat.push_back(std::atof(b.c_str()));
						    
						    std::getline(MetaFi, line);
						    {std::istringstream issl0;
						    issl0.str(line);
						    issl0 >> a >> b >> c;}
                                                    std::cout << b << std::endl;
                                                   
						    avLon.push_back(std::atof(b.c_str()));
						    
						    //corner2 
						    std::getline(MetaFi, line); 
						    std::getline(MetaFi, line);
						    {std::istringstream issl0;
						    issl0.str(line);
						    issl0 >> a >> b >> c;}
                                                    std::cout << b << std::endl;

						    avLat.push_back(std::atof(b.c_str()));

						    std::getline(MetaFi, line);
						    {std::istringstream issl0;
						    issl0.str(line);
						    issl0 >> a >> b >> c;}
                                                    std::cout << b << std::endl;

						    avLon.push_back(std::atof(b.c_str()));

						    //corner3
						    std::getline(MetaFi, line);
						    std::getline(MetaFi, line);
					            {std::istringstream issl0;
					            issl0.str(line);
					            issl0 >> a >> b >> c;}
                                                    std::cout << b << std::endl;

					            avLat.push_back(std::atof(b.c_str()));

						    std::getline(MetaFi, line);
					            {std::istringstream issl0;
					            issl0.str(line);
					            issl0 >> a >> b >> c;}
                                                    std::cout << b << std::endl;

					            avLon.push_back(std::atof(b.c_str()));	    
                                              
						    //corner4
						    std::getline(MetaFi, line);
						    std::getline(MetaFi, line);
						    {std::istringstream issl0;
					            issl0.str(line);
                                                    issl0 >> a >> b >> c;}
						    std::cout << b << std::endl;

						    avLat.push_back(std::atof(b.c_str()));

						    std::getline(MetaFi, line);
						    {std::istringstream issl0;
					            issl0.str(line);
						    issl0 >> a >> b >> c;}
						    std::cout << b << std::endl;

						    avLon.push_back(std::atof(b.c_str()));


						    first_lon = *std::min_element(avLon.begin(),avLon.end());
						    last_lon  = *std::max_element(avLon.begin(),avLon.end());

						    first_lat = *std::min_element(avLat.begin(),avLat.end()); 
						    last_lat  = *std::max_element(avLat.begin(),avLat.end());


						    
						}
						}
		    		}
		    		else
		    		{
		        		std::getline(MetaFi, line);
						std::istringstream iss6;
						iss6.str(line);
						iss6 >> a >> b >> c >> d;

		    		}
				}
	    	}
		}

    }
    MetaFi.close();

    ELISE_ASSERT(!aMetaIsFound, " no metadata found in RPC::ReadASCIIMetaData");

    return EXIT_FAILURE;
}

void RPC::InverseToDirectRPC(const Pt3di &aGridSz)
{
    //Check if inverse exists
    ELISE_ASSERT(IS_INV_INI,"No inverse RPC's for conversion in RPC::InverseToDirectRPC");

    /* What follows is re-writen from DigitalGlobe2Grid 
     * BUT
     * the generated grid is not random but regular in h */
    /****************************************************/

    //Generate a regular grid on the normalized spac 
    vector<Pt3dr> aGridGeoNorm = GenerateNormGrid(aGridSz);

    //Converting the points to image space
    u_int aG;

    vector<Pt3dr> aGridImNorm;
    for (aG = 0; aG < aGridGeoNorm.size(); aG++)
        aGridImNorm.push_back(InverseRPCNorm(aGridGeoNorm[aG]));
    
    GCP2Direct(aGridGeoNorm, aGridImNorm);

    IS_DIR_INI=true;
}

void RPC::InverseToDirectRPC(const Pt2di &aGridSz)
{
    //Check if inverse exists
    ELISE_ASSERT(IS_INV_INI,"No inverse RPC's for conversion in RPC::InverseToDirectRPC");

    /* What follows is re-writen from DigitalGlobe2Grid */
    /****************************************************/

    //Generate a random grid on the normalized spac 
    vector<Pt3dr> aGridGeoNorm = GenerateRandNormGrid(aGridSz);

    //Converting the points to image space
    u_int aG;
    vector<Pt3dr> aGridImNorm;
    for (aG = 0; aG < aGridGeoNorm.size(); aG++)
        aGridImNorm.push_back(InverseRPCNorm(aGridGeoNorm[aG]));
    
    GCP2Direct(aGridGeoNorm, aGridImNorm);

    IS_DIR_INI=true;
}

/* Test calculation of the direct RPCs:
 * calculate mean, max, std of difference in image and ground space between 
 * artificial ground truth and backprojected (image) or forward projected (ground) points */
void RPC::TestDirectRPCGen(const std::string aTargetCS)
{
    int aNb, aK1, aK2, aVTmp0, aVTmp1, aVTmp2; 
    double aRND, aXtmp, aYtmp, aZtmp;
    Pt2dr aMAXdxy(0,0), aMAXdXY(0,0), aUdxy(0,0), aUdXY(0,0), aSdxy(0,0), aSdXY(0,0);
    Pt3dr axyTmp(0,0,0);
    std::vector<Pt3dr> aLPHGT, aLPHFP, aXYHGT, aXYHFP, axyHGT;
    std::vector<Pt2dr> axyBP, adxy, adXY;
    
    Pt2di aGrid(50, 50);
    Pt2dr aStep(double(last_lon - first_lon)/aGrid.x, 
		double(last_lat - first_lat)/aGrid.y );
    int aNNodes = aGrid.x*aGrid.y;

    //aLPHGT - ground truth; generate a random grid (within validity zone) in ground (geodetic CS)
    srand(time(NULL));

    for(aK1=0; aK1<aGrid.x; aK1++)
        for(aK2=0; aK2<aGrid.y; aK2++)
	{
	    aRND = ((double) rand() / (RAND_MAX));
            aLPHGT.push_back(Pt3dr(first_lon + aStep.x*aK1,
				   first_lat + aStep.y*aK2,
			           first_height + (last_height - first_height)*aRND));	    	
	}

    //axyGT - ground truth; back project with inverse RPC to image space
    for(aK1=0; aK1<aNNodes; aK1++)
    {
	axyTmp = InverseRPC(aLPHGT.at(aK1));
        
	axyHGT.push_back(Pt3dr(axyTmp.x, axyTmp.y, aLPHGT.at(aK1).z) );//3rd coordinate is ground H
    }


    //aLPHFP - forward projection of axyHGT and intersection with HGT
    for(aK1=0; aK1<aNNodes; aK1++)
       aLPHFP.push_back(DirectRPC(axyHGT.at(aK1))); 

    //axyBP - backprojection of aLPHFP to image
    for(aK1=0; aK1<aNNodes; aK1++)
    {
	axyTmp = InverseRPC(aLPHFP.at(aK1));
        axyBP.push_back(Pt2dr(axyTmp.x,axyTmp.y));
    }

    //aXYHGT, aXYHFP - convert aLPHGT & aLPHFP to cartographic CS
    ELISE_fp::MkDirSvp("processing");
    std::ofstream aFO("LPHGT_LPHFP.txt");

    for(aK1=0; aK1<aNNodes; aK1++)
        aFO << aLPHGT.at(aK1).x << " " << aLPHGT.at(aK1).y << " " << aLPHGT.at(aK1).z << "\n";
    for(aK1=0; aK1<aNNodes; aK1++)
	aFO << aLPHFP.at(aK1).x << " " << aLPHFP.at(aK1).y << " " << aLPHFP.at(aK1).z << "\n";
    aFO.close();

    std::string aCmdProj = g_externalToolHandler.get("cs2cs").callName() + " " +
	                   "+proj=longlat +datum=WGS84" + " +to " + aTargetCS +
			   " LPHGT_LPHFP.txt  >  XYHGT_XYHFP.txt";

    int aRunOK = system(aCmdProj.c_str());
    ELISE_ASSERT(aRunOK == 0, " Error calling cs2cs");
    
    aVTmp0=0;

    std::ifstream aFI("XYHGT_XYHFP.txt");
    while(aFI.good())
    {
        if(aVTmp0 < aNNodes)
	{
	    aFI >> aXtmp >> aYtmp >> aZtmp;
	    aXYHGT.push_back(Pt3dr(aXtmp, aYtmp, aZtmp));
            aVTmp0++;
	}
	else
	{
	    aFI >> aXtmp >> aYtmp >> aZtmp;
	    aXYHFP.push_back(Pt3dr(aXtmp, aYtmp, aZtmp));
	}
    }
    aFI.close();

    //|axyHGT(:2)-axyBP|, aXYHGT-aXYHFP| - calculate some measures of goodness
    aVTmp1=0, aVTmp2=0;
    aNb=0;

    //mean and max
    for(aK1=0; aK1<aNNodes; aK1++)
    {
	
	if((axyHGT.at(aK1).x >= first_row) &&
	   (axyHGT.at(aK1).x < last_row) &&
	   (axyHGT.at(aK1).y >= first_col) &&
	   (axyHGT.at(aK1).y < last_col) )
	{
	   //image
	   aVTmp1 = std::abs(axyHGT.at(aK1).x - axyBP.at(aK1).x);
	   aVTmp2 = std::abs(axyHGT.at(aK1).y - axyBP.at(aK1).y);
           adxy.push_back( Pt2dr(aVTmp1,aVTmp2) );

	   aUdxy.x += aVTmp1;
	   aUdxy.y += aVTmp2;
	   
	   if(aMAXdxy.x < aVTmp1)
	       aMAXdxy.x = aVTmp1;
	   if(aMAXdxy.y < aVTmp2)
	       aMAXdxy.y = aVTmp2;
	   
           //ground
	   aVTmp1 = std::abs(aXYHGT.at(aK1).x - aXYHFP.at(aK1).x);
	   aVTmp2 = std::abs(aXYHGT.at(aK1).y - aXYHFP.at(aK1).y);
          

           adXY.push_back( Pt2dr(aVTmp1,aVTmp2) );

	   aUdXY.x += aVTmp1;
	   aUdXY.y += aVTmp2;

	   if(aMAXdXY.x < aVTmp1)
	       aMAXdXY.x = aVTmp1;
	   if(aMAXdXY.y < aVTmp2)
	       aMAXdXY.y = aVTmp2;

	   aNb++;
	}
    }
    aUdxy.x = double(aUdxy.x)/aNb;
    aUdxy.y = double(aUdxy.y)/aNb;
    aUdXY.x = double(aUdXY.x)/aNb;
    aUdXY.y = double(aUdXY.y)/aNb;
   

    //standard deviation
    for(aK1=0; aK1<aNb; aK1++)
    {
       //image
       aSdxy.x += (adxy.at(aK1).x - aUdxy.x)*(adxy.at(aK1).x - aUdxy.x);
       aSdxy.y += (adxy.at(aK1).y - aUdxy.y)*(adxy.at(aK1).y - aUdxy.y);

       //ground
       aSdXY.x = (adXY.at(aK1).x - aUdXY.x)*(adXY.at(aK1).x - aUdXY.x);
       aSdXY.y = (adXY.at(aK1).y - aUdXY.y)*(adXY.at(aK1).y - aUdXY.y);
    }
    aSdxy.x = std::sqrt(double(aSdxy.x)/aNb);
    aSdxy.y = std::sqrt(double(aSdxy.y)/aNb);
    aSdXY.x = std::sqrt(double(aSdXY.x)/aNb);
    aSdXY.y = std::sqrt(double(aSdXY.y)/aNb);


    std::cout.precision(5); 
    std::cout << "/**************************************************/\n";
    std::cout << "/******** max, mean, std_dev **********************/\n";
    std::cout << "/******** of the RPC direct calculation ***********/\n";
    std::cout << "/**************************************************/\n";
    std::cout << "\n/******** image space [pix] ***********************/\n";
    std::cout << "max(x,y)     -> " << aMAXdxy.x << " " << aMAXdxy.y << "\n";
    std::cout << "mean(x,y)    -> " << aUdxy.x << " " << aUdxy.y << "\n";
    std::cout << "std_dev(x,y) -> " << aSdxy.x << " " << aSdxy.y << "\n";

    std::cout << "\n/******** ground space [m] ************************/\n";
    std::cout << "max(X,Y)     -> " << aMAXdXY.x << " " << aMAXdXY.y << "\n";
    std::cout << "mean(X,Y)    -> " << aUdXY.x << " " << aUdXY.y << "\n";
    std::cout << "std_dev(X,Y) -> " << aSdXY.x << " " << aSdXY.y << "\n";
    std::cout << "/**************************************************/\n";
    
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                             Function for RPC2D                                             //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////




Pt2dr RPC2D::InverseRPC2D(Pt3dr Pgeo, double aAngle, double aFactor)const
{
	//Converting into normalized coordinates
	Pt2dr PgeoNorm;
	PgeoNorm.x = (Pgeo.x - long_off) / long_scale;
	PgeoNorm.y = (Pgeo.y - lat_off) / lat_scale;

	//Applying inverse RPC
	Pt2dr PimNorm = InverseRPC2DNorm(PgeoNorm);
	//cout << "PgeoNorm = " << PgeoNorm << endl;
	//cout << "PimNorm = " << PimNorm << endl;
	///Converting into Real Coordinates
	Pt2dr Pimg;
	Pimg.x = PimNorm.x * samp_scale + samp_off - cos(aAngle)*Pgeo.z*aFactor; //cout << "Xcor = " << -cos(aAngle)*Pgeo.z*aFactor << endl;
	Pimg.y = PimNorm.y * line_scale + line_off + sin(aAngle)*Pgeo.z*aFactor; //cout << "Ycor = " << sin(aAngle)*Pgeo.z*aFactor << endl;

	return Pimg;
}

Pt2dr RPC2D::InverseRPC2DNorm(Pt2dr PgeoNorm)const
{
	double X = PgeoNorm.x, Y = PgeoNorm.y;
	double vecteurD[] = { 1, X, Y, X*Y, X*X, Y*Y, X*X*X, X*Y*Y, Y*X*X, Y*Y*Y };
	double samp_den = 0.;
	double samp_num = 0.;
	double line_den = 0.;
	double line_num = 0.;

	for (int i = 0; i<10; i++)
	{
		line_num += vecteurD[i] * inverse_line_num_coef[i];
		line_den += vecteurD[i] * inverse_line_den_coef[i];
		samp_num += vecteurD[i] * inverse_samp_num_coef[i];
		samp_den += vecteurD[i] * inverse_samp_den_coef[i];
	}
	//Final computation
	Pt2dr PimgNorm;
	if ((samp_den != 0) && (line_den != 0))
	{
		PimgNorm.x = samp_num / samp_den;
		PimgNorm.y = -(line_num / line_den);//Minus because function is defined for (X,-Y) to have a direct system
	}
	else
	{
		std::cout << "Computing error - denominator = 0" << std::endl;
	}
	return PimgNorm;
}

void RPC2D::ComputeRPC2D(vector<vector<Pt2dr> > aPtsIm, vector<vector<Pt3dr> > aPtsGeo, double aHMax, double aHMin)
{

	//Finding normalization parameters
		//divide Pts into X and Y
	//std::cout << "Size Geo : " << aPtsGeo.size() << " " << aPtsGeo[0].size() << endl;
	//std::cout << "Size Ima : " << aPtsIm.size() << " " << aPtsIm[0].size() << endl;
		vector<double> aPtsGeoX, aPtsGeoY, aPtsImX, aPtsImY;
		for (u_int i = 0; i < aPtsGeo.size(); i++)
		{
			for (u_int j = 0; j < aPtsGeo[0].size(); j++)
			{
				//cout << i << " " << j << endl;
				aPtsGeoX.push_back(aPtsGeo[i][j].x);
				aPtsGeoY.push_back(aPtsGeo[i][j].y);
				aPtsImX.push_back(aPtsIm[i][j].x);
				aPtsImY.push_back(aPtsIm[i][j].y);
			}
		}
		//Find Mins
		Pt2dr aPtGeoMin(*std::min_element(aPtsGeoX.begin(), aPtsGeoX.end()), *std::min_element(aPtsGeoY.begin(), aPtsGeoY.end()));
		Pt2dr aPtGeoMax(*std::max_element(aPtsGeoX.begin(), aPtsGeoX.end()), *std::max_element(aPtsGeoY.begin(), aPtsGeoY.end()));
		Pt2dr aPtImMin(*std::min_element(aPtsImX.begin(), aPtsImX.end()), *std::min_element(aPtsImY.begin(), aPtsImY.end()));
		Pt2dr aPtImMax(*std::max_element(aPtsImX.begin(), aPtsImX.end()), *std::max_element(aPtsImY.begin(), aPtsImY.end()));
		first_lon = aPtGeoMin.x;
		first_lat = aPtGeoMin.y;
		first_col = aPtImMin.x;
		first_row = aPtImMin.y;
		last_lon = aPtGeoMax.x;
		last_lat = aPtGeoMax.y;
		last_col = aPtImMax.x;
		last_row = aPtImMax.y;
		//Compute scales and offsets
		long_scale = (aPtGeoMax.x - aPtGeoMin.x) / 2;
		lat_scale = (aPtGeoMax.y - aPtGeoMin.y) / 2;
		samp_scale = (aPtImMax.x - aPtImMin.x) / 2;
		line_scale = (aPtImMax.y - aPtImMin.y) / 2;
		long_off = (aPtGeoMax.x + aPtGeoMin.x) / 2;;
		lat_off = (aPtGeoMax.y + aPtGeoMin.y) / 2;
		samp_off = (aPtImMax.x + aPtImMin.x) / 2;
		line_off = (aPtImMax.y + aPtImMin.y) / 2;
		height_scale = (aHMax - aHMin) / 2;
		height_off = (aHMax + aHMin) / 2;
		//std::cout << "Scales and offsets computed" << endl;
	//Parameters to get parameters of P1 and P2 in ---  Column=P1(X,Y)/P2(X,Y)  --- where (X,Y) are Geo coordinates (idem for -Row -- minus to have a direct coord system)
	//Function is 0=Poly1(X,Y)-Column*Poly2(X,Y) ==> Column*k=a+bX+cY+dXY+eX^2+fY^2+gX^2Y+hXY^2+iX^3+jY^3-Column(lX+mY+nXY+oX^2+pY^2+qX^2Y+rXY^2+sX^3+tY^3)
	//k=1 to avoid sol=0
	//All coordinates are in normalized space
	L2SysSurResol aSysCol(19), aSysRow(19);

	//For all lattice points
	for (u_int i = 0; i < aPtsGeo.size(); i++)
	{
		for (u_int j = 0; j < aPtsGeo[0].size(); j++)
		{
			//NORMALIZATION
			double X = (aPtsGeo[i][j].x - long_off) / long_scale;
			double Y = (aPtsGeo[i][j].y - lat_off) / lat_scale;
			double COL = (aPtsIm[i][j].x - samp_off) / samp_scale;
			double ROW = -(aPtsIm[i][j].y - line_off) / line_scale;

			double aEqCol[19] = { 1, X, Y, X*Y, X*X, Y*Y, X*X*X, X*Y*Y, Y*X*X, Y*Y*Y,
				-COL*X, -COL*Y, -COL*X*Y, -COL*X*X, -COL*Y*Y, -COL*X*X*X, -COL*X*Y*Y, -COL*Y*X*X, -COL*Y*Y*Y };
			aSysCol.AddEquation(1, aEqCol, COL);


			double aEqRow[19] = { 1, X, Y, X*Y, X*X, Y*Y, X*X*X, X*Y*Y, Y*X*X, Y*Y*Y,
				-ROW*X, -ROW*Y, -ROW*X*Y, -ROW*X*X, -ROW*Y*Y, -ROW*X*X*X, -ROW*X*Y*Y, -ROW*Y*X*X, -ROW*Y*Y*Y };
			aSysRow.AddEquation(1, aEqRow, ROW);
		}
	}

	//Computing the result
	bool Ok;
	Im1D_REAL8 aSolCol = aSysCol.GSSR_Solve(&Ok);
	Im1D_REAL8 aSolRow = aSysRow.GSSR_Solve(&Ok);
	double* aDataCol = aSolCol.data();
	double* aDataRow = aSolRow.data();

	//Copying Data in RPC2D object
	//Numerators
	for (int i = 0; i<10; i++)
	{
		inverse_samp_num_coef.push_back(aDataCol[i]);
		inverse_line_num_coef.push_back(aDataRow[i]);
	}
	//Denominators (first one = 1)
	inverse_samp_den_coef.push_back(1);
	inverse_line_den_coef.push_back(1);
	for (int i = 10; i<19; i++)
	{
		inverse_samp_den_coef.push_back(aDataCol[i]);
		inverse_line_den_coef.push_back(aDataRow[i]);
	}


}

Pt3dr RPC2D::InversePreRPCNorm(Pt3dr aPtGeoNorm, vector<vector<Pt3dr> > aMatPtsGeo, vector<vector<Pt3dr> > aMatSatPos)
{
	//cout << aPtGeoNorm << endl;
	//Convert Normalized coordinates to ground geodetic coords
	Pt3dr aPtGeo;
	aPtGeo.x = aPtGeoNorm.x * long_scale + long_off;
	aPtGeo.y = aPtGeoNorm.y * lat_scale + lat_off;
	aPtGeo.z = aPtGeoNorm.z * height_scale + height_off;

	//Compute angle for altitude correction
	Pt3dr aPtGeoDodgeS(aPtGeo.x, aPtGeo.y - 0.00001, aPtGeo.z);
	Pt3dr aPtGeoDodgeN(aPtGeo.x, aPtGeo.y + 0.00001, aPtGeo.z);
	Pt2dr aPtImDodgeS = InverseRPC2D(aPtGeoDodgeS, 0, 0);
	Pt2dr aPtImDodgeN = InverseRPC2D(aPtGeoDodgeN, 0, 0);
	double aAngle = -atan(abs(aPtImDodgeS.y - aPtImDodgeN.y) / abs(aPtImDodgeS.x - aPtImDodgeN.x));

	Pt3dr aPtGeoDodgeAngle(aPtGeo.x - cos(aAngle) / 111111, aPtGeo.y - sin(aAngle) / 111111, aPtGeo.z);

	//Defining local plane
	Pt3dr aPtGeoDodgeE(aPtGeo.x + 0.00001, aPtGeo.y, aPtGeo.z);
	//cout << aPtGeo << endl;
	//cout << aPtGeoDodgeN << endl;
	//cout << aPtGeoDodgeE << endl;
	//cout << aPtGeoDodgeAngle << endl;

	vector<Pt3dr> aVPtsPlaneECEF;
	// Creating a file with coordinates of point
	{
		std::ofstream fic("processing/localPlane_geo.txt");
		fic << std::setprecision(15);
		fic << aPtGeo.x << " " << aPtGeo.y << endl;
		fic << aPtGeoDodgeN.x << " " <<aPtGeoDodgeN.y << endl;
		fic << aPtGeoDodgeE.x << " " << aPtGeoDodgeE.y << endl;
		fic << aPtGeoDodgeAngle.x << " " << aPtGeoDodgeAngle.y << endl;
	}
	
	// transformation in the ground coordinate system
	std::string command;
	command = g_externalToolHandler.get("cs2cs").callName() + " +proj=longlat +datum=WGS84 +to +proj=geocent +ellps=WGS84 processing/localPlane_geo.txt > processing/localPlane_ECEF.txt";
	int res = system(command.c_str());
	if (res != 0) std::cout << "error calling cs2cs in Defining local plane" << std::endl;
	// loading points
	std::ifstream fic("processing/localPlane_ECEF.txt");
	while (!fic.eof() && fic.good())
	{
		double X, Y, Z;
		fic >> X >> Y >> Z;
		if (fic.good())
			aVPtsPlaneECEF.push_back(Pt3dr(X, Y, Z));
	}

	Pt3dr aPtECEFDodgeAngle = aVPtsPlaneECEF[3];

	//Finding satellite position for point aPtGeoDodgeAngle (aSatPosLoc)
	//Compute the position of the point in 11*16 matrix space and get equivalent (aSatPosLoc) in the aMatSatPos matrix

	Pt3dr aSatPosLoc;
	//Finding the four points around the point
	Pt3dr aPtGeoSol = aPtGeo; aPtGeoSol.z = 0;
	for (u_int i = 0; i < aMatPtsGeo.size() - 1; i++)
	{
		for (u_int j = 0; j < aMatPtsGeo[i].size() - 1; j++)
		{
			//std::cout << i << " - " << j << " : " << aMatPtsGeo[i][j] << endl;
			//std::cout << i << " " << j << endl;
			//std::cout << aMatPtsGeo[i][j] << aMatPtsGeo[i + 1][j] << aMatPtsGeo[i][j + 1] << aMatPtsGeo[i + 1][j + 1] << endl;
			if (aPtGeoSol == aMatPtsGeo[i][j])
			{
				aSatPosLoc = aMatSatPos[i][j];
			}
			else if (aPtGeoSol == aMatPtsGeo[i + 1][j])
			{
				aSatPosLoc = aMatSatPos[i + 1][j];
			}
			else if (aPtGeoSol == aMatPtsGeo[i][j + 1])
			{
				aSatPosLoc = aMatSatPos[i][j + 1];
			}
			else if (aPtGeoSol == aMatPtsGeo[i + 1][j + 1])
			{
				aSatPosLoc = aMatSatPos[i + 1][j + 1];
			}
			else if ((aMatPtsGeo[i][j].y >= aPtGeoSol.y && aMatPtsGeo[i][j + 1].x >= aPtGeoSol.x && aMatPtsGeo[i + 1][j].x <= aPtGeoSol.x && aMatPtsGeo[i + 1][j + 1].y <= aPtGeoSol.y) ||
				(aMatPtsGeo[i][j].y <= aPtGeoSol.y && aMatPtsGeo[i][j + 1].x <= aPtGeoSol.x && aMatPtsGeo[i + 1][j].x >= aPtGeoSol.x && aMatPtsGeo[i + 1][j + 1].y >= aPtGeoSol.y))
			{
				//then the point is in the "square"
				//Computing the distance from the points to the corners of the "square"
				double D1 = euclid(aPtGeoSol - aMatPtsGeo[i][j]), D2 = euclid(aPtGeoSol - aMatPtsGeo[i][j + 1]), D3 = euclid(aPtGeoSol - aMatPtsGeo[i + 1][j]), D4 = euclid(aPtGeoSol - aMatPtsGeo[i + 1][j + 1]);
				//cout << D1 << " " << D2 << " " << D3 << " " << D4<< endl;
				double sumD = 1/D1 + 1/D2 + 1/D3 + 1/D4;
				//cout << sumD << endl;
				//cout << aMatSatPos << endl;
				//cout << aMatSatPos[i][j] << endl;
				//cout << aMatSatPos[i][j + 1] << endl;
				//cout << aMatSatPos[i+1][j] << endl;
				//cout << aMatSatPos[i+1][j+1] << endl;
				//aSatPosLoc.x = (aMatSatPos[i][j].x/D1 + aMatSatPos[i][j + 1].x/D2 + aMatSatPos[i + 1][j].x/D3 + aMatSatPos[i + 1][j + 1].x/D4) / sumD;
				//aSatPosLoc.y = (aMatSatPos[i][j].y/D1 + aMatSatPos[i][j + 1].y/D2 + aMatSatPos[i + 1][j].y/D3 + aMatSatPos[i + 1][j + 1].y/D4) / sumD;
				//aSatPosLoc.z = (aMatSatPos[i][j].z/D1 + aMatSatPos[i][j + 1].z/D2 + aMatSatPos[i + 1][j].z/D3 + aMatSatPos[i + 1][j + 1].z/D4) / sumD;
				aSatPosLoc = (aMatSatPos[i][j] / D1 + aMatSatPos[i][j + 1] / D2 + aMatSatPos[i + 1][j] / D3 + aMatSatPos[i + 1][j + 1] / D4) / sumD;
			}
			//std::cout << i << " - " << j << " : " << aSatPosLoc << endl;
		}
	}



	//cout << std::setprecision(15) << "A0 = " << aVPtsPlaneECEF[0] << endl;
	//cout << "B0 = " << aPtECEFDodgeAngle << endl;
	//cout << "aSatPosLoc = " << aSatPosLoc << endl;
	//Computing the normal
	Pt3dr aNormal = (aVPtsPlaneECEF[1] - aVPtsPlaneECEF[0]) ^ (aVPtsPlaneECEF[2] - aVPtsPlaneECEF[0]);
	//cout << "aNormal = " << aNormal << endl;
	//aSatPosProj is InterSeg on (aPtGeo aPtGeoDodgeAngle)/X/(SatPosLoc SatPosLoc-normal)

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool Ok;
	Pt3dr aSatPosProj = InterSeg(aVPtsPlaneECEF[0], aPtECEFDodgeAngle, aSatPosLoc, aSatPosLoc - aNormal,Ok);
        // MPD : maintenant InterSeg indique si l'intersection est degeneree, il convient sand doute de mieux gere cette erreur ....
        ELISE_ASSERT(Ok,"Bad intersection in RPC2D::InversePreRPCNorm");
	//cout << "aSatPosProj = " << aSatPosProj << endl;

	//Computing distance between aPtGeoDodgeAngle and aSatPosProj, and aSatHeight
	double aSatPosProj2aPtGeoDodgeAngle = euclid(aSatPosProj - aPtECEFDodgeAngle);
	double aSatHeight = euclid(aSatPosProj - aSatPosLoc);

	//Compute correction factor (dependent on height)
	double tanBeta = aSatPosProj2aPtGeoDodgeAngle / aSatHeight;
	//cout << "aSatPosProj2aPtGeoDodgeAngle = " << aSatPosProj2aPtGeoDodgeAngle << endl;
	//cout << "aSatHeight = " << aSatHeight << endl;
	//cout << "tanBeta = " << tanBeta << endl;
		//Compute point Dodged of (1-tanBeta) * 1/111111th of a degree (about 1m) in the same direction as aPtGeoDodgeAngle
		Pt3dr aPtGeoDodgeTanBeta(aPtGeo.x - cos(aAngle)*(1 - tanBeta) / 111111, aPtGeo.y - sin(aAngle)*(1 - tanBeta) / 111111, aPtGeo.z);
		//cout << "aPtGeoDodgeTanBeta = " << aPtGeoDodgeTanBeta << endl;
		//cout << "aPtGeoDodgeAngle = " << aPtGeoDodgeAngle << endl;
		//Compute positions in image
		Pt2dr aPtImDodgeAngle = InverseRPC2D(aPtGeoDodgeAngle, 0, 0);
		Pt2dr aPtImDodgeTanBeta = InverseRPC2D(aPtGeoDodgeTanBeta, 0, 0);

		double aFactor = euclid(aPtImDodgeAngle - aPtImDodgeTanBeta);
		
	//Final computation of position of point in image
		Pt2dr aPtIm=InverseRPC2D(aPtGeoDodgeTanBeta, aAngle, aFactor);
		//cout << "Factor = " << aFactor << "  -  Angle = " << aAngle << endl;
		//cout<< "PointIm = " << aPtIm << endl;
	//Normalization
	Pt3dr aPtImNorm;
	aPtImNorm.x = (aPtIm.x - samp_off) / samp_scale;
	aPtImNorm.y = (aPtIm.y - line_off) / line_scale;
	aPtImNorm.z = aPtGeoNorm.z;

	return aPtImNorm;
}

vector<Pt3dr> RPC2D::filterOutOfBound(vector<Pt3dr> aVectorGeoNormIN, vector<vector<Pt3dr> > aMatPtsGeo)
{
	Pt2dr lrc;
	Pt2dr llc;
	Pt2dr urc;
	Pt2dr ulc;
	//If flying N->S
	if (aMatPtsGeo[0][0].y>aMatPtsGeo[aMatPtsGeo.size() - 1][0].y)
	{
		ulc.x = (aMatPtsGeo[0][0].x - long_off) / long_scale; ulc.y = (aMatPtsGeo[0][0].y - lat_off) / lat_scale;
		urc.x = (aMatPtsGeo[0][aMatPtsGeo[0].size() - 1].x - long_off) / long_scale; urc.y = (aMatPtsGeo[0][aMatPtsGeo[0].size() - 1].y - lat_off) / lat_scale;
		llc.x = (aMatPtsGeo[aMatPtsGeo.size() - 1][0].x - long_off) / long_scale; llc.y = (aMatPtsGeo[aMatPtsGeo.size() - 1][0].y - lat_off) / lat_scale;
		lrc.x = (aMatPtsGeo[aMatPtsGeo.size() - 1][aMatPtsGeo[0].size() - 1].x - long_off) / long_scale; lrc.y = (aMatPtsGeo[aMatPtsGeo.size() - 1][aMatPtsGeo[0].size() - 1].y - lat_off) / lat_scale;
	}
	//if flying S->N
	else
	{
		lrc.x = (aMatPtsGeo[0][0].x - long_off) / long_scale; lrc.y = (aMatPtsGeo[0][0].y - lat_off) / lat_scale;
		llc.x = (aMatPtsGeo[0][aMatPtsGeo[0].size() - 1].x - long_off) / long_scale; llc.y = (aMatPtsGeo[0][aMatPtsGeo[0].size() - 1].y - lat_off) / lat_scale;
		urc.x = (aMatPtsGeo[aMatPtsGeo.size() - 1][0].x - long_off) / long_scale; urc.y = (aMatPtsGeo[aMatPtsGeo.size() - 1][0].y - lat_off) / lat_scale;
		ulc.x = (aMatPtsGeo[aMatPtsGeo.size() - 1][aMatPtsGeo[0].size() - 1].x - long_off) / long_scale; ulc.y = (aMatPtsGeo[aMatPtsGeo.size() - 1][aMatPtsGeo[0].size() - 1].y - lat_off) / lat_scale;
	}
	//cout << ulc << " - " << urc << " - " << llc << " - " << lrc << endl;
	//Finding lines parameters
		double ulcurcA = (urc.y - ulc.y) / (urc.x - ulc.x);
		double ulcurcB = ulc.y - ulcurcA * ulc.x;

		double lrcurcA = (urc.y - lrc.y) / (urc.x - lrc.x);
		double lrcurcB = lrc.y - lrcurcA * lrc.x;

		double llclrcA = (lrc.y - llc.y) / (lrc.x - llc.x);
		double llclrcB = llc.y - llclrcA * llc.x;

		double ulcllcA = (llc.y - ulc.y) / (llc.x - ulc.x);
		double ulcllcB = ulc.y - ulcllcA * ulc.x;

		
	//test if each point is inside [ulc,urc,lrc,llc]
	vector<Pt3dr> aVectorGeoNormOUT;
	for (u_int i = 0; i < aVectorGeoNormIN.size(); i++)
	{
		double X = aVectorGeoNormIN[i].x; double Y = aVectorGeoNormIN[i].y;
		if ((X < (Y - ulcurcB) / ulcurcA) && (Y < ulcurcA*X + ulcurcB)
			&& (X < (Y - lrcurcB) / lrcurcA) && (Y > lrcurcA*X + lrcurcB)
			&& (X > (Y - llclrcB) / llclrcA) && (Y > llclrcA*X + llclrcB)
			&& (X > (Y - ulcllcB) / ulcllcA) && (Y < ulcllcA*X + ulcllcB))
		{
			aVectorGeoNormOUT.push_back(aVectorGeoNormIN[i]);
		}
	}

	return aVectorGeoNormOUT;
}

int RPC_main(int argc, char ** argv)
{
	cout << "This command isn't doing anything right now" << endl;

    return 0;
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
