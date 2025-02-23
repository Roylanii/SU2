/*!
 * \file CMultizoneOutput.cpp
 * \brief Main subroutines for multizone output
 * \author R. Sanchez, T. Albring
 * \version 7.2.1 "Blackbird"
 *
 * SU2 Project Website: https://su2code.github.io
 *
 * The SU2 Project is maintained by the SU2 Foundation
 * (http://su2foundation.org)
 *
 * Copyright 2012-2021, SU2 Contributors (cf. AUTHORS.md)
 *
 * SU2 is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * SU2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with SU2. If not, see <http://www.gnu.org/licenses/>.
 */

#include "../../include/output/CMultizoneOutput.hpp"

CMultizoneOutput::CMultizoneOutput(const CConfig* driver_config, const CConfig* const* config, unsigned short nDim) :
  COutput(driver_config, nDim, false) {

  unsigned short iZone = 0;

  nZone = driver_config->GetnZone();

  bgs_res_name = "BGS_RES";

  write_zone = false;

  /*--- If the zone output is disabled for every zone ---*/
  for (iZone = 0; iZone < nZone; iZone++){
    write_zone = config[iZone]->GetWrt_ZoneConv();
  }

  if (nRequestedHistoryFields == 0){
    requestedHistoryFields.emplace_back("ITER");
    for (iZone = 0; iZone < nZone; iZone++){
      requestedHistoryFields.emplace_back(bgs_res_name + "[" + PrintingToolbox::to_string(iZone) + "]");
      requestedHistoryFields.emplace_back("AVG_RES[" + PrintingToolbox::to_string(iZone) + "]");
    }
    nRequestedHistoryFields = requestedHistoryFields.size();
  }

  if (nRequestedScreenFields == 0){
    if (config[ZONE_0]->GetTime_Domain()) requestedScreenFields.emplace_back("TIME_ITER");
    requestedScreenFields.emplace_back("OUTER_ITER");
    for (iZone = 0; iZone < nZone; iZone++){
      requestedScreenFields.emplace_back("AVG_" + bgs_res_name + "[" + PrintingToolbox::to_string(iZone) + "]");
    }
    nRequestedScreenFields = requestedScreenFields.size();
  }

  multiZoneHeaderString = "Multizone Summary";

  historyFilename = driver_config->GetCaseName();

  /*--- Add the correct file extension depending on the file format ---*/

  string hist_ext = ".csv";
  if (driver_config->GetTabular_FileFormat() == TAB_TECPLOT) hist_ext = ".dat";

  historyFilename += hist_ext;

  /*--- Set the default convergence field --- */

  if (convFields.empty()) convFields.emplace_back("AVG_BGS_RES[0]");

}

void CMultizoneOutput::LoadMultizoneHistoryData(COutput **output, CConfig **config) {

  unsigned short iZone, iField, nField;
  string name, header;

  for (iZone = 0; iZone < nZone; iZone++){

    map<string, HistoryOutputField> ZoneHistoryFields = output[iZone]->GetHistoryFields();
    vector<string>                  ZoneHistoryNames  = output[iZone]->GetHistoryOutput_List();

    nField = ZoneHistoryNames.size();


    /*-- For all the variables per solver --*/
    for (iField = 0; iField < nField; iField++){

      if (ZoneHistoryNames[iField] != "TIME_ITER" && ZoneHistoryNames[iField] != "OUTER_ITER"){

        name   = ZoneHistoryNames[iField]+ "[" + PrintingToolbox::to_string(iZone) + "]";

        SetHistoryOutputValue(name, ZoneHistoryFields[ZoneHistoryNames[iField]].value);

      }
    }
  }
}

void CMultizoneOutput::SetMultizoneHistoryOutputFields(COutput **output, CConfig **config) {

  unsigned short iZone, iField, nField;
  string name, header, group;

  /*--- Set the fields ---*/
  for (iZone = 0; iZone < nZone; iZone++){

    map<string, HistoryOutputField> ZoneHistoryFields = output[iZone]->GetHistoryFields();
    vector<string>                  ZoneHistoryNames  = output[iZone]->GetHistoryOutput_List();

    nField = ZoneHistoryNames.size();


    /*-- For all the variables per solver --*/
    for (iField = 0; iField < nField; iField++){

      if (ZoneHistoryNames[iField] != "TIME_ITER" && ZoneHistoryNames[iField] != "OUTER_ITER"){

        name   = ZoneHistoryNames[iField]+ "[" + PrintingToolbox::to_string(iZone) + "]";
        header = ZoneHistoryFields[ZoneHistoryNames[iField]].fieldName + "[" + PrintingToolbox::to_string(iZone) + "]";
        group  = ZoneHistoryFields[ZoneHistoryNames[iField]].outputGroup + "[" + PrintingToolbox::to_string(iZone) + "]";

        AddHistoryOutput(name, header, ZoneHistoryFields[ZoneHistoryNames[iField]].screenFormat, group, "", ZoneHistoryFields[ZoneHistoryNames[iField]].fieldType );
      }
    }
  }
}

bool CMultizoneOutput::WriteScreen_Header(const CConfig *config) {

  /*--- Print header if the outer iteration is zero or zonal convergence is printed ---*/

  return curOuterIter == 0 || write_zone;

  /*--- Always print header if it is forced ---*/

  if (headerNeeded){
    headerNeeded = false;
    return true;
  }

  return false;
}

bool CMultizoneOutput::WriteScreen_Output(const CConfig *config) {

  unsigned long ScreenWrt_Freq_Outer = config->GetScreen_Wrt_Freq(1);
  unsigned long ScreenWrt_Freq_Time  = config->GetScreen_Wrt_Freq(0);

  /*--- Check if screen output should be written --- */

  if (!PrintOutput(curTimeIter, ScreenWrt_Freq_Time)&&
      !(curTimeIter == config->GetnTime_Iter() - 1)){

    return false;

  }

  if (convergence) {return true;}

  if (!PrintOutput(curOuterIter, ScreenWrt_Freq_Outer) &&
      !(curOuterIter == config->GetnOuter_Iter() - 1)){

    return false;

  }

  return true;
}

bool CMultizoneOutput::WriteHistoryFile_Output(const CConfig *config){

  unsigned long HistoryWrt_Freq_Outer = config->GetHistory_Wrt_Freq(1);
  unsigned long HistoryWrt_Freq_Time  = config->GetHistory_Wrt_Freq(0);

  /*--- Check if screen output should be written --- */

  if (!PrintOutput(curTimeIter, HistoryWrt_Freq_Time)&&
      !(curTimeIter == config->GetnTime_Iter() - 1)){

    return false;

  }

  if (convergence) {return true;}

  if (!PrintOutput(curOuterIter, HistoryWrt_Freq_Outer) &&
      !(curOuterIter == config->GetnOuter_Iter() - 1)){

    return false;

  }

  return true;
}
