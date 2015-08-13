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
   double tmp2_ = mLocGradX_x*(tmp1_);
   double tmp3_ = mLocProjInit_x+tmp2_;
   double tmp4_ = mCompCoord[1];
   double tmp5_ = tmp4_-mLocPTerInit_y;
   double tmp6_ = mLocGradY_x*(tmp5_);
   double tmp7_ = tmp3_+tmp6_;
   double tmp8_ = mCompCoord[2];
   double tmp9_ = tmp8_-mLocPTerInit_z;
   double tmp10_ = mLocGradZ_x*(tmp9_);
   double tmp11_ = tmp7_+tmp10_;
   double tmp12_ = (tmp11_)-mLocCentr_x;
   double tmp13_ = (tmp12_)/mLocAmpl;
   double tmp14_ = mLocGradX_y*(tmp1_);
   double tmp15_ = mLocProjInit_y+tmp14_;
   double tmp16_ = mLocGradY_y*(tmp5_);
   double tmp17_ = tmp15_+tmp16_;
   double tmp18_ = mLocGradZ_y*(tmp9_);
   double tmp19_ = tmp17_+tmp18_;
   double tmp20_ = (tmp19_)-mLocCentr_y;
   double tmp21_ = (tmp20_)/mLocAmpl;
   double tmp22_ = ElSquare(tmp13_);
   double tmp23_ = ElSquare(tmp21_);
   double tmp24_ = VCube(tmp13_);
   double tmp25_ = VCube(tmp21_);
   double tmp26_ = VPow4(tmp13_);
   double tmp27_ = VPow4(tmp21_);
   double tmp28_ = VPow5(tmp13_);
   double tmp29_ = VPow5(tmp21_);
   double tmp30_ = VPow6(tmp13_);
   double tmp31_ = VPow6(tmp21_);

  mVal[0] = (tmp11_+mCompCoord[3]+mCompCoord[4]+mCompCoord[5]*(tmp13_)*(tmp21_)+mCompCoord[6]+mCompCoord[7]*(tmp13_)*(tmp21_)+mCompCoord[8]*tmp22_*tmp23_+mCompCoord[9]+mCompCoord[10]*(tmp13_)*(tmp21_)+mCompCoord[11]*tmp22_*tmp23_+mCompCoord[12]*tmp24_*tmp25_+mCompCoord[13]+mCompCoord[14]*(tmp13_)*(tmp21_)+mCompCoord[15]*tmp22_*tmp23_+mCompCoord[16]*tmp24_*tmp25_+mCompCoord[17]*tmp26_*tmp27_+mCompCoord[18]+mCompCoord[19]*(tmp13_)*(tmp21_)+mCompCoord[20]*tmp22_*tmp23_+mCompCoord[21]*tmp24_*tmp25_+mCompCoord[22]*tmp26_*tmp27_+mCompCoord[23]*tmp28_*tmp29_+mCompCoord[24]+mCompCoord[25]*(tmp13_)*(tmp21_)+mCompCoord[26]*tmp22_*tmp23_+mCompCoord[27]*tmp24_*tmp25_+mCompCoord[28]*tmp26_*tmp27_+mCompCoord[29]*tmp28_*tmp29_+mCompCoord[30]*tmp30_*tmp31_)-mLocPIm_x;

  mVal[1] = (tmp19_+mCompCoord[31]+mCompCoord[32]+mCompCoord[33]*(tmp13_)*(tmp21_)+mCompCoord[34]+mCompCoord[35]*(tmp13_)*(tmp21_)+mCompCoord[36]*tmp22_*tmp23_+mCompCoord[37]+mCompCoord[38]*(tmp13_)*(tmp21_)+mCompCoord[39]*tmp22_*tmp23_+mCompCoord[40]*tmp24_*tmp25_+mCompCoord[41]+mCompCoord[42]*(tmp13_)*(tmp21_)+mCompCoord[43]*tmp22_*tmp23_+mCompCoord[44]*tmp24_*tmp25_+mCompCoord[45]*tmp26_*tmp27_+mCompCoord[46]+mCompCoord[47]*(tmp13_)*(tmp21_)+mCompCoord[48]*tmp22_*tmp23_+mCompCoord[49]*tmp24_*tmp25_+mCompCoord[50]*tmp26_*tmp27_+mCompCoord[51]*tmp28_*tmp29_+mCompCoord[52]+mCompCoord[53]*(tmp13_)*(tmp21_)+mCompCoord[54]*tmp22_*tmp23_+mCompCoord[55]*tmp24_*tmp25_+mCompCoord[56]*tmp26_*tmp27_+mCompCoord[57]*tmp28_*tmp29_+mCompCoord[58]*tmp30_*tmp31_)-mLocPIm_y;

}


void cGen2DBundleEgProj_Deg6::ComputeValDeriv()
{
   double tmp0_ = mCompCoord[0];
   double tmp1_ = tmp0_-mLocPTerInit_x;
   double tmp2_ = mLocGradX_x*(tmp1_);
   double tmp3_ = mLocProjInit_x+tmp2_;
   double tmp4_ = mCompCoord[1];
   double tmp5_ = tmp4_-mLocPTerInit_y;
   double tmp6_ = mLocGradY_x*(tmp5_);
   double tmp7_ = tmp3_+tmp6_;
   double tmp8_ = mCompCoord[2];
   double tmp9_ = tmp8_-mLocPTerInit_z;
   double tmp10_ = mLocGradZ_x*(tmp9_);
   double tmp11_ = tmp7_+tmp10_;
   double tmp12_ = (tmp11_)-mLocCentr_x;
   double tmp13_ = (tmp12_)/mLocAmpl;
   double tmp14_ = mLocGradX_y*(tmp1_);
   double tmp15_ = mLocProjInit_y+tmp14_;
   double tmp16_ = mLocGradY_y*(tmp5_);
   double tmp17_ = tmp15_+tmp16_;
   double tmp18_ = mLocGradZ_y*(tmp9_);
   double tmp19_ = tmp17_+tmp18_;
   double tmp20_ = (tmp19_)-mLocCentr_y;
   double tmp21_ = (tmp20_)/mLocAmpl;
   double tmp22_ = ElSquare(tmp13_);
   double tmp23_ = ElSquare(tmp21_);
   double tmp24_ = VCube(tmp13_);
   double tmp25_ = VCube(tmp21_);
   double tmp26_ = VPow4(tmp13_);
   double tmp27_ = VPow4(tmp21_);
   double tmp28_ = VPow5(tmp13_);
   double tmp29_ = VPow5(tmp21_);
   double tmp30_ = mCompCoord[5];
   double tmp31_ = ElSquare(mLocAmpl);
   double tmp32_ = tmp30_*(tmp13_);
   double tmp33_ = mLocGradX_x*mLocAmpl;
   double tmp34_ = (tmp33_)/tmp31_;
   double tmp35_ = mCompCoord[7];
   double tmp36_ = mLocGradX_y*mLocAmpl;
   double tmp37_ = (tmp36_)/tmp31_;
   double tmp38_ = tmp35_*(tmp13_);
   double tmp39_ = mCompCoord[8];
   double tmp40_ = tmp39_*tmp22_;
   double tmp41_ = mCompCoord[10];
   double tmp42_ = tmp41_*(tmp13_);
   double tmp43_ = 2*(tmp34_);
   double tmp44_ = tmp43_*(tmp13_);
   double tmp45_ = mCompCoord[11];
   double tmp46_ = 2*(tmp37_);
   double tmp47_ = tmp46_*(tmp21_);
   double tmp48_ = tmp45_*tmp22_;
   double tmp49_ = mCompCoord[12];
   double tmp50_ = tmp49_*tmp24_;
   double tmp51_ = mCompCoord[14];
   double tmp52_ = tmp51_*(tmp13_);
   double tmp53_ = mCompCoord[15];
   double tmp54_ = tmp53_*tmp22_;
   double tmp55_ = 3*(tmp34_);
   double tmp56_ = tmp55_*tmp22_;
   double tmp57_ = mCompCoord[16];
   double tmp58_ = 3*(tmp37_);
   double tmp59_ = tmp58_*tmp23_;
   double tmp60_ = tmp57_*tmp24_;
   double tmp61_ = mCompCoord[17];
   double tmp62_ = tmp61_*tmp26_;
   double tmp63_ = mCompCoord[19];
   double tmp64_ = tmp63_*(tmp13_);
   double tmp65_ = mCompCoord[20];
   double tmp66_ = tmp65_*tmp22_;
   double tmp67_ = mCompCoord[21];
   double tmp68_ = tmp67_*tmp24_;
   double tmp69_ = 4*(tmp34_);
   double tmp70_ = tmp69_*tmp24_;
   double tmp71_ = mCompCoord[22];
   double tmp72_ = 4*(tmp37_);
   double tmp73_ = tmp72_*tmp25_;
   double tmp74_ = tmp71_*tmp26_;
   double tmp75_ = mCompCoord[23];
   double tmp76_ = tmp75_*tmp28_;
   double tmp77_ = mCompCoord[25];
   double tmp78_ = tmp77_*(tmp13_);
   double tmp79_ = mCompCoord[26];
   double tmp80_ = tmp79_*tmp22_;
   double tmp81_ = mCompCoord[27];
   double tmp82_ = tmp81_*tmp24_;
   double tmp83_ = mCompCoord[28];
   double tmp84_ = tmp83_*tmp26_;
   double tmp85_ = 5*(tmp34_);
   double tmp86_ = tmp85_*tmp26_;
   double tmp87_ = mCompCoord[29];
   double tmp88_ = 5*(tmp37_);
   double tmp89_ = tmp88_*tmp27_;
   double tmp90_ = tmp87_*tmp28_;
   double tmp91_ = mCompCoord[30];
   double tmp92_ = VPow6(tmp21_);
   double tmp93_ = VPow6(tmp13_);
   double tmp94_ = tmp91_*tmp93_;
   double tmp95_ = mLocGradY_x*mLocAmpl;
   double tmp96_ = (tmp95_)/tmp31_;
   double tmp97_ = mLocGradY_y*mLocAmpl;
   double tmp98_ = (tmp97_)/tmp31_;
   double tmp99_ = 2*(tmp96_);
   double tmp100_ = tmp99_*(tmp13_);
   double tmp101_ = 2*(tmp98_);
   double tmp102_ = tmp101_*(tmp21_);
   double tmp103_ = 3*(tmp96_);
   double tmp104_ = tmp103_*tmp22_;
   double tmp105_ = 3*(tmp98_);
   double tmp106_ = tmp105_*tmp23_;
   double tmp107_ = 4*(tmp96_);
   double tmp108_ = tmp107_*tmp24_;
   double tmp109_ = 4*(tmp98_);
   double tmp110_ = tmp109_*tmp25_;
   double tmp111_ = 5*(tmp96_);
   double tmp112_ = tmp111_*tmp26_;
   double tmp113_ = 5*(tmp98_);
   double tmp114_ = tmp113_*tmp27_;
   double tmp115_ = mLocGradZ_x*mLocAmpl;
   double tmp116_ = (tmp115_)/tmp31_;
   double tmp117_ = mLocGradZ_y*mLocAmpl;
   double tmp118_ = (tmp117_)/tmp31_;
   double tmp119_ = 2*(tmp116_);
   double tmp120_ = tmp119_*(tmp13_);
   double tmp121_ = 2*(tmp118_);
   double tmp122_ = tmp121_*(tmp21_);
   double tmp123_ = 3*(tmp116_);
   double tmp124_ = tmp123_*tmp22_;
   double tmp125_ = 3*(tmp118_);
   double tmp126_ = tmp125_*tmp23_;
   double tmp127_ = 4*(tmp116_);
   double tmp128_ = tmp127_*tmp24_;
   double tmp129_ = 4*(tmp118_);
   double tmp130_ = tmp129_*tmp25_;
   double tmp131_ = 5*(tmp116_);
   double tmp132_ = tmp131_*tmp26_;
   double tmp133_ = 5*(tmp118_);
   double tmp134_ = tmp133_*tmp27_;
   double tmp135_ = (tmp13_)*(tmp21_);
   double tmp136_ = tmp22_*tmp23_;
   double tmp137_ = tmp24_*tmp25_;
   double tmp138_ = tmp26_*tmp27_;
   double tmp139_ = tmp28_*tmp29_;
   double tmp140_ = mCompCoord[33];
   double tmp141_ = tmp140_*(tmp13_);
   double tmp142_ = mCompCoord[35];
   double tmp143_ = tmp142_*(tmp13_);
   double tmp144_ = mCompCoord[36];
   double tmp145_ = tmp144_*tmp22_;
   double tmp146_ = mCompCoord[38];
   double tmp147_ = tmp146_*(tmp13_);
   double tmp148_ = mCompCoord[39];
   double tmp149_ = tmp148_*tmp22_;
   double tmp150_ = mCompCoord[40];
   double tmp151_ = tmp150_*tmp24_;
   double tmp152_ = mCompCoord[42];
   double tmp153_ = tmp152_*(tmp13_);
   double tmp154_ = mCompCoord[43];
   double tmp155_ = tmp154_*tmp22_;
   double tmp156_ = mCompCoord[44];
   double tmp157_ = tmp156_*tmp24_;
   double tmp158_ = mCompCoord[45];
   double tmp159_ = tmp158_*tmp26_;
   double tmp160_ = mCompCoord[47];
   double tmp161_ = tmp160_*(tmp13_);
   double tmp162_ = mCompCoord[48];
   double tmp163_ = tmp162_*tmp22_;
   double tmp164_ = mCompCoord[49];
   double tmp165_ = tmp164_*tmp24_;
   double tmp166_ = mCompCoord[50];
   double tmp167_ = tmp166_*tmp26_;
   double tmp168_ = mCompCoord[51];
   double tmp169_ = tmp168_*tmp28_;
   double tmp170_ = mCompCoord[53];
   double tmp171_ = tmp170_*(tmp13_);
   double tmp172_ = mCompCoord[54];
   double tmp173_ = tmp172_*tmp22_;
   double tmp174_ = mCompCoord[55];
   double tmp175_ = tmp174_*tmp24_;
   double tmp176_ = mCompCoord[56];
   double tmp177_ = tmp176_*tmp26_;
   double tmp178_ = mCompCoord[57];
   double tmp179_ = tmp178_*tmp28_;
   double tmp180_ = 6*(tmp34_);
   double tmp181_ = tmp180_*tmp28_;
   double tmp182_ = mCompCoord[58];
   double tmp183_ = 6*(tmp37_);
   double tmp184_ = tmp183_*tmp29_;
   double tmp185_ = tmp182_*tmp93_;
   double tmp186_ = 6*(tmp96_);
   double tmp187_ = tmp186_*tmp28_;
   double tmp188_ = 6*(tmp98_);
   double tmp189_ = tmp188_*tmp29_;
   double tmp190_ = 6*(tmp116_);
   double tmp191_ = tmp190_*tmp28_;
   double tmp192_ = 6*(tmp118_);
   double tmp193_ = tmp192_*tmp29_;
   double tmp194_ = tmp93_*tmp92_;

  mVal[0] = (tmp11_+mCompCoord[3]+mCompCoord[4]+tmp32_*(tmp21_)+mCompCoord[6]+tmp38_*(tmp21_)+tmp40_*tmp23_+mCompCoord[9]+tmp42_*(tmp21_)+tmp48_*tmp23_+tmp50_*tmp25_+mCompCoord[13]+tmp52_*(tmp21_)+tmp54_*tmp23_+tmp60_*tmp25_+tmp62_*tmp27_+mCompCoord[18]+tmp64_*(tmp21_)+tmp66_*tmp23_+tmp68_*tmp25_+tmp74_*tmp27_+tmp76_*tmp29_+mCompCoord[24]+tmp78_*(tmp21_)+tmp80_*tmp23_+tmp82_*tmp25_+tmp84_*tmp27_+tmp90_*tmp29_+tmp94_*tmp92_)-mLocPIm_x;

  mCompDer[0][0] = mLocGradX_x+(tmp34_)*tmp30_*(tmp21_)+(tmp37_)*tmp32_+(tmp34_)*tmp35_*(tmp21_)+(tmp37_)*tmp38_+tmp44_*tmp39_*tmp23_+tmp47_*tmp40_+(tmp34_)*tmp41_*(tmp21_)+(tmp37_)*tmp42_+tmp44_*tmp45_*tmp23_+tmp47_*tmp48_+tmp56_*tmp49_*tmp25_+tmp59_*tmp50_+(tmp34_)*tmp51_*(tmp21_)+(tmp37_)*tmp52_+tmp44_*tmp53_*tmp23_+tmp47_*tmp54_+tmp56_*tmp57_*tmp25_+tmp59_*tmp60_+tmp70_*tmp61_*tmp27_+tmp73_*tmp62_+(tmp34_)*tmp63_*(tmp21_)+(tmp37_)*tmp64_+tmp44_*tmp65_*tmp23_+tmp47_*tmp66_+tmp56_*tmp67_*tmp25_+tmp59_*tmp68_+tmp70_*tmp71_*tmp27_+tmp73_*tmp74_+tmp86_*tmp75_*tmp29_+tmp89_*tmp76_+(tmp34_)*tmp77_*(tmp21_)+(tmp37_)*tmp78_+tmp44_*tmp79_*tmp23_+tmp47_*tmp80_+tmp56_*tmp81_*tmp25_+tmp59_*tmp82_+tmp70_*tmp83_*tmp27_+tmp73_*tmp84_+tmp86_*tmp87_*tmp29_+tmp89_*tmp90_+tmp181_*tmp91_*tmp92_+tmp184_*tmp94_;
  mCompDer[0][1] = mLocGradY_x+(tmp96_)*tmp30_*(tmp21_)+(tmp98_)*tmp32_+(tmp96_)*tmp35_*(tmp21_)+(tmp98_)*tmp38_+tmp100_*tmp39_*tmp23_+tmp102_*tmp40_+(tmp96_)*tmp41_*(tmp21_)+(tmp98_)*tmp42_+tmp100_*tmp45_*tmp23_+tmp102_*tmp48_+tmp104_*tmp49_*tmp25_+tmp106_*tmp50_+(tmp96_)*tmp51_*(tmp21_)+(tmp98_)*tmp52_+tmp100_*tmp53_*tmp23_+tmp102_*tmp54_+tmp104_*tmp57_*tmp25_+tmp106_*tmp60_+tmp108_*tmp61_*tmp27_+tmp110_*tmp62_+(tmp96_)*tmp63_*(tmp21_)+(tmp98_)*tmp64_+tmp100_*tmp65_*tmp23_+tmp102_*tmp66_+tmp104_*tmp67_*tmp25_+tmp106_*tmp68_+tmp108_*tmp71_*tmp27_+tmp110_*tmp74_+tmp112_*tmp75_*tmp29_+tmp114_*tmp76_+(tmp96_)*tmp77_*(tmp21_)+(tmp98_)*tmp78_+tmp100_*tmp79_*tmp23_+tmp102_*tmp80_+tmp104_*tmp81_*tmp25_+tmp106_*tmp82_+tmp108_*tmp83_*tmp27_+tmp110_*tmp84_+tmp112_*tmp87_*tmp29_+tmp114_*tmp90_+tmp187_*tmp91_*tmp92_+tmp189_*tmp94_;
  mCompDer[0][2] = mLocGradZ_x+(tmp116_)*tmp30_*(tmp21_)+(tmp118_)*tmp32_+(tmp116_)*tmp35_*(tmp21_)+(tmp118_)*tmp38_+tmp120_*tmp39_*tmp23_+tmp122_*tmp40_+(tmp116_)*tmp41_*(tmp21_)+(tmp118_)*tmp42_+tmp120_*tmp45_*tmp23_+tmp122_*tmp48_+tmp124_*tmp49_*tmp25_+tmp126_*tmp50_+(tmp116_)*tmp51_*(tmp21_)+(tmp118_)*tmp52_+tmp120_*tmp53_*tmp23_+tmp122_*tmp54_+tmp124_*tmp57_*tmp25_+tmp126_*tmp60_+tmp128_*tmp61_*tmp27_+tmp130_*tmp62_+(tmp116_)*tmp63_*(tmp21_)+(tmp118_)*tmp64_+tmp120_*tmp65_*tmp23_+tmp122_*tmp66_+tmp124_*tmp67_*tmp25_+tmp126_*tmp68_+tmp128_*tmp71_*tmp27_+tmp130_*tmp74_+tmp132_*tmp75_*tmp29_+tmp134_*tmp76_+(tmp116_)*tmp77_*(tmp21_)+(tmp118_)*tmp78_+tmp120_*tmp79_*tmp23_+tmp122_*tmp80_+tmp124_*tmp81_*tmp25_+tmp126_*tmp82_+tmp128_*tmp83_*tmp27_+tmp130_*tmp84_+tmp132_*tmp87_*tmp29_+tmp134_*tmp90_+tmp191_*tmp91_*tmp92_+tmp193_*tmp94_;
  mCompDer[0][3] = 1;
  mCompDer[0][4] = 1;
  mCompDer[0][5] = tmp135_;
  mCompDer[0][6] = 1;
  mCompDer[0][7] = tmp135_;
  mCompDer[0][8] = tmp136_;
  mCompDer[0][9] = 1;
  mCompDer[0][10] = tmp135_;
  mCompDer[0][11] = tmp136_;
  mCompDer[0][12] = tmp137_;
  mCompDer[0][13] = 1;
  mCompDer[0][14] = tmp135_;
  mCompDer[0][15] = tmp136_;
  mCompDer[0][16] = tmp137_;
  mCompDer[0][17] = tmp138_;
  mCompDer[0][18] = 1;
  mCompDer[0][19] = tmp135_;
  mCompDer[0][20] = tmp136_;
  mCompDer[0][21] = tmp137_;
  mCompDer[0][22] = tmp138_;
  mCompDer[0][23] = tmp139_;
  mCompDer[0][24] = 1;
  mCompDer[0][25] = tmp135_;
  mCompDer[0][26] = tmp136_;
  mCompDer[0][27] = tmp137_;
  mCompDer[0][28] = tmp138_;
  mCompDer[0][29] = tmp139_;
  mCompDer[0][30] = tmp194_;
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
  mVal[1] = (tmp19_+mCompCoord[31]+mCompCoord[32]+tmp141_*(tmp21_)+mCompCoord[34]+tmp143_*(tmp21_)+tmp145_*tmp23_+mCompCoord[37]+tmp147_*(tmp21_)+tmp149_*tmp23_+tmp151_*tmp25_+mCompCoord[41]+tmp153_*(tmp21_)+tmp155_*tmp23_+tmp157_*tmp25_+tmp159_*tmp27_+mCompCoord[46]+tmp161_*(tmp21_)+tmp163_*tmp23_+tmp165_*tmp25_+tmp167_*tmp27_+tmp169_*tmp29_+mCompCoord[52]+tmp171_*(tmp21_)+tmp173_*tmp23_+tmp175_*tmp25_+tmp177_*tmp27_+tmp179_*tmp29_+tmp185_*tmp92_)-mLocPIm_y;

  mCompDer[1][0] = mLocGradX_y+(tmp34_)*tmp140_*(tmp21_)+(tmp37_)*tmp141_+(tmp34_)*tmp142_*(tmp21_)+(tmp37_)*tmp143_+tmp44_*tmp144_*tmp23_+tmp47_*tmp145_+(tmp34_)*tmp146_*(tmp21_)+(tmp37_)*tmp147_+tmp44_*tmp148_*tmp23_+tmp47_*tmp149_+tmp56_*tmp150_*tmp25_+tmp59_*tmp151_+(tmp34_)*tmp152_*(tmp21_)+(tmp37_)*tmp153_+tmp44_*tmp154_*tmp23_+tmp47_*tmp155_+tmp56_*tmp156_*tmp25_+tmp59_*tmp157_+tmp70_*tmp158_*tmp27_+tmp73_*tmp159_+(tmp34_)*tmp160_*(tmp21_)+(tmp37_)*tmp161_+tmp44_*tmp162_*tmp23_+tmp47_*tmp163_+tmp56_*tmp164_*tmp25_+tmp59_*tmp165_+tmp70_*tmp166_*tmp27_+tmp73_*tmp167_+tmp86_*tmp168_*tmp29_+tmp89_*tmp169_+(tmp34_)*tmp170_*(tmp21_)+(tmp37_)*tmp171_+tmp44_*tmp172_*tmp23_+tmp47_*tmp173_+tmp56_*tmp174_*tmp25_+tmp59_*tmp175_+tmp70_*tmp176_*tmp27_+tmp73_*tmp177_+tmp86_*tmp178_*tmp29_+tmp89_*tmp179_+tmp181_*tmp182_*tmp92_+tmp184_*tmp185_;
  mCompDer[1][1] = mLocGradY_y+(tmp96_)*tmp140_*(tmp21_)+(tmp98_)*tmp141_+(tmp96_)*tmp142_*(tmp21_)+(tmp98_)*tmp143_+tmp100_*tmp144_*tmp23_+tmp102_*tmp145_+(tmp96_)*tmp146_*(tmp21_)+(tmp98_)*tmp147_+tmp100_*tmp148_*tmp23_+tmp102_*tmp149_+tmp104_*tmp150_*tmp25_+tmp106_*tmp151_+(tmp96_)*tmp152_*(tmp21_)+(tmp98_)*tmp153_+tmp100_*tmp154_*tmp23_+tmp102_*tmp155_+tmp104_*tmp156_*tmp25_+tmp106_*tmp157_+tmp108_*tmp158_*tmp27_+tmp110_*tmp159_+(tmp96_)*tmp160_*(tmp21_)+(tmp98_)*tmp161_+tmp100_*tmp162_*tmp23_+tmp102_*tmp163_+tmp104_*tmp164_*tmp25_+tmp106_*tmp165_+tmp108_*tmp166_*tmp27_+tmp110_*tmp167_+tmp112_*tmp168_*tmp29_+tmp114_*tmp169_+(tmp96_)*tmp170_*(tmp21_)+(tmp98_)*tmp171_+tmp100_*tmp172_*tmp23_+tmp102_*tmp173_+tmp104_*tmp174_*tmp25_+tmp106_*tmp175_+tmp108_*tmp176_*tmp27_+tmp110_*tmp177_+tmp112_*tmp178_*tmp29_+tmp114_*tmp179_+tmp187_*tmp182_*tmp92_+tmp189_*tmp185_;
  mCompDer[1][2] = mLocGradZ_y+(tmp116_)*tmp140_*(tmp21_)+(tmp118_)*tmp141_+(tmp116_)*tmp142_*(tmp21_)+(tmp118_)*tmp143_+tmp120_*tmp144_*tmp23_+tmp122_*tmp145_+(tmp116_)*tmp146_*(tmp21_)+(tmp118_)*tmp147_+tmp120_*tmp148_*tmp23_+tmp122_*tmp149_+tmp124_*tmp150_*tmp25_+tmp126_*tmp151_+(tmp116_)*tmp152_*(tmp21_)+(tmp118_)*tmp153_+tmp120_*tmp154_*tmp23_+tmp122_*tmp155_+tmp124_*tmp156_*tmp25_+tmp126_*tmp157_+tmp128_*tmp158_*tmp27_+tmp130_*tmp159_+(tmp116_)*tmp160_*(tmp21_)+(tmp118_)*tmp161_+tmp120_*tmp162_*tmp23_+tmp122_*tmp163_+tmp124_*tmp164_*tmp25_+tmp126_*tmp165_+tmp128_*tmp166_*tmp27_+tmp130_*tmp167_+tmp132_*tmp168_*tmp29_+tmp134_*tmp169_+(tmp116_)*tmp170_*(tmp21_)+(tmp118_)*tmp171_+tmp120_*tmp172_*tmp23_+tmp122_*tmp173_+tmp124_*tmp174_*tmp25_+tmp126_*tmp175_+tmp128_*tmp176_*tmp27_+tmp130_*tmp177_+tmp132_*tmp178_*tmp29_+tmp134_*tmp179_+tmp191_*tmp182_*tmp92_+tmp193_*tmp185_;
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
  mCompDer[1][32] = 1;
  mCompDer[1][33] = tmp135_;
  mCompDer[1][34] = 1;
  mCompDer[1][35] = tmp135_;
  mCompDer[1][36] = tmp136_;
  mCompDer[1][37] = 1;
  mCompDer[1][38] = tmp135_;
  mCompDer[1][39] = tmp136_;
  mCompDer[1][40] = tmp137_;
  mCompDer[1][41] = 1;
  mCompDer[1][42] = tmp135_;
  mCompDer[1][43] = tmp136_;
  mCompDer[1][44] = tmp137_;
  mCompDer[1][45] = tmp138_;
  mCompDer[1][46] = 1;
  mCompDer[1][47] = tmp135_;
  mCompDer[1][48] = tmp136_;
  mCompDer[1][49] = tmp137_;
  mCompDer[1][50] = tmp138_;
  mCompDer[1][51] = tmp139_;
  mCompDer[1][52] = 1;
  mCompDer[1][53] = tmp135_;
  mCompDer[1][54] = tmp136_;
  mCompDer[1][55] = tmp137_;
  mCompDer[1][56] = tmp138_;
  mCompDer[1][57] = tmp139_;
  mCompDer[1][58] = tmp194_;
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


