// File Automatically generated by eLiSe
#include "StdAfx.h"
#include "cGen2DBundleEgProj_Deg6.h"


cGen2DBundleEgProj_Deg6::cGen2DBundleEgProj_Deg6():
    cElCompiledFonc(2)
{
   AddIntRef (cIncIntervale("CX",3,31));
   AddIntRef (cIncIntervale("CY",31,59));
   AddIntRef (cIncIntervale("Tmp_PTer",0,3));
   Close(false);
}



void cGen2DBundleEgProj_Deg6::ComputeVal()
{
   double tmp0_ = mCompCoord[0];
   double tmp1_ = tmp0_-mLocPTerInit_x;
   double tmp2_ = mCompCoord[1];
   double tmp3_ = tmp2_-mLocPTerInit_y;
   double tmp4_ = mCompCoord[2];
   double tmp5_ = tmp4_-mLocPTerInit_z;
   double tmp6_ = mLocGradX_x*(tmp1_);
   double tmp7_ = mLocProjInit_x+tmp6_;
   double tmp8_ = mLocGradY_x*(tmp3_);
   double tmp9_ = tmp7_+tmp8_;
   double tmp10_ = mLocGradZ_x*(tmp5_);
   double tmp11_ = tmp9_+tmp10_;
   double tmp12_ = mLocGradX_y*(tmp1_);
   double tmp13_ = mLocProjInit_y+tmp12_;
   double tmp14_ = mLocGradY_y*(tmp3_);
   double tmp15_ = tmp13_+tmp14_;
   double tmp16_ = mLocGradZ_y*(tmp5_);
   double tmp17_ = tmp15_+tmp16_;
   double tmp18_ = (tmp17_)-mLocCentr_y;
   double tmp19_ = (tmp18_)/mLocAmpl;
   double tmp20_ = (tmp11_)-mLocCentr_x;
   double tmp21_ = (tmp20_)/mLocAmpl;
   double tmp22_ = ElSquare(tmp19_);
   double tmp23_ = ElSquare(tmp21_);
   double tmp24_ = VCube(tmp19_);
   double tmp25_ = VCube(tmp21_);
   double tmp26_ = VPow4(tmp19_);
   double tmp27_ = VPow4(tmp21_);
   double tmp28_ = VPow5(tmp19_);
   double tmp29_ = VPow5(tmp21_);
   double tmp30_ = VPow6(tmp19_);
   double tmp31_ = VPow6(tmp21_);

  mVal[0] = (tmp11_+mCompCoord[3]+mCompCoord[4]*(tmp19_)+mCompCoord[5]*(tmp21_)+mCompCoord[6]*tmp22_+mCompCoord[7]*(tmp21_)*(tmp19_)+mCompCoord[8]*tmp23_+mCompCoord[9]*tmp24_+mCompCoord[10]*(tmp21_)*tmp22_+mCompCoord[11]*tmp23_*(tmp19_)+mCompCoord[12]*tmp25_+mCompCoord[13]*tmp26_+mCompCoord[14]*(tmp21_)*tmp24_+mCompCoord[15]*tmp23_*tmp22_+mCompCoord[16]*tmp25_*(tmp19_)+mCompCoord[17]*tmp27_+mCompCoord[18]*tmp28_+mCompCoord[19]*(tmp21_)*tmp26_+mCompCoord[20]*tmp23_*tmp24_+mCompCoord[21]*tmp25_*tmp22_+mCompCoord[22]*tmp27_*(tmp19_)+mCompCoord[23]*tmp29_+mCompCoord[24]*tmp30_+mCompCoord[25]*(tmp21_)*tmp28_+mCompCoord[26]*tmp23_*tmp26_+mCompCoord[27]*tmp25_*tmp24_+mCompCoord[28]*tmp27_*tmp22_+mCompCoord[29]*tmp29_*(tmp19_)+mCompCoord[30]*tmp31_)-mLocPIm_x;

  mVal[1] = (tmp17_+mCompCoord[31]+mCompCoord[32]*(tmp19_)+mCompCoord[33]*(tmp21_)+mCompCoord[34]*tmp22_+mCompCoord[35]*(tmp21_)*(tmp19_)+mCompCoord[36]*tmp23_+mCompCoord[37]*tmp24_+mCompCoord[38]*(tmp21_)*tmp22_+mCompCoord[39]*tmp23_*(tmp19_)+mCompCoord[40]*tmp25_+mCompCoord[41]*tmp26_+mCompCoord[42]*(tmp21_)*tmp24_+mCompCoord[43]*tmp23_*tmp22_+mCompCoord[44]*tmp25_*(tmp19_)+mCompCoord[45]*tmp27_+mCompCoord[46]*tmp28_+mCompCoord[47]*(tmp21_)*tmp26_+mCompCoord[48]*tmp23_*tmp24_+mCompCoord[49]*tmp25_*tmp22_+mCompCoord[50]*tmp27_*(tmp19_)+mCompCoord[51]*tmp29_+mCompCoord[52]*tmp30_+mCompCoord[53]*(tmp21_)*tmp28_+mCompCoord[54]*tmp23_*tmp26_+mCompCoord[55]*tmp25_*tmp24_+mCompCoord[56]*tmp27_*tmp22_+mCompCoord[57]*tmp29_*(tmp19_)+mCompCoord[58]*tmp31_)-mLocPIm_y;

}


void cGen2DBundleEgProj_Deg6::ComputeValDeriv()
{
   double tmp0_ = mCompCoord[0];
   double tmp1_ = tmp0_-mLocPTerInit_x;
   double tmp2_ = mCompCoord[1];
   double tmp3_ = tmp2_-mLocPTerInit_y;
   double tmp4_ = mCompCoord[2];
   double tmp5_ = tmp4_-mLocPTerInit_z;
   double tmp6_ = mLocGradX_x*(tmp1_);
   double tmp7_ = mLocProjInit_x+tmp6_;
   double tmp8_ = mLocGradY_x*(tmp3_);
   double tmp9_ = tmp7_+tmp8_;
   double tmp10_ = mLocGradZ_x*(tmp5_);
   double tmp11_ = tmp9_+tmp10_;
   double tmp12_ = mLocGradX_y*(tmp1_);
   double tmp13_ = mLocProjInit_y+tmp12_;
   double tmp14_ = mLocGradY_y*(tmp3_);
   double tmp15_ = tmp13_+tmp14_;
   double tmp16_ = mLocGradZ_y*(tmp5_);
   double tmp17_ = tmp15_+tmp16_;
   double tmp18_ = (tmp17_)-mLocCentr_y;
   double tmp19_ = (tmp18_)/mLocAmpl;
   double tmp20_ = (tmp11_)-mLocCentr_x;
   double tmp21_ = (tmp20_)/mLocAmpl;
   double tmp22_ = ElSquare(tmp19_);
   double tmp23_ = ElSquare(tmp21_);
   double tmp24_ = VCube(tmp19_);
   double tmp25_ = VCube(tmp21_);
   double tmp26_ = VPow4(tmp19_);
   double tmp27_ = VPow4(tmp21_);
   double tmp28_ = VPow5(tmp19_);
   double tmp29_ = VPow5(tmp21_);
   double tmp30_ = mCompCoord[4];
   double tmp31_ = ElSquare(mLocAmpl);
   double tmp32_ = mCompCoord[5];
   double tmp33_ = mLocGradX_y*mLocAmpl;
   double tmp34_ = (tmp33_)/tmp31_;
   double tmp35_ = mCompCoord[6];
   double tmp36_ = mLocGradX_x*mLocAmpl;
   double tmp37_ = (tmp36_)/tmp31_;
   double tmp38_ = mCompCoord[7];
   double tmp39_ = tmp38_*(tmp21_);
   double tmp40_ = mCompCoord[8];
   double tmp41_ = mCompCoord[9];
   double tmp42_ = mCompCoord[10];
   double tmp43_ = 2*(tmp34_);
   double tmp44_ = tmp43_*(tmp19_);
   double tmp45_ = tmp42_*(tmp21_);
   double tmp46_ = 2*(tmp37_);
   double tmp47_ = tmp46_*(tmp21_);
   double tmp48_ = mCompCoord[11];
   double tmp49_ = tmp48_*tmp23_;
   double tmp50_ = mCompCoord[12];
   double tmp51_ = mCompCoord[13];
   double tmp52_ = mCompCoord[14];
   double tmp53_ = 3*(tmp34_);
   double tmp54_ = tmp53_*tmp22_;
   double tmp55_ = tmp52_*(tmp21_);
   double tmp56_ = mCompCoord[15];
   double tmp57_ = tmp56_*tmp23_;
   double tmp58_ = 3*(tmp37_);
   double tmp59_ = tmp58_*tmp23_;
   double tmp60_ = mCompCoord[16];
   double tmp61_ = tmp60_*tmp25_;
   double tmp62_ = mCompCoord[17];
   double tmp63_ = mCompCoord[18];
   double tmp64_ = mCompCoord[19];
   double tmp65_ = 4*(tmp34_);
   double tmp66_ = tmp65_*tmp24_;
   double tmp67_ = tmp64_*(tmp21_);
   double tmp68_ = mCompCoord[20];
   double tmp69_ = tmp68_*tmp23_;
   double tmp70_ = mCompCoord[21];
   double tmp71_ = tmp70_*tmp25_;
   double tmp72_ = 4*(tmp37_);
   double tmp73_ = tmp72_*tmp25_;
   double tmp74_ = mCompCoord[22];
   double tmp75_ = tmp74_*tmp27_;
   double tmp76_ = mCompCoord[23];
   double tmp77_ = mCompCoord[24];
   double tmp78_ = mCompCoord[25];
   double tmp79_ = 5*(tmp34_);
   double tmp80_ = tmp79_*tmp26_;
   double tmp81_ = tmp78_*(tmp21_);
   double tmp82_ = mCompCoord[26];
   double tmp83_ = tmp82_*tmp23_;
   double tmp84_ = mCompCoord[27];
   double tmp85_ = tmp84_*tmp25_;
   double tmp86_ = mCompCoord[28];
   double tmp87_ = tmp86_*tmp27_;
   double tmp88_ = 5*(tmp37_);
   double tmp89_ = tmp88_*tmp27_;
   double tmp90_ = mCompCoord[29];
   double tmp91_ = tmp90_*tmp29_;
   double tmp92_ = mCompCoord[30];
   double tmp93_ = mLocGradY_y*mLocAmpl;
   double tmp94_ = (tmp93_)/tmp31_;
   double tmp95_ = mLocGradY_x*mLocAmpl;
   double tmp96_ = (tmp95_)/tmp31_;
   double tmp97_ = 2*(tmp94_);
   double tmp98_ = tmp97_*(tmp19_);
   double tmp99_ = 2*(tmp96_);
   double tmp100_ = tmp99_*(tmp21_);
   double tmp101_ = 3*(tmp94_);
   double tmp102_ = tmp101_*tmp22_;
   double tmp103_ = 3*(tmp96_);
   double tmp104_ = tmp103_*tmp23_;
   double tmp105_ = 4*(tmp94_);
   double tmp106_ = tmp105_*tmp24_;
   double tmp107_ = 4*(tmp96_);
   double tmp108_ = tmp107_*tmp25_;
   double tmp109_ = 5*(tmp94_);
   double tmp110_ = tmp109_*tmp26_;
   double tmp111_ = 5*(tmp96_);
   double tmp112_ = tmp111_*tmp27_;
   double tmp113_ = mLocGradZ_y*mLocAmpl;
   double tmp114_ = (tmp113_)/tmp31_;
   double tmp115_ = mLocGradZ_x*mLocAmpl;
   double tmp116_ = (tmp115_)/tmp31_;
   double tmp117_ = 2*(tmp114_);
   double tmp118_ = tmp117_*(tmp19_);
   double tmp119_ = 2*(tmp116_);
   double tmp120_ = tmp119_*(tmp21_);
   double tmp121_ = 3*(tmp114_);
   double tmp122_ = tmp121_*tmp22_;
   double tmp123_ = 3*(tmp116_);
   double tmp124_ = tmp123_*tmp23_;
   double tmp125_ = 4*(tmp114_);
   double tmp126_ = tmp125_*tmp24_;
   double tmp127_ = 4*(tmp116_);
   double tmp128_ = tmp127_*tmp25_;
   double tmp129_ = 5*(tmp114_);
   double tmp130_ = tmp129_*tmp26_;
   double tmp131_ = 5*(tmp116_);
   double tmp132_ = tmp131_*tmp27_;
   double tmp133_ = VPow6(tmp19_);
   double tmp134_ = VPow6(tmp21_);
   double tmp135_ = mCompCoord[32];
   double tmp136_ = mCompCoord[33];
   double tmp137_ = mCompCoord[34];
   double tmp138_ = mCompCoord[35];
   double tmp139_ = tmp138_*(tmp21_);
   double tmp140_ = mCompCoord[36];
   double tmp141_ = mCompCoord[37];
   double tmp142_ = mCompCoord[38];
   double tmp143_ = tmp142_*(tmp21_);
   double tmp144_ = mCompCoord[39];
   double tmp145_ = tmp144_*tmp23_;
   double tmp146_ = mCompCoord[40];
   double tmp147_ = mCompCoord[41];
   double tmp148_ = mCompCoord[42];
   double tmp149_ = tmp148_*(tmp21_);
   double tmp150_ = mCompCoord[43];
   double tmp151_ = tmp150_*tmp23_;
   double tmp152_ = mCompCoord[44];
   double tmp153_ = tmp152_*tmp25_;
   double tmp154_ = mCompCoord[45];
   double tmp155_ = mCompCoord[46];
   double tmp156_ = mCompCoord[47];
   double tmp157_ = tmp156_*(tmp21_);
   double tmp158_ = mCompCoord[48];
   double tmp159_ = tmp158_*tmp23_;
   double tmp160_ = mCompCoord[49];
   double tmp161_ = tmp160_*tmp25_;
   double tmp162_ = mCompCoord[50];
   double tmp163_ = tmp162_*tmp27_;
   double tmp164_ = mCompCoord[51];
   double tmp165_ = 6*(tmp34_);
   double tmp166_ = tmp165_*tmp28_;
   double tmp167_ = mCompCoord[52];
   double tmp168_ = mCompCoord[53];
   double tmp169_ = tmp168_*(tmp21_);
   double tmp170_ = mCompCoord[54];
   double tmp171_ = tmp170_*tmp23_;
   double tmp172_ = mCompCoord[55];
   double tmp173_ = tmp172_*tmp25_;
   double tmp174_ = mCompCoord[56];
   double tmp175_ = tmp174_*tmp27_;
   double tmp176_ = mCompCoord[57];
   double tmp177_ = tmp176_*tmp29_;
   double tmp178_ = 6*(tmp37_);
   double tmp179_ = tmp178_*tmp29_;
   double tmp180_ = mCompCoord[58];
   double tmp181_ = 6*(tmp94_);
   double tmp182_ = tmp181_*tmp28_;
   double tmp183_ = 6*(tmp96_);
   double tmp184_ = tmp183_*tmp29_;
   double tmp185_ = 6*(tmp114_);
   double tmp186_ = tmp185_*tmp28_;
   double tmp187_ = 6*(tmp116_);
   double tmp188_ = tmp187_*tmp29_;
   double tmp189_ = (tmp21_)*(tmp19_);
   double tmp190_ = (tmp21_)*tmp22_;
   double tmp191_ = tmp23_*(tmp19_);
   double tmp192_ = (tmp21_)*tmp24_;
   double tmp193_ = tmp23_*tmp22_;
   double tmp194_ = tmp25_*(tmp19_);
   double tmp195_ = (tmp21_)*tmp26_;
   double tmp196_ = tmp23_*tmp24_;
   double tmp197_ = tmp25_*tmp22_;
   double tmp198_ = tmp27_*(tmp19_);
   double tmp199_ = (tmp21_)*tmp28_;
   double tmp200_ = tmp23_*tmp26_;
   double tmp201_ = tmp25_*tmp24_;
   double tmp202_ = tmp27_*tmp22_;
   double tmp203_ = tmp29_*(tmp19_);

  mVal[0] = (tmp11_+mCompCoord[3]+tmp30_*(tmp19_)+tmp32_*(tmp21_)+tmp35_*tmp22_+tmp39_*(tmp19_)+tmp40_*tmp23_+tmp41_*tmp24_+tmp45_*tmp22_+tmp49_*(tmp19_)+tmp50_*tmp25_+tmp51_*tmp26_+tmp55_*tmp24_+tmp57_*tmp22_+tmp61_*(tmp19_)+tmp62_*tmp27_+tmp63_*tmp28_+tmp67_*tmp26_+tmp69_*tmp24_+tmp71_*tmp22_+tmp75_*(tmp19_)+tmp76_*tmp29_+tmp77_*tmp133_+tmp81_*tmp28_+tmp83_*tmp26_+tmp85_*tmp24_+tmp87_*tmp22_+tmp91_*(tmp19_)+tmp92_*tmp134_)-mLocPIm_x;

  mCompDer[0][0] = mLocGradX_x+(tmp34_)*tmp30_+(tmp37_)*tmp32_+tmp44_*tmp35_+(tmp37_)*tmp38_*(tmp19_)+(tmp34_)*tmp39_+tmp47_*tmp40_+tmp54_*tmp41_+(tmp37_)*tmp42_*tmp22_+tmp44_*tmp45_+tmp47_*tmp48_*(tmp19_)+(tmp34_)*tmp49_+tmp59_*tmp50_+tmp66_*tmp51_+(tmp37_)*tmp52_*tmp24_+tmp54_*tmp55_+tmp47_*tmp56_*tmp22_+tmp44_*tmp57_+tmp59_*tmp60_*(tmp19_)+(tmp34_)*tmp61_+tmp73_*tmp62_+tmp80_*tmp63_+(tmp37_)*tmp64_*tmp26_+tmp66_*tmp67_+tmp47_*tmp68_*tmp24_+tmp54_*tmp69_+tmp59_*tmp70_*tmp22_+tmp44_*tmp71_+tmp73_*tmp74_*(tmp19_)+(tmp34_)*tmp75_+tmp89_*tmp76_+tmp166_*tmp77_+(tmp37_)*tmp78_*tmp28_+tmp80_*tmp81_+tmp47_*tmp82_*tmp26_+tmp66_*tmp83_+tmp59_*tmp84_*tmp24_+tmp54_*tmp85_+tmp73_*tmp86_*tmp22_+tmp44_*tmp87_+tmp89_*tmp90_*(tmp19_)+(tmp34_)*tmp91_+tmp179_*tmp92_;
  mCompDer[0][1] = mLocGradY_x+(tmp94_)*tmp30_+(tmp96_)*tmp32_+tmp98_*tmp35_+(tmp96_)*tmp38_*(tmp19_)+(tmp94_)*tmp39_+tmp100_*tmp40_+tmp102_*tmp41_+(tmp96_)*tmp42_*tmp22_+tmp98_*tmp45_+tmp100_*tmp48_*(tmp19_)+(tmp94_)*tmp49_+tmp104_*tmp50_+tmp106_*tmp51_+(tmp96_)*tmp52_*tmp24_+tmp102_*tmp55_+tmp100_*tmp56_*tmp22_+tmp98_*tmp57_+tmp104_*tmp60_*(tmp19_)+(tmp94_)*tmp61_+tmp108_*tmp62_+tmp110_*tmp63_+(tmp96_)*tmp64_*tmp26_+tmp106_*tmp67_+tmp100_*tmp68_*tmp24_+tmp102_*tmp69_+tmp104_*tmp70_*tmp22_+tmp98_*tmp71_+tmp108_*tmp74_*(tmp19_)+(tmp94_)*tmp75_+tmp112_*tmp76_+tmp182_*tmp77_+(tmp96_)*tmp78_*tmp28_+tmp110_*tmp81_+tmp100_*tmp82_*tmp26_+tmp106_*tmp83_+tmp104_*tmp84_*tmp24_+tmp102_*tmp85_+tmp108_*tmp86_*tmp22_+tmp98_*tmp87_+tmp112_*tmp90_*(tmp19_)+(tmp94_)*tmp91_+tmp184_*tmp92_;
  mCompDer[0][2] = mLocGradZ_x+(tmp114_)*tmp30_+(tmp116_)*tmp32_+tmp118_*tmp35_+(tmp116_)*tmp38_*(tmp19_)+(tmp114_)*tmp39_+tmp120_*tmp40_+tmp122_*tmp41_+(tmp116_)*tmp42_*tmp22_+tmp118_*tmp45_+tmp120_*tmp48_*(tmp19_)+(tmp114_)*tmp49_+tmp124_*tmp50_+tmp126_*tmp51_+(tmp116_)*tmp52_*tmp24_+tmp122_*tmp55_+tmp120_*tmp56_*tmp22_+tmp118_*tmp57_+tmp124_*tmp60_*(tmp19_)+(tmp114_)*tmp61_+tmp128_*tmp62_+tmp130_*tmp63_+(tmp116_)*tmp64_*tmp26_+tmp126_*tmp67_+tmp120_*tmp68_*tmp24_+tmp122_*tmp69_+tmp124_*tmp70_*tmp22_+tmp118_*tmp71_+tmp128_*tmp74_*(tmp19_)+(tmp114_)*tmp75_+tmp132_*tmp76_+tmp186_*tmp77_+(tmp116_)*tmp78_*tmp28_+tmp130_*tmp81_+tmp120_*tmp82_*tmp26_+tmp126_*tmp83_+tmp124_*tmp84_*tmp24_+tmp122_*tmp85_+tmp128_*tmp86_*tmp22_+tmp118_*tmp87_+tmp132_*tmp90_*(tmp19_)+(tmp114_)*tmp91_+tmp188_*tmp92_;
  mCompDer[0][3] = 1;
  mCompDer[0][4] = tmp19_;
  mCompDer[0][5] = tmp21_;
  mCompDer[0][6] = tmp22_;
  mCompDer[0][7] = tmp189_;
  mCompDer[0][8] = tmp23_;
  mCompDer[0][9] = tmp24_;
  mCompDer[0][10] = tmp190_;
  mCompDer[0][11] = tmp191_;
  mCompDer[0][12] = tmp25_;
  mCompDer[0][13] = tmp26_;
  mCompDer[0][14] = tmp192_;
  mCompDer[0][15] = tmp193_;
  mCompDer[0][16] = tmp194_;
  mCompDer[0][17] = tmp27_;
  mCompDer[0][18] = tmp28_;
  mCompDer[0][19] = tmp195_;
  mCompDer[0][20] = tmp196_;
  mCompDer[0][21] = tmp197_;
  mCompDer[0][22] = tmp198_;
  mCompDer[0][23] = tmp29_;
  mCompDer[0][24] = tmp133_;
  mCompDer[0][25] = tmp199_;
  mCompDer[0][26] = tmp200_;
  mCompDer[0][27] = tmp201_;
  mCompDer[0][28] = tmp202_;
  mCompDer[0][29] = tmp203_;
  mCompDer[0][30] = tmp134_;
  mCompDer[0][31] = 0;
  mCompDer[0][32] = 0;
  mCompDer[0][33] = 0;
  mCompDer[0][34] = 0;
  mCompDer[0][35] = 0;
  mCompDer[0][36] = 0;
  mCompDer[0][37] = 0;
  mCompDer[0][38] = 0;
  mCompDer[0][39] = 0;
  mCompDer[0][40] = 0;
  mCompDer[0][41] = 0;
  mCompDer[0][42] = 0;
  mCompDer[0][43] = 0;
  mCompDer[0][44] = 0;
  mCompDer[0][45] = 0;
  mCompDer[0][46] = 0;
  mCompDer[0][47] = 0;
  mCompDer[0][48] = 0;
  mCompDer[0][49] = 0;
  mCompDer[0][50] = 0;
  mCompDer[0][51] = 0;
  mCompDer[0][52] = 0;
  mCompDer[0][53] = 0;
  mCompDer[0][54] = 0;
  mCompDer[0][55] = 0;
  mCompDer[0][56] = 0;
  mCompDer[0][57] = 0;
  mCompDer[0][58] = 0;
  mVal[1] = (tmp17_+mCompCoord[31]+tmp135_*(tmp19_)+tmp136_*(tmp21_)+tmp137_*tmp22_+tmp139_*(tmp19_)+tmp140_*tmp23_+tmp141_*tmp24_+tmp143_*tmp22_+tmp145_*(tmp19_)+tmp146_*tmp25_+tmp147_*tmp26_+tmp149_*tmp24_+tmp151_*tmp22_+tmp153_*(tmp19_)+tmp154_*tmp27_+tmp155_*tmp28_+tmp157_*tmp26_+tmp159_*tmp24_+tmp161_*tmp22_+tmp163_*(tmp19_)+tmp164_*tmp29_+tmp167_*tmp133_+tmp169_*tmp28_+tmp171_*tmp26_+tmp173_*tmp24_+tmp175_*tmp22_+tmp177_*(tmp19_)+tmp180_*tmp134_)-mLocPIm_y;

  mCompDer[1][0] = mLocGradX_y+(tmp34_)*tmp135_+(tmp37_)*tmp136_+tmp44_*tmp137_+(tmp37_)*tmp138_*(tmp19_)+(tmp34_)*tmp139_+tmp47_*tmp140_+tmp54_*tmp141_+(tmp37_)*tmp142_*tmp22_+tmp44_*tmp143_+tmp47_*tmp144_*(tmp19_)+(tmp34_)*tmp145_+tmp59_*tmp146_+tmp66_*tmp147_+(tmp37_)*tmp148_*tmp24_+tmp54_*tmp149_+tmp47_*tmp150_*tmp22_+tmp44_*tmp151_+tmp59_*tmp152_*(tmp19_)+(tmp34_)*tmp153_+tmp73_*tmp154_+tmp80_*tmp155_+(tmp37_)*tmp156_*tmp26_+tmp66_*tmp157_+tmp47_*tmp158_*tmp24_+tmp54_*tmp159_+tmp59_*tmp160_*tmp22_+tmp44_*tmp161_+tmp73_*tmp162_*(tmp19_)+(tmp34_)*tmp163_+tmp89_*tmp164_+tmp166_*tmp167_+(tmp37_)*tmp168_*tmp28_+tmp80_*tmp169_+tmp47_*tmp170_*tmp26_+tmp66_*tmp171_+tmp59_*tmp172_*tmp24_+tmp54_*tmp173_+tmp73_*tmp174_*tmp22_+tmp44_*tmp175_+tmp89_*tmp176_*(tmp19_)+(tmp34_)*tmp177_+tmp179_*tmp180_;
  mCompDer[1][1] = mLocGradY_y+(tmp94_)*tmp135_+(tmp96_)*tmp136_+tmp98_*tmp137_+(tmp96_)*tmp138_*(tmp19_)+(tmp94_)*tmp139_+tmp100_*tmp140_+tmp102_*tmp141_+(tmp96_)*tmp142_*tmp22_+tmp98_*tmp143_+tmp100_*tmp144_*(tmp19_)+(tmp94_)*tmp145_+tmp104_*tmp146_+tmp106_*tmp147_+(tmp96_)*tmp148_*tmp24_+tmp102_*tmp149_+tmp100_*tmp150_*tmp22_+tmp98_*tmp151_+tmp104_*tmp152_*(tmp19_)+(tmp94_)*tmp153_+tmp108_*tmp154_+tmp110_*tmp155_+(tmp96_)*tmp156_*tmp26_+tmp106_*tmp157_+tmp100_*tmp158_*tmp24_+tmp102_*tmp159_+tmp104_*tmp160_*tmp22_+tmp98_*tmp161_+tmp108_*tmp162_*(tmp19_)+(tmp94_)*tmp163_+tmp112_*tmp164_+tmp182_*tmp167_+(tmp96_)*tmp168_*tmp28_+tmp110_*tmp169_+tmp100_*tmp170_*tmp26_+tmp106_*tmp171_+tmp104_*tmp172_*tmp24_+tmp102_*tmp173_+tmp108_*tmp174_*tmp22_+tmp98_*tmp175_+tmp112_*tmp176_*(tmp19_)+(tmp94_)*tmp177_+tmp184_*tmp180_;
  mCompDer[1][2] = mLocGradZ_y+(tmp114_)*tmp135_+(tmp116_)*tmp136_+tmp118_*tmp137_+(tmp116_)*tmp138_*(tmp19_)+(tmp114_)*tmp139_+tmp120_*tmp140_+tmp122_*tmp141_+(tmp116_)*tmp142_*tmp22_+tmp118_*tmp143_+tmp120_*tmp144_*(tmp19_)+(tmp114_)*tmp145_+tmp124_*tmp146_+tmp126_*tmp147_+(tmp116_)*tmp148_*tmp24_+tmp122_*tmp149_+tmp120_*tmp150_*tmp22_+tmp118_*tmp151_+tmp124_*tmp152_*(tmp19_)+(tmp114_)*tmp153_+tmp128_*tmp154_+tmp130_*tmp155_+(tmp116_)*tmp156_*tmp26_+tmp126_*tmp157_+tmp120_*tmp158_*tmp24_+tmp122_*tmp159_+tmp124_*tmp160_*tmp22_+tmp118_*tmp161_+tmp128_*tmp162_*(tmp19_)+(tmp114_)*tmp163_+tmp132_*tmp164_+tmp186_*tmp167_+(tmp116_)*tmp168_*tmp28_+tmp130_*tmp169_+tmp120_*tmp170_*tmp26_+tmp126_*tmp171_+tmp124_*tmp172_*tmp24_+tmp122_*tmp173_+tmp128_*tmp174_*tmp22_+tmp118_*tmp175_+tmp132_*tmp176_*(tmp19_)+(tmp114_)*tmp177_+tmp188_*tmp180_;
  mCompDer[1][3] = 0;
  mCompDer[1][4] = 0;
  mCompDer[1][5] = 0;
  mCompDer[1][6] = 0;
  mCompDer[1][7] = 0;
  mCompDer[1][8] = 0;
  mCompDer[1][9] = 0;
  mCompDer[1][10] = 0;
  mCompDer[1][11] = 0;
  mCompDer[1][12] = 0;
  mCompDer[1][13] = 0;
  mCompDer[1][14] = 0;
  mCompDer[1][15] = 0;
  mCompDer[1][16] = 0;
  mCompDer[1][17] = 0;
  mCompDer[1][18] = 0;
  mCompDer[1][19] = 0;
  mCompDer[1][20] = 0;
  mCompDer[1][21] = 0;
  mCompDer[1][22] = 0;
  mCompDer[1][23] = 0;
  mCompDer[1][24] = 0;
  mCompDer[1][25] = 0;
  mCompDer[1][26] = 0;
  mCompDer[1][27] = 0;
  mCompDer[1][28] = 0;
  mCompDer[1][29] = 0;
  mCompDer[1][30] = 0;
  mCompDer[1][31] = 1;
  mCompDer[1][32] = tmp19_;
  mCompDer[1][33] = tmp21_;
  mCompDer[1][34] = tmp22_;
  mCompDer[1][35] = tmp189_;
  mCompDer[1][36] = tmp23_;
  mCompDer[1][37] = tmp24_;
  mCompDer[1][38] = tmp190_;
  mCompDer[1][39] = tmp191_;
  mCompDer[1][40] = tmp25_;
  mCompDer[1][41] = tmp26_;
  mCompDer[1][42] = tmp192_;
  mCompDer[1][43] = tmp193_;
  mCompDer[1][44] = tmp194_;
  mCompDer[1][45] = tmp27_;
  mCompDer[1][46] = tmp28_;
  mCompDer[1][47] = tmp195_;
  mCompDer[1][48] = tmp196_;
  mCompDer[1][49] = tmp197_;
  mCompDer[1][50] = tmp198_;
  mCompDer[1][51] = tmp29_;
  mCompDer[1][52] = tmp133_;
  mCompDer[1][53] = tmp199_;
  mCompDer[1][54] = tmp200_;
  mCompDer[1][55] = tmp201_;
  mCompDer[1][56] = tmp202_;
  mCompDer[1][57] = tmp203_;
  mCompDer[1][58] = tmp134_;
}


void cGen2DBundleEgProj_Deg6::ComputeValDerivHessian()
{
  ELISE_ASSERT(false,"Foncteur cGen2DBundleEgProj_Deg6 Has no Der Sec");
}

void cGen2DBundleEgProj_Deg6::SetAmpl(double aVal){ mLocAmpl = aVal;}
void cGen2DBundleEgProj_Deg6::SetCentr_x(double aVal){ mLocCentr_x = aVal;}
void cGen2DBundleEgProj_Deg6::SetCentr_y(double aVal){ mLocCentr_y = aVal;}
void cGen2DBundleEgProj_Deg6::SetGradX_x(double aVal){ mLocGradX_x = aVal;}
void cGen2DBundleEgProj_Deg6::SetGradX_y(double aVal){ mLocGradX_y = aVal;}
void cGen2DBundleEgProj_Deg6::SetGradY_x(double aVal){ mLocGradY_x = aVal;}
void cGen2DBundleEgProj_Deg6::SetGradY_y(double aVal){ mLocGradY_y = aVal;}
void cGen2DBundleEgProj_Deg6::SetGradZ_x(double aVal){ mLocGradZ_x = aVal;}
void cGen2DBundleEgProj_Deg6::SetGradZ_y(double aVal){ mLocGradZ_y = aVal;}
void cGen2DBundleEgProj_Deg6::SetPIm_x(double aVal){ mLocPIm_x = aVal;}
void cGen2DBundleEgProj_Deg6::SetPIm_y(double aVal){ mLocPIm_y = aVal;}
void cGen2DBundleEgProj_Deg6::SetPTerInit_x(double aVal){ mLocPTerInit_x = aVal;}
void cGen2DBundleEgProj_Deg6::SetPTerInit_y(double aVal){ mLocPTerInit_y = aVal;}
void cGen2DBundleEgProj_Deg6::SetPTerInit_z(double aVal){ mLocPTerInit_z = aVal;}
void cGen2DBundleEgProj_Deg6::SetProjInit_x(double aVal){ mLocProjInit_x = aVal;}
void cGen2DBundleEgProj_Deg6::SetProjInit_y(double aVal){ mLocProjInit_y = aVal;}



double * cGen2DBundleEgProj_Deg6::AdrVarLocFromString(const std::string & aName)
{
   if (aName == "Ampl") return & mLocAmpl;
   if (aName == "Centr_x") return & mLocCentr_x;
   if (aName == "Centr_y") return & mLocCentr_y;
   if (aName == "GradX_x") return & mLocGradX_x;
   if (aName == "GradX_y") return & mLocGradX_y;
   if (aName == "GradY_x") return & mLocGradY_x;
   if (aName == "GradY_y") return & mLocGradY_y;
   if (aName == "GradZ_x") return & mLocGradZ_x;
   if (aName == "GradZ_y") return & mLocGradZ_y;
   if (aName == "PIm_x") return & mLocPIm_x;
   if (aName == "PIm_y") return & mLocPIm_y;
   if (aName == "PTerInit_x") return & mLocPTerInit_x;
   if (aName == "PTerInit_y") return & mLocPTerInit_y;
   if (aName == "PTerInit_z") return & mLocPTerInit_z;
   if (aName == "ProjInit_x") return & mLocProjInit_x;
   if (aName == "ProjInit_y") return & mLocProjInit_y;
   return 0;
}


cElCompiledFonc::cAutoAddEntry cGen2DBundleEgProj_Deg6::mTheAuto("cGen2DBundleEgProj_Deg6",cGen2DBundleEgProj_Deg6::Alloc);


cElCompiledFonc *  cGen2DBundleEgProj_Deg6::Alloc()
{  return new cGen2DBundleEgProj_Deg6();
}


