#ifndef __OPTIMISATION_H__
#define __OPTIMISATION_H__

#include "GpGpu/GpGpuTools.h"
#include "GpGpu/data2Optimize.h"

//#ifndef BOOST_ALL_NO_LIB
//    #define BOOST_ALL_NO_LIB
//#endif

#include <boost/thread/thread.hpp>

template <class T>
void LaunchKernel();

#define HOST_Data2Opti Data2Optimiz<CuHostData3D>
#define DEVC_Data2Opti Data2Optimiz<CuDeviceData3D>

extern "C" void Launch();
extern "C" void OptimisationOneDirection(DEVC_Data2Opti  &d2O);

/// \class InterfMicMacOptGpGpu
/// \brief Class qui permet a micmac de lancer les calculs d optimisations sur le Gpu
class InterfOptimizGpGpu
{
public:
    InterfOptimizGpGpu();
    ~InterfOptimizGpGpu();


    HOST_Data2Opti& Data2Opt(){ return _H_data2Opt;}

    void            Dealloc();
    void            oneDirOptGpGpu();
    void            ReallocParam(uint size);


    void            SetDirToCompute(bool compute);
    bool            GetDirToCompute();
    void            SetDirToCopy(bool copy);
    bool            GetDirToCopy();
    bool            GetPreCompNextDir();
    void            SetPreCompNextDir(bool precompute);

private:

    void            threadFuncOptimi();

    HOST_Data2Opti  _H_data2Opt;
    DEVC_Data2Opti  _D_data2Opt;
    boost::thread*  _gpGpuThreadOpti;

    boost::mutex    _mutexCompu;
    boost::mutex    _mutexCopy;
    boost::mutex    _mutexPreCompute;

    bool            _compute;
    bool            _copy;
    bool            _precompute;

};


#endif
