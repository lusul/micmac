#include "StdAfx.h"

#ifdef __USE_JP2__

#include <iostream>
#include <vector>
#include <complex>

#ifdef INT
	#undef INT
#endif

#include "Jp2ImageLoader.h"


// Kakadu core includes
#include "kdu_arch.h"
#include "kdu_elementary.h"
#include "kdu_messaging.h"
#include "kdu_params.h"
#include "kdu_compressed.h"
#include "kdu_sample_processing.h"
#include "kdu_stripe_decompressor.h"
// Application includes
#include "kdu_args.h"
#include "kdu_file_io.h"
#include "jp2.h"


#ifdef __USE_IMAGEIGN__
#include <ign/image/BufferImage.h>
#include <boost/filesystem.hpp>
#endif
namespace NS_ParamMICMAC
{	
	JP2ImageLoader::JP2ImageLoader(std::string const &nomfic)
	{
		m_Nomfic=nomfic;
		//std::cout << "constructeur : "<<nomfic<<std::endl;
		bool verbose = false;
		jp2_source              m_Source;
		kdu_compressed_source * m_Input;
		jp2_family_src          jp2_ultimate_src;
		kdu_codestream          codestream;
		
		m_Input=&m_Source;
		//std::cout << "m_Input  : "<<m_Input<<std::endl;
		jp2_ultimate_src.open(nomfic.c_str());
		if (!m_Source.open(&jp2_ultimate_src))
		{
			std::cout << "ce n'est pas un fichier valide"<<std::endl;
			return;
		} 
		m_Source.read_header();
		codestream.create(m_Input);
		
		int num_components = codestream.get_num_components(true);
		if (verbose) std::cout << "Nombre de components : "<<num_components<<std::endl;
		
		m_Nbc = codestream.get_num_components(true);
		m_BPS = codestream.get_bit_depth(0);
		m_S   = codestream.get_signed(0);
		kdu_dims dims;
		codestream.get_dims(0,dims);
		m_SzIm=std::complex<int>(dims.size.x,dims.size.y);
		
		if (m_BPS == 8) m_Type = eUnsignedChar;
		else if (m_BPS == 16)
		{
			if (m_S) m_Type = eSignedShort;
			else m_Type = eUnsignedShort;
		}
		else if (m_BPS == 4) m_Type = eFloat;
		else m_Type = eOther;
		
		if (verbose)
		{
			for(int n=0;n<num_components;++n)
			{
				std::cout << "n="<<n<<std::endl;
				int nb_bits = codestream.get_bit_depth(n);
				bool S = codestream.get_signed(n);
				std::cout << "Nombre de bits : "<<nb_bits;
				if (S) std::cout << " signes "<<std::endl;
				else std::cout << " non signes "<<std::endl;
				kdu_dims dims;
				codestream.get_dims(n,dims);
				std::cout << dims.access_pos()->get_x()<<" "<<dims.access_pos()->get_y()<<" : "<<dims.access_size()->get_x()<<" "<<dims.access_size()->get_y()<<std::endl;
			}
		}
		
		if (verbose) std::cout << "Nombre deZoom dispo : "<<codestream.get_min_dwt_levels()<<std::endl;
		
		
		codestream.destroy();
		m_Input->close();
		if (jp2_ultimate_src.exists()) jp2_ultimate_src.close();
		
		
		if (verbose) std::cout << "Fin du constructeur"<<std::endl;
	}	
	
	
	
	void JP2ImageLoader::LoadNCanaux(const std::vector<sLowLevelIm<short unsigned int> > & aVImages,
									 int              mFlagLoadedIms,
									 int              aDeZoom,
									 tPInt            aP0Im,
									 tPInt            aP0File,
									 tPInt            aSz
									 )
	{
		bool verbose = 0;
		if (verbose) std::cout << "LoadNCanaux en unsigned short"<<std::endl;
		int precision = 16;
		bool signe = false;
		
		jp2_source              m_Source;
		kdu_compressed_source * m_Input;
		jp2_family_src          jp2_ultimate_src;
		kdu_codestream          codestream;
		
		m_Input=&m_Source;
		jp2_ultimate_src.open(m_Nomfic.c_str());
		if (!m_Source.open(&jp2_ultimate_src)) return;
		m_Source.read_header();
		codestream.create(m_Input);
		
		
        //int dz = aDeZoom;
		int max_layers = 0;
        int discard_levels = 0;
        while(((1 << discard_levels) & aDeZoom)==0) ++discard_levels;
        //std::cout << "discard_levels : "<<discard_levels<<std::endl;
		int minDwtLevels = codestream.get_min_dwt_levels();
		int reDeZoom = 0;
		if (discard_levels>minDwtLevels)
		{
			reDeZoom = (1<<(discard_levels-minDwtLevels));
			discard_levels=minDwtLevels;
            //dz = (1 << discard_levels);
			//std::cout << "On fait un dz "<<dz<<" puis on fera un ssech "<<reDeZoom<<std::endl;
		}
		//std::cout << "reDeZoom : "<<reDeZoom<<std::endl;
		
		int * precisions = new int[aVImages.size()];
		bool *is_signed = new bool[aVImages.size()];
		for(size_t i=0;i<aVImages.size();++i) precisions[i]=precision;
		for(size_t i=0;i<aVImages.size();++i) is_signed[i]=signe;
		kdu_dims dims,mdims;
		// Position de l'origine en coord fichier (cad plein resolution)
		dims.pos=kdu_coords(aP0File.real()*aDeZoom,aP0File.imag()*aDeZoom);
		// Taille de la zone en coord fichier (cad pleine resolution)
		dims.size=kdu_coords(aSz.real()*aDeZoom,aSz.imag()*aDeZoom);
		codestream.map_region(0,dims,mdims);
		int premier_canal = -1;
		int dernier_canal = -1;
		for(int c=0;c<m_Nbc;++c)
		{
			if ((1<<c)&(mFlagLoadedIms))
			{
				if (premier_canal==-1) premier_canal=c;
				dernier_canal = c;
			}
		}
		if (premier_canal==-1)
		{
			premier_canal=0;
			dernier_canal=m_Nbc;
		}
		int nb_canaux = dernier_canal-premier_canal+1;
		if (nb_canaux>(int)aVImages.size()) nb_canaux=aVImages.size();	
		
		codestream.apply_input_restrictions(premier_canal,nb_canaux,discard_levels,max_layers,&mdims,KDU_WANT_OUTPUT_COMPONENTS);	
		
		kdu_thread_env env, *env_ref=NULL;
		int n, num_components = codestream.get_num_components(true);
		kdu_dims *comp_dims = new kdu_dims[num_components];
		for (n=0; n < num_components; n++)
			codestream.get_dims(n,comp_dims[n],true);
		int *stripe_heights = new int[num_components];
		int *sample_gaps = NULL;
		int *row_gaps = NULL;
		
		kdu_int16 **stripe_bufs = new kdu_int16 *[num_components];
		
		if (reDeZoom>0)
		{
			for(n=0;n<num_components;++n)
			{
				stripe_bufs[n] = new kdu_int16[comp_dims[n].size.x];
			}
		}
		
		int env_dbuf_height = 0;
		//int preferred_min_stripe_height = 8;
		//int absolute_max_stripe_height = 1024;
		kdu_stripe_decompressor decompressor;
		decompressor.start(codestream,false,false,env_ref,NULL,env_dbuf_height);
		for (n=0; n < num_components; n++) stripe_heights[n] = 1; 	
		for(int l=0;l<aSz.imag();++l)
		{
			if (reDeZoom<=0) for(n=0;n<num_components;++n) stripe_bufs[n] = (short*)&(aVImages[n].mData[aP0Im.imag()+l][aP0Im.real()]);
			decompressor.pull_stripe(stripe_bufs,stripe_heights,sample_gaps,row_gaps,precisions,is_signed);
			if (reDeZoom>0)
			{ 
				for(n=0;n<num_components;++n)
					for(int c=0;c<aSz.real();++c)
						aVImages[n].mData[aP0Im.imag()+l][c] = stripe_bufs[n][c*reDeZoom];
				for(int ll=1;ll<reDeZoom;++ll) decompressor.pull_stripe(stripe_bufs,stripe_heights,sample_gaps,row_gaps,precisions,is_signed);
			}
		}
		
		if (reDeZoom) 
		{
			for(n=0;n<num_components;++n) delete[] stripe_bufs[n];
			delete[] sample_gaps;
			delete[] row_gaps;
		}	
		
		// Clean up
		decompressor.finish();
		if (env.exists())
			env.destroy(); 
		codestream.destroy();	
		m_Input->close();
		if (jp2_ultimate_src.exists()) jp2_ultimate_src.close();
		delete[] precisions;
		delete[] stripe_heights;
		delete[] stripe_bufs;
        delete[] comp_dims;
	}
#ifdef __USE_IMAGEIGN__
	void _debug_IO(const std::vector<sLowLevelIm<float> > & aVImages, cInterfModuleImageLoader::tPInt aSz, int band, const std::string &filename, const std::string &filename2)
	{
		std::cout<<"on ouvre en ecriture: "<<filename2<<std::endl;
		Tiff_Im  aTOut( filename2.c_str(), Pt2di(aSz.real(),aSz.imag()), GenIm::real4,
					   Tiff_Im::No_Compr,  Tiff_Im::BlackIsZero );
		std::cout<<"on initialise l'image"<<std::endl;

		Im2D<float,double> aIm(aVImages[band].mDataLin,aVImages[band].mData,aSz.real(),aSz.imag());
		
		
		ELISE_COPY
		(
		 aTOut.all_pts(),
		 aIm.in(),
		 aTOut.out()
		 );
		
		//check en relecture
		/*ign::image::BufferImage<float> img1(filename);
		ign::image::BufferImage<float> img2(filename2);
		for (int l = 0; l < aSz.imag(); ++l)
			for (int c  = 0; c <aSz.real(); ++c)
				if (img1.get(c,l,0) != img2.get(c,l,0))
				{
					std::ostringstream oss;
					oss<<"Incoherence dans "<<filename<<" au pixel: "<<c<<", "<<l;
					IGN_THROW_EXCEPTION(oss.str());
				}
		*/
		//boost::filesystem::remove(filename);
		//boost::filesystem::remove(oss2.str());
	}
#endif
	void JP2ImageLoader::LoadNCanaux(const std::vector<sLowLevelIm<float> > & aVImages,
									 int              mFlagLoadedIms,
									 int              aDeZoom,
									 tPInt            aP0Im,
									 tPInt            aP0File,
									 tPInt            aSz
									 )
	{
		bool verbose  = 1;
		if (verbose) std::cout << "LoadNCanaux en float "<<aDeZoom<<" - "<<aP0Im.real()<<" "<<aP0Im.imag()<<" - "<<aP0File.real()<<" "<<aP0File.imag()<<" - "<<aSz.real()<<" "<<aSz.imag()<<std::endl;
		bool avecDeZoom=false;

		int precision = 16;
		bool signe = false;
		
		jp2_source              m_Source;
		kdu_compressed_source * m_Input;
		jp2_family_src          jp2_ultimate_src;
		kdu_codestream          codestream;
		
		m_Input=&m_Source;
		jp2_ultimate_src.open(m_Nomfic.c_str());
		if (!m_Source.open(&jp2_ultimate_src)) 
		{
			if (verbose) std::cout << "LoadNCanaux failed on "<<m_Nomfic<<std::endl;
			return;
		}
		m_Source.read_header();
		codestream.create(m_Input);
		
		int dz = aDeZoom;
		int max_layers = 0;
        int discard_levels = 0;
        while(((1 << discard_levels) & aDeZoom)==0) ++discard_levels;
        if (verbose) std::cout << "discard_levels : "<<discard_levels<<std::endl;
		int minDwtLevels = 0;
		if (avecDeZoom)
			minDwtLevels = std::max<int>(0,std::min<int>(codestream.get_min_dwt_levels(),discard_levels-1));
		else
			minDwtLevels = codestream.get_min_dwt_levels();
		int reDeZoom = 0;
		if (discard_levels>minDwtLevels)
		{
			reDeZoom = (1<<(discard_levels-minDwtLevels));
			discard_levels=minDwtLevels;
			dz = (1 << discard_levels);
			if (verbose) std::cout << "On fait un dz "<<dz<<" puis on fera un ssech "<<reDeZoom<<std::endl;
		}
		if (verbose) std::cout << "reDeZoom : "<<reDeZoom<<std::endl;
		
		int * precisions = new int[aVImages.size()];
		bool *is_signed = new bool[aVImages.size()];
		for(size_t i=0;i<aVImages.size();++i) precisions[i]=precision;
		for(size_t i=0;i<aVImages.size();++i) is_signed[i]=signe;
		kdu_dims dims,mdims;
		// Position de l'origine en coord fichier (cad plein resolution)
		dims.pos=kdu_coords(aP0File.real()*aDeZoom,aP0File.imag()*aDeZoom);
		// Taille de la zone en coord fichier (cad pleine resolution)
		dims.size=kdu_coords(aSz.real()*aDeZoom,aSz.imag()*aDeZoom);
		codestream.map_region(0,dims,mdims);
		if (verbose) std::cout << "mdims : "<<mdims.pos.x<<" "<<mdims.pos.y<<" "<<mdims.size.x<<" "<<mdims.size.y<<std::endl;
		int premier_canal = -1;
		int dernier_canal = -1;
		for(int c=0;c<m_Nbc;++c)
		{
			if ((1<<c)&(mFlagLoadedIms))
			{
				if (premier_canal==-1) premier_canal=c;
				dernier_canal = c;
			}
		}
		if (premier_canal==-1)
		{
			premier_canal=0;
			dernier_canal=m_Nbc;
		}
		int nb_canaux = dernier_canal-premier_canal+1;
		if (nb_canaux>(int)aVImages.size()) nb_canaux=aVImages.size();	
		
		codestream.apply_input_restrictions(premier_canal,nb_canaux,discard_levels,max_layers,&mdims,KDU_WANT_OUTPUT_COMPONENTS);	
		
		kdu_thread_env env, *env_ref=NULL;
		int n, num_components = codestream.get_num_components(true);
		kdu_dims *comp_dims = new kdu_dims[num_components];
		for (n=0; n < num_components; n++)
			codestream.get_dims(n,comp_dims[n],true);
		int *stripe_heights = new int[num_components];
		int *sample_gaps = NULL;
		int *row_gaps = NULL;
        
        kdu_int16 **stripe_bufs = new kdu_int16 *[num_components];
		
		{
			for(n=0;n<num_components;++n)
			{
				stripe_bufs[n] = new kdu_int16[comp_dims[n].size.x];
			}
		}
        
        std::vector<float> coef;
        float norm = 0.f;
        float d2 = (float)(reDeZoom-1.)/2.;
        float sigma = 0.5*d2;
        for(int l=0;l<reDeZoom;++l)
        {
            for(int c=0;c<reDeZoom;++c)
            {
                float dist = (l-d2)*(l-d2) + (c-d2)*(c-d2);
                float cf = exp(-dist/(2.*sigma*sigma));
                coef.push_back(cf);
                norm += cf;
            }
        }
        if (verbose) std::cout << "norm : "<<norm<<std::endl;
		
		int env_dbuf_height = 0;
		//int preferred_min_stripe_height = 8;
		//int absolute_max_stripe_height = 1024;
		kdu_stripe_decompressor decompressor;
		decompressor.start(codestream,false,false,env_ref,NULL,env_dbuf_height);
		for (n=0; n < num_components; n++) stripe_heights[n] = 1; 	
		for(int l=0;l<aSz.imag();++l)
		{
			if (reDeZoom>0)
			{ 
//				float R2 = reDeZoom*reDeZoom;
				for(int ll=0;ll<reDeZoom;++ll)
				{
					decompressor.pull_stripe(stripe_bufs,stripe_heights,sample_gaps,row_gaps,precisions,is_signed);
                    
					for(n=0;n<num_components;++n)
					{
						float* pt_out = &(aVImages[n].mData[aP0Im.imag()+l][aP0Im.real()]);
						kdu_int16* pt_buf = &(stripe_bufs[n][0]);
						for(int c=0;c<aSz.real();++c)
						{
							if (ll==0) (*pt_out) = 0;
							for(int cc=0;cc<reDeZoom;++cc)
							{
								(*pt_out)+=(float)(*pt_buf) * coef[cc+ll*reDeZoom];
								++pt_buf;
							}
                            
							if (ll==(reDeZoom-1))
                            {
                                (*pt_out) /=(float)norm;
                            }
							++pt_out;
						}
					}
				}
			}
			else
			{
				decompressor.pull_stripe(stripe_bufs,stripe_heights,sample_gaps,row_gaps,precisions);
				for(n=0;n<num_components;++n)
					for(int c=0;c<aSz.real();++c)
						aVImages[n].mData[aP0Im.imag()+l][aP0Im.real()+c] = (float)((stripe_bufs[n][c]));
			}
		}
		
		{
			for(n=0;n<num_components;++n) delete[] stripe_bufs[n];
			delete[] sample_gaps;
			delete[] row_gaps;
		}	
		
		// Clean up
		decompressor.finish();
		if (env.exists())
			env.destroy(); 
		codestream.destroy();	
		m_Input->close();
		if (jp2_ultimate_src.exists()) jp2_ultimate_src.close();
		delete[] precisions;
		delete[] stripe_heights;
		delete[] stripe_bufs;

        delete[] is_signed;
        delete[] comp_dims;
		
#ifdef __USE_IMAGEIGN__
		std::ostringstream oss;
		oss<<"/Temp/"<<boost::filesystem::path(m_Nomfic).stem().generic_string()<<"_testELISE"<<0<<".TIF";
		std::ostringstream oss2;
		oss2<<"/Temp/"<<boost::filesystem::path(m_Nomfic).stem().generic_string()<<"_testKAK"<<0<<".TIF";
		_debug_IO(aVImages, aSz, 0, oss.str(), oss2.str());
#endif
        
        /*
        Tiff_Im  aTOut
        (
         "toto.tif",
         Pt2di(aSz.real(),aSz.imag()),
         GenIm::real4,
         Tiff_Im::No_Compr,
         Tiff_Im::BlackIsZero
         );
        
        Im2D<float,double> aIm(aVImages[0].mDataLin,aVImages[0].mData,aSz.real(),aSz.imag());
        
        
        ELISE_COPY
        (
         aTOut.all_pts(),
         aIm.in(),
         aTOut.out()
         );
         */
	}
	
	void JP2ImageLoader::LoadNCanaux(const std::vector<sLowLevelIm<unsigned char> > & aVImages,
									 int              mFlagLoadedIms,
									 int              aDeZoom,
									 tPInt            aP0Im,
									 tPInt            aP0File,
									 tPInt            aSz
									 )
	{
		bool verbose  = 0;
		if (verbose) std::cout << "LoadNCanaux en unsigned char"<<std::endl;
		int precision = 8;
		
		jp2_source              m_Source;
		kdu_compressed_source * m_Input;
		jp2_family_src          jp2_ultimate_src;
		kdu_codestream          codestream;
		
		m_Input=&m_Source;
		jp2_ultimate_src.open(m_Nomfic.c_str());
		if (!m_Source.open(&jp2_ultimate_src)) return;
		m_Source.read_header();
		codestream.create(m_Input);
		
		
        //int dz = aDeZoom;
		int max_layers = 0;
		int discard_levels = 0;
		while(((1 << discard_levels) & aDeZoom)==0) ++discard_levels;
		//std::cout << "discard_levels : "<<discard_levels<<std::endl;
		int minDwtLevels = codestream.get_min_dwt_levels();
		int reDeZoom = 0;
		if (discard_levels>minDwtLevels)
		{
			reDeZoom = (1<<(discard_levels-minDwtLevels));
			discard_levels=minDwtLevels;
            //dz = (1 << discard_levels);
			//std::cout << "On fait un dz "<<dz<<" puis on fera un ssech "<<reDeZoom<<std::endl;
		}
		
		int * precisions = new int[aVImages.size()];
		for(size_t i=0;i<aVImages.size();++i) precisions[i]=precision;
		kdu_dims dims,mdims;
		dims.pos=kdu_coords(aP0File.real(),aP0File.imag());
		dims.size=kdu_coords(aSz.real()*aDeZoom,aSz.imag()*aDeZoom);
		codestream.map_region(0,dims,mdims);
		int premier_canal = -1;
		int dernier_canal = -1;
		for(int c=0;c<m_Nbc;++c)
		{
			if ((1<<c)&(mFlagLoadedIms))
			{
				if (premier_canal==-1) premier_canal=c;
				dernier_canal = c;
			}
		}
		if (premier_canal==-1)
		{
			premier_canal=0;
			dernier_canal=m_Nbc;
		}
		int nb_canaux = dernier_canal-premier_canal+1;
		if (nb_canaux>(int)aVImages.size()) nb_canaux=aVImages.size();	
		
		codestream.apply_input_restrictions(premier_canal,nb_canaux,discard_levels,max_layers,&mdims,KDU_WANT_OUTPUT_COMPONENTS);	
		
		kdu_thread_env env, *env_ref=NULL;
		int n, num_components = codestream.get_num_components(true);
		kdu_dims *comp_dims = new kdu_dims[num_components];
		for (n=0; n < num_components; n++)
			codestream.get_dims(n,comp_dims[n],true);
		int *stripe_heights = new int[num_components];
		int *sample_gaps = NULL;
		int *row_gaps = NULL;
		
		unsigned char **stripe_bufs = new unsigned char *[num_components];
		
		if (reDeZoom>0)
		{
			for(n=0;n<num_components;++n)
			{
				stripe_bufs[n] = new unsigned char[comp_dims[n].size.x];
			}
		}
		
		int env_dbuf_height = 0;
		//int preferred_min_stripe_height = 8;
		//int absolute_max_stripe_height = 1024;
		kdu_stripe_decompressor decompressor;
		decompressor.start(codestream,false,false,env_ref,NULL,env_dbuf_height);
		for (n=0; n < num_components; n++) stripe_heights[n] = 1; 	
		for(int l=0;l<aSz.imag();++l)
		{
			if (reDeZoom<=0) for(n=0;n<num_components;++n) stripe_bufs[n] = (unsigned char*)&(aVImages[n].mData[aP0Im.imag()+l][aP0Im.real()]);
			decompressor.pull_stripe(stripe_bufs,stripe_heights,sample_gaps,row_gaps,precisions);
			if (reDeZoom>0)
			{ 
				for(n=0;n<num_components;++n)
					for(int c=0;c<aSz.real();++c)
						aVImages[n].mData[aP0Im.imag()+l][c] = stripe_bufs[n][c*reDeZoom];
				for(int ll=1;ll<reDeZoom;++ll) decompressor.pull_stripe(stripe_bufs,stripe_heights,sample_gaps,row_gaps,precisions);
			}
		}
		
		if (reDeZoom) 
		{
			for(n=0;n<num_components;++n) delete[] stripe_bufs[n];
			delete[] sample_gaps;
			delete[] row_gaps;
		}	
		
		// Clean up
		decompressor.finish();
		if (env.exists())
			env.destroy(); 
		codestream.destroy();	
		m_Input->close();
		if (jp2_ultimate_src.exists()) jp2_ultimate_src.close();
		delete[] precisions;
		delete[] stripe_heights;
		delete[] stripe_bufs;
        delete[] comp_dims;
		
	}
	
};

#endif

