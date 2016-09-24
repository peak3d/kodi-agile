/*
 *      Copyright (C) 2016 Team KODI
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with KODI; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "Translators.h"
#include "addons/kodi-addon-dev-kit/include/kodi/xbmc_addon_types.h"

extern "C" {
namespace kodi {
namespace addon {
  
int GetAddonAudioFormat(enum AEDataFormat format)
{
  switch (format)
  {
  case AE_FMT_U8:       return AUDIO_FMT_U8;
  case AE_FMT_S16BE:    return AUDIO_FMT_S16BE;
  case AE_FMT_S16LE:    return AUDIO_FMT_S16LE;
  case AE_FMT_S16NE:    return AUDIO_FMT_S16NE;
  case AE_FMT_S32BE:    return AUDIO_FMT_S32BE;
  case AE_FMT_S32LE:    return AUDIO_FMT_S32LE;
  case AE_FMT_S32NE:    return AUDIO_FMT_S32NE;
  case AE_FMT_S24BE4:   return AUDIO_FMT_S24BE4;
  case AE_FMT_S24LE4:   return AUDIO_FMT_S24LE4;
  case AE_FMT_S24NE4:   return AUDIO_FMT_S24NE4;
  case AE_FMT_S24NE4MSB: return AUDIO_FMT_S24NE4MSB;
  case AE_FMT_S24BE3:   return AUDIO_FMT_S24BE3;
  case AE_FMT_S24LE3:   return AUDIO_FMT_S24LE3;
  case AE_FMT_S24NE3:   return AUDIO_FMT_S24NE3;
  case AE_FMT_DOUBLE:   return AUDIO_FMT_DOUBLE;
  case AE_FMT_FLOAT:    return AUDIO_FMT_FLOAT;
  case AE_FMT_RAW:      return AUDIO_FMT_RAW;
  case AE_FMT_U8P:      return AUDIO_FMT_U8P;
  case AE_FMT_S16NEP:   return AUDIO_FMT_S16NEP;
  case AE_FMT_S32NEP:   return AUDIO_FMT_S32NEP;
  case AE_FMT_S24NE4P:  return AUDIO_FMT_S24NE4P;
  case AE_FMT_S24NE4MSBP: return AUDIO_FMT_S24NE4MSBP;
  case AE_FMT_S24NE3P:  return AUDIO_FMT_S24NE3P;
  case AE_FMT_DOUBLEP:  return AUDIO_FMT_DOUBLEP;
  case AE_FMT_FLOATP:   return AUDIO_FMT_FLOATP;
  case AE_FMT_MAX:      return AUDIO_FMT_MAX;
  default:
    return AUDIO_FMT_INVALID;
  }
}

enum AEDataFormat GetKodiAudioFormat(int format)
{
  switch (format)
  {
  case AUDIO_FMT_U8:       return AE_FMT_U8;
  case AUDIO_FMT_S16BE:    return AE_FMT_S16BE;
  case AUDIO_FMT_S16LE:    return AE_FMT_S16LE;
  case AUDIO_FMT_S16NE:    return AE_FMT_S16NE;
  case AUDIO_FMT_S32BE:    return AE_FMT_S32BE;
  case AUDIO_FMT_S32LE:    return AE_FMT_S32LE;
  case AUDIO_FMT_S32NE:    return AE_FMT_S32NE;
  case AUDIO_FMT_S24BE4:   return AE_FMT_S24BE4;
  case AUDIO_FMT_S24LE4:   return AE_FMT_S24LE4;
  case AUDIO_FMT_S24NE4:   return AE_FMT_S24NE4;
  case AUDIO_FMT_S24NE4MSB: return AE_FMT_S24NE4MSB;
  case AUDIO_FMT_S24BE3:   return AE_FMT_S24BE3;
  case AUDIO_FMT_S24LE3:   return AE_FMT_S24LE3;
  case AUDIO_FMT_S24NE3:   return AE_FMT_S24NE3;
  case AUDIO_FMT_DOUBLE:   return AE_FMT_DOUBLE;
  case AUDIO_FMT_FLOAT:    return AE_FMT_FLOAT;
  case AUDIO_FMT_RAW:      return AE_FMT_RAW;
  case AUDIO_FMT_U8P:      return AE_FMT_U8P;
  case AUDIO_FMT_S16NEP:   return AE_FMT_S16NEP;
  case AUDIO_FMT_S32NEP:   return AE_FMT_S32NEP;
  case AUDIO_FMT_S24NE4P:  return AE_FMT_S24NE4P;
  case AUDIO_FMT_S24NE4MSBP: return AE_FMT_S24NE4MSBP;
  case AUDIO_FMT_S24NE3P:  return AE_FMT_S24NE3P;
  case AUDIO_FMT_DOUBLEP:  return AE_FMT_DOUBLEP;
  case AUDIO_FMT_FLOATP:   return AE_FMT_FLOATP;
  case AUDIO_FMT_MAX:      return AE_FMT_MAX;
  default:
    return AE_FMT_INVALID;
  }
}

int GetAddonAudioChannel(enum AEChannel channel)
{
  switch (channel)
  {
  case AE_CH_RAW:       return AUDIO_CH_RAW;
  case AE_CH_FL:        return AUDIO_CH_FL;
  case AE_CH_FR:        return AUDIO_CH_FR;
  case AE_CH_FC:        return AUDIO_CH_FC;
  case AE_CH_LFE:       return AUDIO_CH_LFE;
  case AE_CH_BL:        return AUDIO_CH_BL;
  case AE_CH_BR:        return AUDIO_CH_BR;
  case AE_CH_FLOC:      return AUDIO_CH_FLOC;
  case AE_CH_FROC:      return AUDIO_CH_FROC;
  case AE_CH_BC:        return AUDIO_CH_BC;
  case AE_CH_SL:        return AUDIO_CH_SL;
  case AE_CH_SR:        return AUDIO_CH_SR;
  case AE_CH_TFL:       return AUDIO_CH_TFL;
  case AE_CH_TFR:       return AUDIO_CH_TFR;
  case AE_CH_TFC:       return AUDIO_CH_TFC;
  case AE_CH_TC:        return AUDIO_CH_TC;
  case AE_CH_TBL:       return AUDIO_CH_TBL;
  case AE_CH_TBR:       return AUDIO_CH_TBR;
  case AE_CH_TBC:       return AUDIO_CH_TBC;
  case AE_CH_BLOC:      return AUDIO_CH_BLOC;
  case AE_CH_BROC:      return AUDIO_CH_BROC;
  
  /* p16v devices */
  case AE_CH_UNKNOWN1:  return AUDIO_CH_UNKNOWN1;
  case AE_CH_UNKNOWN2:  return AUDIO_CH_UNKNOWN2;
  case AE_CH_UNKNOWN3:  return AUDIO_CH_UNKNOWN3;
  case AE_CH_UNKNOWN4:  return AUDIO_CH_UNKNOWN4;
  case AE_CH_UNKNOWN5:  return AUDIO_CH_UNKNOWN5;
  case AE_CH_UNKNOWN6:  return AUDIO_CH_UNKNOWN6;
  case AE_CH_UNKNOWN7:  return AUDIO_CH_UNKNOWN7;
  case AE_CH_UNKNOWN8:  return AUDIO_CH_UNKNOWN8;
  case AE_CH_UNKNOWN9:  return AUDIO_CH_UNKNOWN9;
  case AE_CH_UNKNOWN10: return AUDIO_CH_UNKNOWN10;
  case AE_CH_UNKNOWN11: return AUDIO_CH_UNKNOWN11;
  case AE_CH_UNKNOWN12: return AUDIO_CH_UNKNOWN12;
  case AE_CH_UNKNOWN13: return AUDIO_CH_UNKNOWN13;
  case AE_CH_UNKNOWN14: return AUDIO_CH_UNKNOWN14;
  case AE_CH_UNKNOWN15: return AUDIO_CH_UNKNOWN15;
  case AE_CH_UNKNOWN16: return AUDIO_CH_UNKNOWN16;
  case AE_CH_UNKNOWN17: return AUDIO_CH_UNKNOWN17;
  case AE_CH_UNKNOWN18: return AUDIO_CH_UNKNOWN18;
  case AE_CH_UNKNOWN19: return AUDIO_CH_UNKNOWN19;
  case AE_CH_UNKNOWN20: return AUDIO_CH_UNKNOWN20;
  case AE_CH_UNKNOWN21: return AUDIO_CH_UNKNOWN21;
  case AE_CH_UNKNOWN22: return AUDIO_CH_UNKNOWN22;
  case AE_CH_UNKNOWN23: return AUDIO_CH_UNKNOWN23;
  case AE_CH_UNKNOWN24: return AUDIO_CH_UNKNOWN24;
  case AE_CH_UNKNOWN25: return AUDIO_CH_UNKNOWN25;
  case AE_CH_UNKNOWN26: return AUDIO_CH_UNKNOWN26;
  case AE_CH_UNKNOWN27: return AUDIO_CH_UNKNOWN27;
  case AE_CH_UNKNOWN28: return AUDIO_CH_UNKNOWN28;
  case AE_CH_UNKNOWN29: return AUDIO_CH_UNKNOWN29;
  case AE_CH_UNKNOWN30: return AUDIO_CH_UNKNOWN30;
  case AE_CH_UNKNOWN31: return AUDIO_CH_UNKNOWN31;
  case AE_CH_UNKNOWN32: return AUDIO_CH_UNKNOWN32;
  case AE_CH_UNKNOWN33: return AUDIO_CH_UNKNOWN33;
  case AE_CH_UNKNOWN34: return AUDIO_CH_UNKNOWN34;
  case AE_CH_UNKNOWN35: return AUDIO_CH_UNKNOWN35;
  case AE_CH_UNKNOWN36: return AUDIO_CH_UNKNOWN36;
  case AE_CH_UNKNOWN37: return AUDIO_CH_UNKNOWN37;
  case AE_CH_UNKNOWN38: return AUDIO_CH_UNKNOWN38;
  case AE_CH_UNKNOWN39: return AUDIO_CH_UNKNOWN39;
  case AE_CH_UNKNOWN40: return AUDIO_CH_UNKNOWN40;
  case AE_CH_UNKNOWN41: return AUDIO_CH_UNKNOWN41;
  case AE_CH_UNKNOWN42: return AUDIO_CH_UNKNOWN42;
  case AE_CH_UNKNOWN43: return AUDIO_CH_UNKNOWN43;
  case AE_CH_UNKNOWN44: return AUDIO_CH_UNKNOWN44;
  case AE_CH_UNKNOWN45: return AUDIO_CH_UNKNOWN45;
  case AE_CH_UNKNOWN46: return AUDIO_CH_UNKNOWN46;
  case AE_CH_UNKNOWN47: return AUDIO_CH_UNKNOWN47;
  case AE_CH_UNKNOWN48: return AUDIO_CH_UNKNOWN48;
  case AE_CH_UNKNOWN49: return AUDIO_CH_UNKNOWN49;
  case AE_CH_UNKNOWN50: return AUDIO_CH_UNKNOWN50;
  case AE_CH_UNKNOWN51: return AUDIO_CH_UNKNOWN51;
  case AE_CH_UNKNOWN52: return AUDIO_CH_UNKNOWN52;
  case AE_CH_UNKNOWN53: return AUDIO_CH_UNKNOWN53;
  case AE_CH_UNKNOWN54: return AUDIO_CH_UNKNOWN54;
  case AE_CH_UNKNOWN55: return AUDIO_CH_UNKNOWN55;
  case AE_CH_UNKNOWN56: return AUDIO_CH_UNKNOWN56;
  case AE_CH_UNKNOWN57: return AUDIO_CH_UNKNOWN57;
  case AE_CH_UNKNOWN58: return AUDIO_CH_UNKNOWN58;
  case AE_CH_UNKNOWN59: return AUDIO_CH_UNKNOWN59;
  case AE_CH_UNKNOWN60: return AUDIO_CH_UNKNOWN60;
  case AE_CH_UNKNOWN61: return AUDIO_CH_UNKNOWN61;
  case AE_CH_UNKNOWN62: return AUDIO_CH_UNKNOWN62;
  case AE_CH_UNKNOWN63: return AUDIO_CH_UNKNOWN63;
  case AE_CH_UNKNOWN64: return AUDIO_CH_UNKNOWN64;
  case AE_CH_MAX:       return AUDIO_CH_MAX;
  case AE_CH_NULL:
  default:
    return AUDIO_CH_NULL;
  }
};

enum AEChannel GetKodiAudioChannel(int channel)
{
  switch (channel)
  {
  case AUDIO_CH_RAW:       return AE_CH_RAW;
  case AUDIO_CH_FL:        return AE_CH_FL;
  case AUDIO_CH_FR:        return AE_CH_FR;
  case AUDIO_CH_FC:        return AE_CH_FC;
  case AUDIO_CH_LFE:       return AE_CH_LFE;
  case AUDIO_CH_BL:        return AE_CH_BL;
  case AUDIO_CH_BR:        return AE_CH_BR;
  case AUDIO_CH_FLOC:      return AE_CH_FLOC;
  case AUDIO_CH_FROC:      return AE_CH_FROC;
  case AUDIO_CH_BC:        return AE_CH_BC;
  case AUDIO_CH_SL:        return AE_CH_SL;
  case AUDIO_CH_SR:        return AE_CH_SR;
  case AUDIO_CH_TFL:       return AE_CH_TFL;
  case AUDIO_CH_TFR:       return AE_CH_TFR;
  case AUDIO_CH_TFC:       return AE_CH_TFC;
  case AUDIO_CH_TC:        return AE_CH_TC;
  case AUDIO_CH_TBL:       return AE_CH_TBL;
  case AUDIO_CH_TBR:       return AE_CH_TBR;
  case AUDIO_CH_TBC:       return AE_CH_TBC;
  case AUDIO_CH_BLOC:      return AE_CH_BLOC;
  case AUDIO_CH_BROC:      return AE_CH_BROC;
  
  /* p16v devices */
  case AUDIO_CH_UNKNOWN1:  return AE_CH_UNKNOWN1;
  case AUDIO_CH_UNKNOWN2:  return AE_CH_UNKNOWN2;
  case AUDIO_CH_UNKNOWN3:  return AE_CH_UNKNOWN3;
  case AUDIO_CH_UNKNOWN4:  return AE_CH_UNKNOWN4;
  case AUDIO_CH_UNKNOWN5:  return AE_CH_UNKNOWN5;
  case AUDIO_CH_UNKNOWN6:  return AE_CH_UNKNOWN6;
  case AUDIO_CH_UNKNOWN7:  return AE_CH_UNKNOWN7;
  case AUDIO_CH_UNKNOWN8:  return AE_CH_UNKNOWN8;
  case AUDIO_CH_UNKNOWN9:  return AE_CH_UNKNOWN9;
  case AUDIO_CH_UNKNOWN10: return AE_CH_UNKNOWN10;
  case AUDIO_CH_UNKNOWN11: return AE_CH_UNKNOWN11;
  case AUDIO_CH_UNKNOWN12: return AE_CH_UNKNOWN12;
  case AUDIO_CH_UNKNOWN13: return AE_CH_UNKNOWN13;
  case AUDIO_CH_UNKNOWN14: return AE_CH_UNKNOWN14;
  case AUDIO_CH_UNKNOWN15: return AE_CH_UNKNOWN15;
  case AUDIO_CH_UNKNOWN16: return AE_CH_UNKNOWN16;
  case AUDIO_CH_UNKNOWN17: return AE_CH_UNKNOWN17;
  case AUDIO_CH_UNKNOWN18: return AE_CH_UNKNOWN18;
  case AUDIO_CH_UNKNOWN19: return AE_CH_UNKNOWN19;
  case AUDIO_CH_UNKNOWN20: return AE_CH_UNKNOWN20;
  case AUDIO_CH_UNKNOWN21: return AE_CH_UNKNOWN21;
  case AUDIO_CH_UNKNOWN22: return AE_CH_UNKNOWN22;
  case AUDIO_CH_UNKNOWN23: return AE_CH_UNKNOWN23;
  case AUDIO_CH_UNKNOWN24: return AE_CH_UNKNOWN24;
  case AUDIO_CH_UNKNOWN25: return AE_CH_UNKNOWN25;
  case AUDIO_CH_UNKNOWN26: return AE_CH_UNKNOWN26;
  case AUDIO_CH_UNKNOWN27: return AE_CH_UNKNOWN27;
  case AUDIO_CH_UNKNOWN28: return AE_CH_UNKNOWN28;
  case AUDIO_CH_UNKNOWN29: return AE_CH_UNKNOWN29;
  case AUDIO_CH_UNKNOWN30: return AE_CH_UNKNOWN30;
  case AUDIO_CH_UNKNOWN31: return AE_CH_UNKNOWN31;
  case AUDIO_CH_UNKNOWN32: return AE_CH_UNKNOWN32;
  case AUDIO_CH_UNKNOWN33: return AE_CH_UNKNOWN33;
  case AUDIO_CH_UNKNOWN34: return AE_CH_UNKNOWN34;
  case AUDIO_CH_UNKNOWN35: return AE_CH_UNKNOWN35;
  case AUDIO_CH_UNKNOWN36: return AE_CH_UNKNOWN36;
  case AUDIO_CH_UNKNOWN37: return AE_CH_UNKNOWN37;
  case AUDIO_CH_UNKNOWN38: return AE_CH_UNKNOWN38;
  case AUDIO_CH_UNKNOWN39: return AE_CH_UNKNOWN39;
  case AUDIO_CH_UNKNOWN40: return AE_CH_UNKNOWN40;
  case AUDIO_CH_UNKNOWN41: return AE_CH_UNKNOWN41;
  case AUDIO_CH_UNKNOWN42: return AE_CH_UNKNOWN42;
  case AUDIO_CH_UNKNOWN43: return AE_CH_UNKNOWN43;
  case AUDIO_CH_UNKNOWN44: return AE_CH_UNKNOWN44;
  case AUDIO_CH_UNKNOWN45: return AE_CH_UNKNOWN45;
  case AUDIO_CH_UNKNOWN46: return AE_CH_UNKNOWN46;
  case AUDIO_CH_UNKNOWN47: return AE_CH_UNKNOWN47;
  case AUDIO_CH_UNKNOWN48: return AE_CH_UNKNOWN48;
  case AUDIO_CH_UNKNOWN49: return AE_CH_UNKNOWN49;
  case AUDIO_CH_UNKNOWN50: return AE_CH_UNKNOWN50;
  case AUDIO_CH_UNKNOWN51: return AE_CH_UNKNOWN51;
  case AUDIO_CH_UNKNOWN52: return AE_CH_UNKNOWN52;
  case AUDIO_CH_UNKNOWN53: return AE_CH_UNKNOWN53;
  case AUDIO_CH_UNKNOWN54: return AE_CH_UNKNOWN54;
  case AUDIO_CH_UNKNOWN55: return AE_CH_UNKNOWN55;
  case AUDIO_CH_UNKNOWN56: return AE_CH_UNKNOWN56;
  case AUDIO_CH_UNKNOWN57: return AE_CH_UNKNOWN57;
  case AUDIO_CH_UNKNOWN58: return AE_CH_UNKNOWN58;
  case AUDIO_CH_UNKNOWN59: return AE_CH_UNKNOWN59;
  case AUDIO_CH_UNKNOWN60: return AE_CH_UNKNOWN60;
  case AUDIO_CH_UNKNOWN61: return AE_CH_UNKNOWN61;
  case AUDIO_CH_UNKNOWN62: return AE_CH_UNKNOWN62;
  case AUDIO_CH_UNKNOWN63: return AE_CH_UNKNOWN63;
  case AUDIO_CH_UNKNOWN64: return AE_CH_UNKNOWN64;
  case AUDIO_CH_MAX:       return AE_CH_MAX;
  case AUDIO_CH_NULL:
  default:
    return AE_CH_NULL;
  }
};

} /* namespace addon */
} /* namespace kodi */
} /* extern "C" */
