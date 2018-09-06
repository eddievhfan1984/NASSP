/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

RTCC Calculations for Mission F

Project Apollo is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Project Apollo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Project Apollo; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

See http://nassp.sourceforge.net/license/ for more details.

**************************************************************************/

#include "Orbitersdk.h"
#include "soundlib.h"
#include "apolloguidance.h"
#include "saturn.h"
#include "LEM.h"
#include "../src_rtccmfd/OrbMech.h"
#include "mcc.h"
#include "rtcc.h"

bool RTCC::CalculationMTP_F(int fcn, LPVOID &pad, char * upString, char * upDesc, char * upMessage)
{
	char uplinkdata[1024 * 3];
	bool preliminary = false;
	bool scrubbed = false;

	//Hardcoded for now, better solution at some point...
	double AGCEpoch = 40221.525;
	double LSLat = 0.732*RAD;
	double LSLng = 23.647*RAD;
	double LSAlt = -3073.263;
	double LSAzi = -91.0*RAD;
	double EMPLat = -4.933294*RAD;

	int LGCREFSAddrOffs = -2;
	int LGCDeltaVAddr = 3431;

	switch (fcn) {
	case 1: //TLI+90 PAD + State Vector
	{
		EntryOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		double GETbase, TLIBase, TIG, CSMmass;
		SV sv, sv1;
		char buffer1[1000];

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = getGETBase();

		TLIBase = floor((TimeofIgnition / 1800.0) + 0.5)*1800.0; //Round to next half hour
		TIG = TLIBase + 90.0*60.0;
		entopt.lng = -25.0*RAD;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		CSMmass = 28862.0;

		sv1.mass = CSMmass;
		sv1.gravref = AGCGravityRef(calcParams.src);
		sv1.MJD = GETbase + calcParams.TLI / 24.0 / 3600.0;
		sv1.R = calcParams.R_TLI;
		sv1.V = calcParams.V_TLI;

		entopt.entrylongmanual = true;
		entopt.GETbase = GETbase;
		entopt.impulsive = RTCC_NONIMPULSIVE;
		entopt.ReA = 0;
		entopt.TIGguess = TIG;
		entopt.type = RTCC_ENTRY_ABORT;
		entopt.vessel = calcParams.src;
		entopt.useSV = true;
		entopt.RV_MCC = sv1;

		EntryTargeting(&entopt, &res); //Target Load for uplink

		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = true;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.TIG = res.P30TIG;
		opt.vessel = calcParams.src;
		opt.vesseltype = 0;
		opt.useSV = true;
		opt.RV_MCC = sv1;

		AP11ManeuverPAD(&opt, *form);
		form->lat = res.latitude*DEG;
		form->lng = res.longitude*DEG;
		form->RTGO = res.RTGO;
		form->VI0 = res.VIO / 0.3048;
		form->Weight = CSMmass / 0.45359237;
		form->GET05G = res.GET05G;

		sprintf(form->purpose, "TLI+90");

		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase, true);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector and V66");
		}
	}
	break;
	case 2: //TLI+4 PAD
	{
		AP11BLKOpt opt;
		double CSMmass, GETbase;
		SV sv1;

		P37PAD * form = (P37PAD *)pad;

		GETbase = getGETBase();

		CSMmass = 28862.0;

		sv1.mass = CSMmass;
		sv1.gravref = AGCGravityRef(calcParams.src);
		sv1.MJD = GETbase + calcParams.TLI / 24.0 / 3600.0;
		sv1.R = calcParams.R_TLI;
		sv1.V = calcParams.V_TLI;

		opt.n = 1;

		opt.GETI.push_back(OrbMech::HHMMSSToSS(6, 30, 0));
		opt.lng.push_back(-165.0*RAD);
		opt.useSV = true;
		opt.RV_MCC = sv1;

		AP11BlockData(&opt, *form);
	}
	break;
	case 3: //TLI PAD
	{
		TLIPADOpt opt;
		double GETbase;

		TLIPAD * form = (TLIPAD *)pad;

		GETbase = getGETBase();

		opt.dV_LVLH = DeltaV_LVLH;
		opt.GETbase = GETbase;
		opt.R_TLI = calcParams.R_TLI;
		opt.V_TLI = calcParams.V_TLI;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.TIG = TimeofIgnition;
		opt.TLI = calcParams.TLI;
		opt.vessel = calcParams.src;
		opt.SeparationAttitude = _V(PI, 120.0*RAD, 40.0*RAD);
		opt.uselvdc = true;

		TLI_PAD(&opt, *form);
	}
	break;
	case 4: //Evasive Maneuver
	{
		AP11ManPADOpt opt;

		AP11MNV * form = (AP11MNV *)pad;

		opt.dV_LVLH = _V(5.1, 0.0, 19.0)*0.3048;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = getGETBase();
		opt.HeadsUp = true;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.TIG = calcParams.TLI + 2.0*3600.0;
		opt.vessel = calcParams.src;
		opt.vesseltype = 1;

		AP11ManeuverPAD(&opt, *form);

		sprintf(form->purpose, "Evasive");
	}
	break;
	case 5: //Block Data 1
	case 6: //Block Data 2
	{
		AP11BLKOpt opt;
		double GETbase;

		P37PAD * form = (P37PAD *)pad;

		GETbase = getGETBase();

		double TLIbase = floor((calcParams.TLI / 1800.0) + 0.5)*1800.0; //Round to next half hour

		if (fcn == 5)
		{
			opt.n = 3;

			opt.GETI.push_back(TLIbase + 11.0*3600.0);
			opt.lng.push_back(-165.0*RAD);
			opt.GETI.push_back(TLIbase + 25.0*3600.0);
			opt.lng.push_back(-165.0*RAD);
			opt.GETI.push_back(TLIbase + 35.0*3600.0);
			opt.lng.push_back(-165.0*RAD);
		}
		else
		{
			opt.n = 4;

			opt.GETI.push_back(TLIbase + 25.0*3600.0);
			opt.lng.push_back(-165.0*RAD);
			opt.GETI.push_back(TLIbase + 35.0*3600.0);
			opt.lng.push_back(-165.0*RAD);
			opt.GETI.push_back(TLIbase + 44.0*3600.0);
			opt.lng.push_back(-165.0*RAD);
			opt.GETI.push_back(TLIbase + 53.0*3600.0);
			opt.lng.push_back(-165.0*RAD);
		}

		AP11BlockData(&opt, *form);
	}
	break;
	case 7: //PTC REFSMMAT
	{
		char buffer1[1000];

		MATRIX3 REFSMMAT = _M(-0.5, -0.8660254, 0.0, -0.79453912, 0.45872741, 0.39784005, -0.34453959, 0.19892003, -0.91745479);

		AGCDesiredREFSMMATUpdate(buffer1, REFSMMAT, AGCEpoch, true, true);
		sprintf(uplinkdata, "%s", buffer1);

		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "PTC REFSMMAT");
		}
	}
	break;
	case 10: //MCC-1
	case 11: //MCC-2
	{
		AP11ManPADOpt manopt;
		double GETbase, P30TIG, MCC1GET, MCC2GET, MCC3GET;
		VECTOR3 dV_LVLH;
		SV sv;
		char manname[8];

		if (fcn == 10)
		{
			sprintf(manname, "MCC1");
		}
		else if (fcn == 11)
		{
			sprintf(manname, "MCC2");
		}

		if (calcParams.LOI == 0)
		{
			calcParams.LOI = OrbMech::HHMMSSToSS(75.0, 49.0, 40.2);
		}

		if (calcParams.TLAND == 0)
		{
			calcParams.TLAND = OrbMech::HHMMSSToSS(102.0, 47.0, 11.0);
		}

		double TLIbase = floor((TimeofIgnition / 60.0) + 0.5)*60.0; //Round TLI ignition time to next minute

		MCC1GET = TLIbase + 9.0*3600.0;
		MCC2GET = TLIbase + 24.0*3600.0;
		MCC3GET = calcParams.LOI - 22.0*3600.0;

		AP11MNV * form = (AP11MNV *)pad;
		GETbase = getGETBase();

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		MCCFRMan opt;
		TLMCCResults res;

		opt.type = 0;
		opt.lat = EMPLat;
		opt.PeriGET = calcParams.LOI;
		opt.h_peri = 60.0 * 1852.0;
		opt.alt = LSAlt;
		opt.azi = LSAzi;
		opt.csmlmdocked = true;
		opt.GETbase = GETbase;

		opt.LOIh_apo = 170.0*1852.0;
		opt.LOIh_peri = 60.0*1852.0;
		opt.LSlat = LSLat;
		opt.LSlng = LSLng;
		opt.t_land = calcParams.TLAND;
		opt.vessel = calcParams.src;

		//Evaluate MCC-3 DV
		opt.MCCGET = MCC3GET;
		if (TranslunarMidcourseCorrectionTargetingFreeReturn(&opt, &res))
		{
			calcParams.alt_node = res.NodeAlt;
			calcParams.lat_node = res.NodeLat;
			calcParams.lng_node = res.NodeLng;
			calcParams.GET_node = res.NodeGET;
			calcParams.LOI = res.PericynthionGET;
		}

		if (length(res.dV_LVLH) < 25.0*0.3048)
		{
			scrubbed = true;
		}
		else
		{
			if (fcn == 10)
			{
				opt.MCCGET = MCC1GET;
			}
			else
			{
				opt.MCCGET = MCC2GET;
			}

			TranslunarMidcourseCorrectionTargetingFreeReturn(&opt, &res);

			//Scrub MCC-1 if DV is less than 50 ft/s
			if (fcn == 10 && length(res.dV_LVLH) < 50.0*0.3048)
			{
				scrubbed = true;
			}
			else
			{
				calcParams.alt_node = res.NodeAlt;
				calcParams.lat_node = res.NodeLat;
				calcParams.lng_node = res.NodeLng;
				calcParams.GET_node = res.NodeGET;
				calcParams.LOI = res.PericynthionGET;
				dV_LVLH = res.dV_LVLH;
				P30TIG = res.P30TIG;
			}
		}

		if (scrubbed)
		{
			char buffer1[1000];

			sprintf(upMessage, "%s has been scrubbed.", manname);
			sprintf(upDesc, "CSM state vector");

			AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);

			sprintf(uplinkdata, "%s", buffer1);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
			}
		}
		else
		{
			char buffer1[1000];
			char buffer2[1000];

			manopt.dV_LVLH = dV_LVLH;
			manopt.enginetype = SPSRCSDecision(SPS_THRUST / calcParams.src->GetMass(), dV_LVLH);
			manopt.GETbase = GETbase;
			manopt.HeadsUp = true;
			manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
			manopt.TIG = P30TIG;
			manopt.vessel = calcParams.src;
			manopt.vesseltype = 1;

			AP11ManeuverPAD(&manopt, *form);
			sprintf(form->purpose, manname);

			AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
			AGCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

			sprintf(uplinkdata, "%s%s", buffer1, buffer2);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector, target load");
			}
		}
	}
	break;
	case 12: //Lunar Flyby PAD
	{
		RTEFlybyOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		SV sv;
		double GETbase;
		char buffer1[1000];

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = getGETBase();

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		entopt.EntryLng = -165.0*RAD;
		entopt.GETbase = GETbase;
		entopt.returnspeed = 0;
		entopt.FlybyType = 0;
		entopt.TIGguess = calcParams.LOI - 5.0*3600.0;
		entopt.vessel = calcParams.src;

		RTEFlybyTargeting(&entopt, &res);

		SV sv_peri = FindPericynthion(res.sv_postburn);
		double h_peri = length(sv_peri.R) - oapiGetSize(oapiGetObjectByName("Moon"));

		opt.alt = LSAlt;
		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.TIG = res.P30TIG;
		opt.vessel = calcParams.src;
		opt.vesseltype = 1;

		AP11ManeuverPAD(&opt, *form);

		sprintf(form->purpose, "Flyby");
		sprintf(form->remarks, "Height of pericynthion is %.0f NM", h_peri / 1852.0);
		form->lat = res.latitude*DEG;
		form->lng = res.longitude*DEG;
		form->RTGO = res.RTGO;
		form->VI0 = res.VIO / 0.3048;
		form->GET05G = res.GET05G;

		//Save parameters for further use
		SplashLatitude = res.latitude;
		SplashLongitude = res.longitude;
		calcParams.TEI = res.P30TIG;
		calcParams.EI = res.GET400K;

		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector");
		}
	}
	break;
	case 13: //MCC-3
	{
		AP11ManPADOpt manopt;
		LOIMan loiopt;
		MCCNodeMan opt;
		VECTOR3 dV_LVLH, dV_LOI;
		SV sv, sv_peri, sv_node, sv_postLOI;
		double GETbase, MCCGET, P30TIG, r_M, TIG_LOI, h_peri, h_node;

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = getGETBase();
		r_M = oapiGetSize(oapiGetObjectByName("Moon"));
		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		loiopt.alt = LSAlt;
		loiopt.azi = LSAzi;
		loiopt.csmlmdocked = true;
		loiopt.GETbase = GETbase;
		loiopt.h_apo = 170.0*1852.0;
		loiopt.h_peri = 60.0*1852.0;
		loiopt.impulsive = RTCC_IMPULSIVE;
		loiopt.lat = LSLat;
		loiopt.lng = LSLng;
		loiopt.t_land = calcParams.TLAND;
		loiopt.vessel = calcParams.src;

		LOITargeting(&loiopt, dV_LOI, TIG_LOI, sv_node, sv_postLOI);

		sv_peri = FindPericynthion(sv);

		h_peri = length(sv_peri.R) - r_M;
		h_node = length(sv_node.R) - r_M;

		//Maneuver execution criteria
		if (h_peri > 50.0*1852.0 && h_peri < 70.0*1852.0)
		{
			if (h_node > 50.0*1852.0 && h_node < 75.0*1852.0)
			{
				scrubbed = true;
			}
		}

		if (scrubbed)
		{
			char buffer1[1000];

			sprintf(upMessage, "MCC-3 has been scrubbed");
			sprintf(upDesc, "CSM state vector");

			AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);

			sprintf(uplinkdata, "%s", buffer1);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
			}
		}
		else
		{
			char buffer1[1000];
			char buffer2[1000];

			MCCGET = calcParams.LOI - 22.0*3600.0;

			opt.lat = calcParams.lat_node;
			opt.lng = calcParams.lng_node;
			opt.NodeGET = calcParams.GET_node;
			opt.h_node = calcParams.alt_node;
			opt.csmlmdocked = true;
			opt.GETbase = GETbase;
			opt.MCCGET = MCCGET;
			opt.vessel = calcParams.src;

			TranslunarMidcourseCorrectionTargetingNodal(&opt, dV_LVLH, P30TIG);

			manopt.dV_LVLH = dV_LVLH;
			manopt.enginetype = SPSRCSDecision(SPS_THRUST / calcParams.src->GetMass(), dV_LVLH);
			manopt.GETbase = GETbase;
			manopt.HeadsUp = false;
			manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
			manopt.TIG = P30TIG;
			manopt.vessel = calcParams.src;
			manopt.vesseltype = 1;

			AP11ManeuverPAD(&manopt, *form);
			sprintf(form->purpose, "MCC-3");

			AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
			AGCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

			sprintf(uplinkdata, "%s%s", buffer1, buffer2);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector, target load");
			}
		}
	}
	break;
	case 14: //MCC-4
	{
		LOIMan loiopt;
		REFSMMATOpt refsopt;
		VECTOR3 dV_LVLH, dV_LOI;
		SV sv, sv_peri, sv_node, sv_postLOI;
		MATRIX3 REFSMMAT;
		double GETbase, MCCGET, P30TIG, r_M, TIG_LOI, h_peri, h_node;

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = getGETBase();
		r_M = oapiGetSize(oapiGetObjectByName("Moon"));
		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		loiopt.alt = LSAlt;
		loiopt.azi = LSAzi;
		loiopt.csmlmdocked = true;
		loiopt.GETbase = GETbase;
		loiopt.h_apo = 170.0*1852.0;
		loiopt.h_peri = 60.0*1852.0;
		loiopt.impulsive = RTCC_IMPULSIVE;
		loiopt.lat = LSLat;
		loiopt.lng = LSLng;
		loiopt.t_land = calcParams.TLAND;
		loiopt.vessel = calcParams.src;

		LOITargeting(&loiopt, dV_LOI, TIG_LOI, sv_node, sv_postLOI);

		sv_peri = FindPericynthion(sv);

		h_peri = length(sv_peri.R) - r_M;
		h_node = length(sv_node.R) - r_M;

		//Maneuver execution criteria
		if (h_peri > 50.0*1852.0 && h_peri < 70.0*1852.0)
		{
			if (h_node > 50.0*1852.0 && h_node < 75.0*1852.0)
			{
				scrubbed = true;
			}
		}

		//REFSMMAT calculation
		refsopt.GETbase = GETbase;
		refsopt.LSAzi = LSAzi;
		refsopt.LSLat = LSLat;
		refsopt.LSLng = LSLng;
		refsopt.REFSMMATopt = 8;
		refsopt.REFSMMATTime = calcParams.TLAND;

		REFSMMAT = REFSMMATCalc(&refsopt);

		if (scrubbed)
		{
			char buffer1[1000];
			char buffer2[1000];

			sprintf(upMessage, "MCC-4 has been scrubbed");
			sprintf(upDesc, "CSM state vector, Landing Site REFSMMAT");

			AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
			AGCDesiredREFSMMATUpdate(buffer2, REFSMMAT, AGCEpoch);

			sprintf(uplinkdata, "%s%s", buffer1, buffer2);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
			}
		}
		else
		{
			AP11ManPADOpt manopt;
			MCCNodeMan opt;
			char buffer1[1000];
			char buffer2[1000];
			char buffer3[1000];

			MCCGET = calcParams.LOI - 5.0*3600.0;

			opt.lat = calcParams.lat_node;
			opt.lng = calcParams.lng_node;
			opt.NodeGET = calcParams.GET_node;
			opt.h_node = calcParams.alt_node;
			opt.csmlmdocked = true;
			opt.GETbase = GETbase;
			opt.MCCGET = MCCGET;
			opt.vessel = calcParams.src;

			TranslunarMidcourseCorrectionTargetingNodal(&opt, dV_LVLH, P30TIG);

			manopt.dV_LVLH = dV_LVLH;
			manopt.enginetype = SPSRCSDecision(SPS_THRUST / calcParams.src->GetMass(), dV_LVLH);
			manopt.GETbase = GETbase;
			manopt.HeadsUp = false;
			manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
			manopt.TIG = P30TIG;
			manopt.vessel = calcParams.src;
			manopt.vesseltype = 1;

			AP11ManeuverPAD(&manopt, *form);
			sprintf(form->purpose, "MCC-4");

			AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
			AGCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);
			AGCDesiredREFSMMATUpdate(buffer3, REFSMMAT, AGCEpoch);

			sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector, target load, Landing Site REFSMMAT");
			}
		}

	}
	break;
	case 15: //PC+2 UPDATE
	{
		RTEFlybyOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		double GETbase;

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = getGETBase();

		entopt.EntryLng = -165.0*RAD;
		entopt.returnspeed = 2;
		entopt.GETbase = GETbase;
		entopt.FlybyType = 1;
		entopt.vessel = calcParams.src;

		RTEFlybyTargeting(&entopt, &res);//dV_LVLH, P30TIG, latitude, longitude, RET, RTGO, VIO, EntryAng);

		opt.alt = LSAlt;
		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.TIG = res.P30TIG;
		opt.vessel = calcParams.src;
		opt.vesseltype = 0;

		AP11ManeuverPAD(&opt, *form);

		if (!REFSMMATDecision(form->Att*RAD))
		{
			REFSMMATOpt refsopt;
			MATRIX3 REFSMMAT;

			refsopt.dV_LVLH = res.dV_LVLH;
			refsopt.GETbase = GETbase;
			refsopt.P30TIG = res.P30TIG;
			refsopt.REFSMMATopt = 0;
			refsopt.vessel = calcParams.src;

			REFSMMAT = REFSMMATCalc(&refsopt);

			opt.HeadsUp = true;
			opt.REFSMMAT = REFSMMAT;
			AP11ManeuverPAD(&opt, *form);

			sprintf(form->remarks, "Requires realignment to preferred REFSMMAT");
		}
		sprintf(form->purpose, "PC+2");
		form->lat = res.latitude*DEG;
		form->lng = res.longitude*DEG;
		form->RTGO = res.RTGO;
		form->VI0 = res.VIO / 0.3048;
		form->GET05G = res.GET05G;

		//Save parameters for further use
		SplashLatitude = res.latitude;
		SplashLongitude = res.longitude;
		calcParams.TEI = res.P30TIG;
		calcParams.EI = res.GET400K;
	}
	break;
	case 20:	// MISSION F PRELIMINARY LOI-1 MANEUVER
	case 21:	// MISSION F LOI-1 MANEUVER
	{
		LOIMan opt;
		AP11ManPADOpt manopt;
		double GETbase, P30TIG;
		VECTOR3 dV_LVLH;
		SV sv, sv_n, sv_postLOI;

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = getGETBase();

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		opt.csmlmdocked = true;
		opt.GETbase = GETbase;
		opt.h_apo = 170.0*1852.0;
		opt.h_peri = 60.0*1852.0;
		opt.alt = LSAlt;
		opt.azi = LSAzi;
		opt.lat = LSLat;
		opt.lng = LSLng;
		opt.t_land = calcParams.TLAND;
		opt.vessel = calcParams.src;

		LOITargeting(&opt, dV_LVLH, P30TIG, sv_n, sv_postLOI);

		manopt.alt = LSAlt;
		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		manopt.GETbase = GETbase;
		manopt.HeadsUp = false;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		manopt.TIG = P30TIG;
		manopt.vessel = calcParams.src;
		manopt.vesseltype = 1;

		AP11ManeuverPAD(&manopt, *form);
		sprintf(form->purpose, "LOI-1");
		sprintf(form->remarks, "LM weight is %.0f.", form->LMWeight);

		TimeofIgnition = P30TIG;
		DeltaV_LVLH = dV_LVLH;

		if (fcn == 21)
		{
			char buffer1[1000];
			char buffer2[1000];

			AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
			AGCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

			sprintf(uplinkdata, "%s%s", buffer1, buffer2);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector, target load");
			}
		}
	}
	break;
	case 22:	// MISSION F LOI-2 MANEUVER
	{
		LOI2Man opt;
		AP11ManPADOpt manopt;
		double GETbase, P30TIG;
		VECTOR3 dV_LVLH;
		SV sv;
		char buffer1[1000];
		char buffer2[1000];

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = getGETBase();

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		opt.alt = LSAlt;
		opt.csmlmdocked = true;
		opt.GETbase = GETbase;
		opt.h_circ = 60.0*1852.0;
		opt.vessel = calcParams.src;

		LOI2Targeting(&opt, dV_LVLH, P30TIG);

		manopt.alt = LSAlt;
		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		manopt.GETbase = GETbase;
		manopt.HeadsUp = false;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		manopt.TIG = P30TIG;
		manopt.vessel = calcParams.src;
		manopt.vesseltype = 1;

		AP11ManeuverPAD(&manopt, *form);
		sprintf(form->purpose, "LOI-2");

		TimeofIgnition = P30TIG;
		DeltaV_LVLH = dV_LVLH;

		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
		AGCExternalDeltaVUpdate(buffer2, P30TIG, dV_LVLH);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, target load");
		}
	}
	break;
	case 30: //TEI-1 UPDATE (PRE LOI-1)
	case 31: //TEI-4 UPDATE (PRE LOI-1)
	case 32: //TEI-5 UPDATE (PRE LOI-2)
	case 33: //TEI-10 UPDATE
	case 34: //TEI-22 UPDATE
	case 35: //TEI-23 UPDATE
	case 36: //TEI-24 UPDATE
	case 37: //TEI-25 UPDATE
	case 38: //TEI-26 UPDATE
	case 39: //TEI-27 UPDATE
	case 130: //TEI-29 UPDATE
	case 131: //TEI-30 UPDATE
	case 132: //PRELIMINARY TEI-31 UPDATE
	case 133: //FINAL TEI-31 UPDATE
	case 134: //TEI-32 UPDATE
	{
		TEIOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		double GETbase;
		SV sv0, sv1, sv2;
		char manname[8];

		AP11MNV * form = (AP11MNV *)pad;

		GETbase = getGETBase();
		sv0 = StateVectorCalc(calcParams.src); //State vector for uplink

		if (fcn == 30 || fcn == 31 || fcn == 32)
		{
			sv1 = ExecuteManeuver(calcParams.src, GETbase, TimeofIgnition, DeltaV_LVLH, sv0, GetDockedVesselMass(calcParams.src));
		}
		else
		{
			sv1 = sv0;
		}

		if (fcn == 30)
		{
			sprintf(manname, "TEI-1");
			sv2 = coast(sv1, 0.5*2.0*3600.0);
		}
		else if (fcn == 31)
		{
			sprintf(manname, "TEI-4");
			sv2 = coast(sv1, 3.5*2.0*3600.0);
		}
		else if (fcn == 32)
		{
			sprintf(manname, "TEI-5");
			sv2 = coast(sv1, 2.5*2.0*3600.0);
		}
		else if (fcn == 33)
		{
			sprintf(manname, "TEI-10");
			sv2 = coast(sv1, 5.5*2.0*3600.0);
		}
		else if (fcn == 34)
		{
			sprintf(manname, "TEI-22");
			sv2 = coast(sv1, 5.5*2.0*3600.0);
		}
		else if (fcn == 35)
		{
			sprintf(manname, "TEI-23");
			sv2 = coast(sv1, 0.5*2.0*3600.0);
		}
		else if (fcn == 36)
		{
			sprintf(manname, "TEI-24");
			sv2 = coast(sv1, 0.5*2.0*3600.0);
		}
		else if (fcn == 37)
		{
			sprintf(manname, "TEI-25");
			sv2 = coast(sv1, 0.5*2.0*3600.0);
		}
		else if (fcn == 38)
		{
			sprintf(manname, "TEI-26");
			sv2 = coast(sv1, 0.5*2.0*3600.0);
		}
		else if (fcn == 39)
		{
			sprintf(manname, "TEI-27");
			sv2 = coast(sv1, 0.5*2.0*3600.0);
		}
		else if (fcn == 130)
		{
			sprintf(manname, "TEI-29");
			sv2 = coast(sv1, 2.5*2.0*3600.0);
		}
		else if (fcn == 131)
		{
			sprintf(manname, "TEI-30");
			sv2 = coast(sv1, 1.0*2.0*3600.0);
		}
		else if (fcn == 132)
		{
			sprintf(manname, "TEI-31");
			sv2 = coast(sv1, 1.0*2.0*3600.0);
		}
		else if (fcn == 133)
		{
			sprintf(manname, "TEI-31");
			sv2 = sv1;
		}
		else if (fcn == 134)
		{
			sprintf(manname, "TEI-32");
			sv2 = coast(sv1, 1.5*2.0*3600.0);
		}

		entopt.EntryLng = -165.0*RAD;
		entopt.GETbase = GETbase;
		entopt.returnspeed = 1;
		entopt.RV_MCC = sv2;
		entopt.useSV = true;
		entopt.vessel = calcParams.src;

		TEITargeting(&entopt, &res);

		opt.alt = LSAlt;
		opt.dV_LVLH = res.dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.RV_MCC = sv1;
		opt.TIG = res.P30TIG;
		opt.useSV = true;
		opt.vessel = calcParams.src;
		opt.vesseltype = 0;

		AP11ManeuverPAD(&opt, *form);
		sprintf(form->purpose, manname);
		form->lat = res.latitude*DEG;
		form->lng = res.longitude*DEG;
		form->RTGO = res.RTGO;
		form->VI0 = res.VIO / 0.3048;
		form->GET05G = res.GET05G;

		if (fcn != 134)
		{
			//Save parameters for further use
			SplashLatitude = res.latitude;
			SplashLongitude = res.longitude;
			calcParams.TEI = res.P30TIG;
			calcParams.EI = res.GET400K;
		}

		if (fcn == 133)
		{
			char buffer1[1000];
			char buffer2[1000];

			TimeofIgnition = res.P30TIG;
			DeltaV_LVLH = res.dV_LVLH;

			AGCStateVectorUpdate(buffer1, sv0, true, AGCEpoch, GETbase, true);
			AGCExternalDeltaVUpdate(buffer2, TimeofIgnition, DeltaV_LVLH);

			sprintf(uplinkdata, "%s%s", buffer1, buffer2);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "State vectors, target load");
			}
		}
	}
	break;
	case 40: //REV 1 MAP UPDATE
	{
		SV sv0, sv1, sv2;
		AP10MAPUPDATE upd_hyper, upd_ellip;

		AP10MAPUPDATE * form = (AP10MAPUPDATE *)pad;

		sv0 = StateVectorCalc(calcParams.src);
		LunarOrbitMapUpdate(sv0, getGETBase(), upd_hyper);

		sv1 = ExecuteManeuver(calcParams.src, getGETBase(), TimeofIgnition, DeltaV_LVLH, sv0, GetDockedVesselMass(calcParams.src));
		sv2 = coast(sv1, -30.0*60.0);
		LunarOrbitMapUpdate(sv2, getGETBase(), upd_ellip);

		form->Rev = 1;
		form->AOSGET = upd_ellip.AOSGET;
		form->LOSGET = upd_hyper.LOSGET;
		form->PMGET = upd_hyper.PMGET;
	}
	break;
	case 41: //REV 2 MAP UPDATE
	case 43: //REV 4 MAP UPDATE
	case 44: //REV 11 MAP UPDATE
	case 45: //REV 22 MAP UPDATE
	case 46: //REV 23 MAP UPDATE
	case 47: //REV 24 MAP UPDATE
	case 48: //REV 25 MAP UPDATE
	case 49: //REV 26 MAP UPDATE
	case 140: //REV 27 MAP UPDATE
	case 141: //REV 29 MAP UPDATE
	case 142: //REV 30 MAP UPDATE
	case 143: //REV 31 MAP UPDATE
	{
		SV sv0, sv1;

		AP10MAPUPDATE * form = (AP10MAPUPDATE *)pad;

		sv0 = StateVectorCalc(calcParams.src);

		if (fcn == 45)
		{
			sv1 = coast(sv0, 2.0*4.0*3600.0);
		}
		else if (fcn == 141)
		{
			sv1 = coast(sv0, 1.0*2.0*3600.0);
		}
		else
		{
			sv1 = sv0;
		}

		LunarOrbitMapUpdate(sv1, getGETBase(), *form);

		if (fcn == 41)
		{
			form->Rev = 2;
		}
		else if (fcn == 43)
		{
			form->Rev = 4;
		}
		else if (fcn == 44)
		{
			form->Rev = 11;
		}
		else if (fcn == 45 || fcn == 46 || fcn == 47 || fcn == 48 || fcn == 49)
		{
			form->Rev = fcn - 23;
		}
		else if (fcn == 140)
		{
			form->Rev = 27;
		}
		else if (fcn == 141)
		{
			form->Rev = 29;
		}
		else if (fcn == 142)
		{
			form->Rev = 30;
		}
		else if (fcn == 143)
		{
			form->Rev = 31;
		}
	}
	break;
	case 42: //REV 3 MAP UPDATE
	{
		SV sv0, sv1;
		AP10MAPUPDATE upd_preloi, upd_postloi;

		AP10MAPUPDATE * form = (AP10MAPUPDATE *)pad;

		sv0 = StateVectorCalc(calcParams.src);
		LunarOrbitMapUpdate(sv0, getGETBase(), upd_preloi);

		sv1 = ExecuteManeuver(calcParams.src, getGETBase(), TimeofIgnition, DeltaV_LVLH, sv0, GetDockedVesselMass(calcParams.src));
		LunarOrbitMapUpdate(sv0, getGETBase(), upd_postloi);

		form->Rev = 3;
		form->AOSGET = upd_postloi.AOSGET;
		form->LOSGET = upd_preloi.LOSGET;
		form->PMGET = upd_preloi.PMGET;
	}
	break;
	case 144: //TEI MAP UPDATE
	{
		SV sv0, sv1;
		AP10MAPUPDATE upd_pretei, upd_posttei;

		AP10MAPUPDATE * form = (AP10MAPUPDATE *)pad;

		sv0 = StateVectorCalc(calcParams.src);
		LunarOrbitMapUpdate(sv0, getGETBase(), upd_pretei);

		sv1 = ExecuteManeuver(calcParams.src, getGETBase(), TimeofIgnition, DeltaV_LVLH, sv0, GetDockedVesselMass(calcParams.src));
		LunarOrbitMapUpdate(sv1, getGETBase(), upd_posttei);

		form->Rev = 32;
		form->AOSGET = upd_posttei.AOSGET;
		form->LOSGET = upd_pretei.LOSGET;
		form->PMGET = upd_pretei.PMGET;
	}
	break;
	case 50: //REV 4 LANDMARK TRACKING PAD F-1
	case 51: //REV 4 LANDMARK TRACKING PAD B-1
	case 52: //REV 11 LANDMARK TRACKING PAD LLS-2
	case 53: //REV 24 LANDMARK TRACKING PADs
	case 54: //REV 25 LANDMARK TRACKING PADs
	case 55: //REV 26 LANDMARK TRACKING PADs
	case 56: //REV 27 LANDMARK TRACKING PADs
	case 57: //REV 30 LANDMARK TRACKING PADs
	{
		LMARKTRKPADOpt opt;

		AP11LMARKTRKPAD * form = (AP11LMARKTRKPAD *)pad;

		opt.GETbase = getGETBase();
		opt.vessel = calcParams.src;

		if (fcn == 50)
		{
			sprintf(form->LmkID[0], "F-1");
			opt.alt[0] = 0;
			opt.lat[0] = 1.6*RAD;
			opt.LmkTime[0] = OrbMech::HHMMSSToSS(82, 27, 0);
			opt.lng[0] = 86.88*RAD;
			opt.entries = 1;
		}
		else if (fcn == 51)
		{
			sprintf(form->LmkID[0], "B-1");
			opt.alt[0] = -1.54*1852.0;
			opt.lat[0] = 2.522*RAD;
			opt.LmkTime[0] = OrbMech::HHMMSSToSS(82, 45, 0);
			opt.lng[0] = 35.036*RAD;
			opt.entries = 1;
		}
		else if (fcn == 52)
		{
			sprintf(form->LmkID[0], "130");
			opt.alt[0] = -1.73*1852.0;
			opt.lat[0] = 1.266*RAD;
			opt.LmkTime[0] = OrbMech::HHMMSSToSS(96, 35, 0);
			opt.lng[0] = 23.678*RAD;
			opt.entries = 1;
		}
		else if (fcn == 53 || fcn == 54 || fcn == 55 || fcn == 56)
		{
			if (fcn == 53)
			{
				opt.LmkTime[0] = OrbMech::HHMMSSToSS(121, 26, 0);
				opt.LmkTime[1] = OrbMech::HHMMSSToSS(121, 41, 0);
				opt.LmkTime[2] = OrbMech::HHMMSSToSS(121, 54, 0);
				opt.LmkTime[3] = OrbMech::HHMMSSToSS(122, 15, 0);
			}
			else if (fcn == 54)
			{
				opt.LmkTime[0] = OrbMech::HHMMSSToSS(123, 24, 0);
				opt.LmkTime[1] = OrbMech::HHMMSSToSS(123, 39, 0);
				opt.LmkTime[2] = OrbMech::HHMMSSToSS(123, 52, 0);
				opt.LmkTime[3] = OrbMech::HHMMSSToSS(124, 13, 0);
			}
			else if (fcn == 55)
			{
				opt.LmkTime[0] = OrbMech::HHMMSSToSS(125, 22, 0);
				opt.LmkTime[1] = OrbMech::HHMMSSToSS(125, 37, 0);
				opt.LmkTime[2] = OrbMech::HHMMSSToSS(125, 50, 0);
				opt.LmkTime[3] = OrbMech::HHMMSSToSS(126, 11, 0);
			}
			else if (fcn == 56)
			{
				opt.LmkTime[0] = OrbMech::HHMMSSToSS(127, 20, 0);
				opt.LmkTime[1] = OrbMech::HHMMSSToSS(127, 35, 0);
				opt.LmkTime[2] = OrbMech::HHMMSSToSS(127, 48, 0);
				opt.LmkTime[3] = OrbMech::HHMMSSToSS(128, 9, 0);
			}

			sprintf(form->LmkID[0], "CP-1");
			opt.alt[0] = 0.0;
			opt.lat[0] = 0.875*RAD;
			opt.lng[0] = 170.146*RAD;

			sprintf(form->LmkID[1], "CP-2");
			opt.alt[1] = 0.0;
			opt.lat[1] = 1.0*RAD;
			opt.lng[1] = 127.4*RAD;

			sprintf(form->LmkID[2], "F-1");
			opt.alt[2] = 0.0;
			opt.lat[2] = 1.6*RAD;
			opt.lng[2] = 86.88*RAD;

			sprintf(form->LmkID[3], "130");
			opt.alt[3] = -1.73*1852.0;
			opt.lat[3] = 1.266*RAD;
			opt.lng[3] = 23.678*RAD;

			opt.entries = 4;
		}
		else if (fcn == 57)
		{
			sprintf(form->LmkID[0], "B-1");
			opt.alt[0] = -1.54*1852.0;
			opt.lat[0] = 2.522*RAD;
			opt.LmkTime[0] = OrbMech::HHMMSSToSS(134, 0, 0);
			opt.lng[0] = 35.036*RAD;

			sprintf(form->LmkID[1], "150");
			opt.alt[1] = -1.05*1852.0;
			opt.lat[1] = 0.283*RAD;
			opt.LmkTime[1] = OrbMech::HHMMSSToSS(134, 12, 0);
			opt.lng[1] = -1.428*RAD;

			opt.entries = 2;
		}

		LandmarkTrackingPAD(&opt, *form);
	}
	break;
	case 60: //STATE VECTOR and LLS 2 REFSMMAT UPLINK
	{
		MATRIX3 REFSMMAT;
		VECTOR3 DV;
		double GETbase, t_PDI, t_land, CR;
		SV sv;
		REFSMMATOpt opt;
		DOIMan doiopt;
		char buffer1[1000];
		char buffer2[1000];

		sv = StateVectorCalc(calcParams.src); //State vector for uplink
		GETbase = getGETBase();

		doiopt.alt = LSAlt;
		doiopt.EarliestGET = OrbMech::HHMMSSToSS(99, 0, 0);
		doiopt.GETbase = GETbase;
		doiopt.lat = LSLat;
		doiopt.lng = LSLng;
		doiopt.N = 0;
		doiopt.opt = 0;
		doiopt.sv0 = sv;

		DOITargeting(&doiopt, DV, TimeofIgnition, t_PDI, t_land, CR);
		calcParams.DOI = TimeofIgnition;
		calcParams.TLAND = t_land;

		opt.GETbase = GETbase;
		opt.LSLat = LSLat;
		opt.LSLng = LSLng;
		opt.REFSMMATopt = 5;
		opt.REFSMMATTime = calcParams.TLAND;
		opt.vessel = calcParams.src;

		REFSMMAT = REFSMMATCalc(&opt);

		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
		AGCDesiredREFSMMATUpdate(buffer2, REFSMMAT, AGCEpoch);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector, LLS2 REFSMMAT");
		}
	}
	break;
	case 61: //CSM DAP DATA
	{
		AP10DAPDATA * form = (AP10DAPDATA *)pad;

		CSMDAPUpdate(calcParams.src, *form);
	}
	break;
	case 62: //LM DAP DATA
	{
		AP10DAPDATA * form = (AP10DAPDATA *)pad;

		LMDAPUpdate(calcParams.tgt, *form);
	}
	break;
	case 63: //GYRO TORQUING ANGLES
	{
		TORQANG * form = (TORQANG *)pad;
		LEM *lem = (LEM *)calcParams.tgt;

		VECTOR3 lmn20, csmn20, V42angles;

		csmn20.x = calcParams.src->imu.Gimbal.X;
		csmn20.y = calcParams.src->imu.Gimbal.Y;
		csmn20.z = calcParams.src->imu.Gimbal.Z;

		lmn20.x = lem->imu.Gimbal.X;
		lmn20.y = lem->imu.Gimbal.Y;
		lmn20.z = lem->imu.Gimbal.Z;

		V42angles = OrbMech::LMDockedFineAlignment(lmn20, csmn20);

		form->V42Angles.x = V42angles.x*DEG;
		form->V42Angles.y = V42angles.y*DEG;
		form->V42Angles.z = V42angles.z*DEG;
	}
	break;
	case 64: //LGC ACTIVATION UPDATE
	{
		SV sv;
		REFSMMATOpt opt;
		MATRIX3 REFSMMAT;
		double GETbase, TEPHEM0, tephem, t_AGC, t_actual, deltaT;
		LEM *lem;
		char clockupdate[128];
		char buffer1[1000];
		char buffer2[1000];
		char buffer3[1000];

		sv = StateVectorCalc(calcParams.src); //State vector for uplink
		GETbase = getGETBase();
		lem = (LEM *)calcParams.tgt;
		TEPHEM0 = 40038.;

		tephem = GetTEPHEMFromAGC(&lem->agc.vagc);
		t_AGC = GetClockTimeFromAGC(&lem->agc.vagc);

		tephem = (tephem / 8640000.) + TEPHEM0;
		t_actual = (oapiGetSimMJD() - tephem) * 8640000.;
		deltaT = t_actual - t_AGC;

		IncrementAGCTime(clockupdate, deltaT);

		opt.GETbase = GETbase;
		opt.LSLat = LSLat;
		opt.LSLng = LSLng;
		opt.REFSMMATopt = 5;
		opt.REFSMMATTime = calcParams.TLAND;
		opt.vessel = calcParams.src;

		REFSMMAT = REFSMMATCalc(&opt);

		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
		AGCStateVectorUpdate(buffer2, sv, false, AGCEpoch, GETbase);
		AGCREFSMMATUpdate(buffer3, REFSMMAT, AGCEpoch, LGCREFSAddrOffs);

		sprintf(uplinkdata, "%s%s%s%s", clockupdate, buffer1, buffer2, buffer3);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "clock update, state vectors, LS REFSMMAT");
		}
	}
	break;
	case 70: //CSM SEPARATION BURN
	{
		AP11ManPADOpt opt;
		SV sv;
		VECTOR3 dV_LVLH;
		double GETbase, t_P, mu, t_Sep;
		char buffer1[1000];
		char buffer2[1000];

		AP11MNV * form = (AP11MNV *)pad;

		sv = StateVectorCalc(calcParams.src); //State vector for uplink
		GETbase = getGETBase();
		mu = GGRAV * oapiGetMass(sv.gravref);

		t_P = OrbMech::period(sv.R, sv.V, mu);
		t_Sep = floor(calcParams.DOI - t_P / 2.0);
		dV_LVLH = _V(0, 0, -2.5)*0.3048;

		opt.alt = LSAlt;
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_RCS;
		opt.GETbase = GETbase;
		opt.HeadsUp = true;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.TIG = t_Sep;
		opt.vessel = calcParams.src;
		opt.vesseltype = 0;

		AP11ManeuverPAD(&opt, *form);

		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
		AGCStateVectorUpdate(buffer2, sv, false, AGCEpoch, GETbase);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "state vectors");
		}
	}
	break;
	case 71: //DESCENT ORBIT INSERTION
	{
		AP11LMManPADOpt opt;

		VECTOR3 DV;
		double GETbase, t_PDI, t_land, CR, t_DOI_imp, t_TPI_guess;
		SV sv_CSM, sv, sv_DOI;
		DOIMan doiopt;
		char GETbuffer[64];
		char TLANDbuffer[64];
		char buffer1[1000];
		char buffer2[1000];

		AP11LMMNV * form = (AP11LMMNV *)pad;

		sv_CSM = StateVectorCalc(calcParams.src);
		sv = StateVectorCalc(calcParams.tgt);
		GETbase = getGETBase();

		doiopt.alt = LSAlt;
		doiopt.EarliestGET = OrbMech::HHMMSSToSS(99, 0, 0);
		doiopt.GETbase = GETbase;
		doiopt.lat = LSLat;
		doiopt.lng = LSLng;
		doiopt.N = 0;
		doiopt.opt = 0;
		doiopt.sv0 = sv;

		DOITargeting(&doiopt, DV, t_DOI_imp, t_PDI, t_land, CR);

		calcParams.DOI = t_DOI_imp;
		calcParams.TLAND = t_land;

		PoweredFlightProcessor(sv, GETbase, t_DOI_imp, RTCC_VESSELTYPE_LM, RTCC_ENGINETYPE_SPSDPS, 0.0, DV, false, TimeofIgnition, DeltaV_LVLH);

		opt.alt = LSAlt;
		opt.csmlmdocked = false;
		opt.dV_LVLH = DeltaV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = true;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, AGCEpoch, LGCREFSAddrOffs);
		opt.TIG = TimeofIgnition;
		opt.vessel = calcParams.tgt;

		AP11LMManeuverPAD(&opt, *form);
		sprintf(form->purpose, "DOI");

		//Rendezvous Plan
		double MJD_Phasing;

		sv_DOI = ExecuteManeuver(calcParams.tgt, GETbase, TimeofIgnition, DeltaV_LVLH, sv, 0.0);
		MJD_Phasing = OrbMech::P29TimeOfLongitude(sv_DOI.R, sv_DOI.V, sv_DOI.MJD, sv_DOI.gravref, -12.5*RAD);
		calcParams.Phasing = (MJD_Phasing - GETbase)*24.0*3600.0;

		t_TPI_guess = OrbMech::HHMMSSToSS(105, 9, 0);
		calcParams.TPI = FindOrbitalMidnight(sv_CSM, GETbase, t_TPI_guess);

		FMissionRendezvousPlan(calcParams.tgt, calcParams.src, sv_DOI, GETbase, calcParams.Phasing, calcParams.TPI, calcParams.Insertion, calcParams.CSI);

		OrbMech::format_time_HHMMSS(GETbuffer, calcParams.CSI);
		sprintf(form->remarks, "CSI time: %s, ", GETbuffer);
		OrbMech::format_time_HHMMSS(GETbuffer, calcParams.TPI);
		sprintf(form->remarks, "%sTPI time: %s, N equal to 1", form->remarks, GETbuffer);

		AGCStateVectorUpdate(buffer1, sv, false, AGCEpoch, GETbase);
		AGCExternalDeltaVUpdate(buffer2, TimeofIgnition, DeltaV_LVLH, LGCDeltaVAddr);
		TLANDUpdate(TLANDbuffer, calcParams.TLAND, 2400);

		sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, TLANDbuffer);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "LM state vector, DOI target load");
		}
	}
	break;
	case 72: //PRELIMINARY PHASING MANEUVER
		preliminary = true;
	case 73: //PHASING MANEUVER
	{
		AP11LMManPADOpt opt;
		LambertMan lamopt;
		TwoImpulseResuls res;
		SV sv_CSM, sv_LM, sv_DOI;
		VECTOR3 dV_LVLH;
		double GETbase, MJD_LS, t_LS, P30TIG, MJD_100E, t_100E;
		char GETbuffer[64];
		char GETbuffer2[64];

		AP11LMMNV * form = (AP11LMMNV *)pad;
		GETbase = getGETBase();

		sv_CSM = StateVectorCalc(calcParams.src);
		sv_LM = StateVectorCalc(calcParams.tgt);

		if (preliminary)
		{
			sv_DOI = ExecuteManeuver(calcParams.tgt, GETbase, TimeofIgnition, DeltaV_LVLH, sv_LM, 0.0);
		}
		else
		{
			sv_DOI = sv_LM;
		}

		lamopt.axis = RTCC_LAMBERT_MULTIAXIS;
		lamopt.GETbase = GETbase;
		lamopt.N = 0;
		lamopt.Offset = _V(-270.0*1852.0, 0.0, 60.0*1852.0 - 60000.0*0.3048);
		lamopt.Perturbation = RTCC_LAMBERT_PERTURBED;
		lamopt.sv_A = sv_DOI;
		lamopt.sv_P = sv_CSM;
		lamopt.T1 = calcParams.Phasing;
		lamopt.T2 = calcParams.Insertion;

		LambertTargeting(&lamopt, res);
		PoweredFlightProcessor(sv_DOI, GETbase, lamopt.T1, RTCC_VESSELTYPE_LM, RTCC_ENGINETYPE_SPSDPS, 0.0, res.dV, false, P30TIG, dV_LVLH);

		opt.alt = LSAlt;
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, AGCEpoch, LGCREFSAddrOffs);
		opt.RV_MCC = sv_DOI;
		opt.TIG = P30TIG;
		opt.useSV = true;
		opt.vessel = calcParams.tgt;

		AP11LMManeuverPAD(&opt, *form);
		sprintf(form->purpose, "Phasing");

		if (preliminary)
		{
			MJD_LS = OrbMech::P29TimeOfLongitude(sv_DOI.R, sv_DOI.V, sv_DOI.MJD, sv_DOI.gravref, LSLng);
			t_LS = (MJD_LS - GETbase)*24.0*3600.0;
			MJD_100E = OrbMech::P29TimeOfLongitude(sv_DOI.R, sv_DOI.V, sv_DOI.MJD, sv_DOI.gravref, 100.0*RAD);
			t_100E = (MJD_100E - GETbase)*24.0*3600.0;

			OrbMech::format_time_MMSS(GETbuffer, P30TIG - t_100E);
			OrbMech::format_time_MMSS(GETbuffer2, P30TIG - t_LS);
			sprintf(form->remarks, "100-degree east time is %s. Site 2 time is %s", GETbuffer, GETbuffer2);
		}
	}
	break;
	case 74: //PDI ABORT MANEUVER
	{
		AP11LMManPADOpt opt;
		DKIOpt dkiopt;
		DKIResults dkires;
		SV sv_LM, sv_CSM, sv_DOI, sv_Phasing;
		VECTOR3 dV_LVLH;
		double GETbase, dt_peri, t_Abort, t_TPI_guess, t_TPI_Abort, P30TIG;
		char GETbuffer[64], GETbuffer2[64];

		AP11LMMNV * form = (AP11LMMNV *)pad;
		GETbase = getGETBase();

		sv_CSM = StateVectorCalc(calcParams.src);
		sv_LM = StateVectorCalc(calcParams.tgt);
		sv_DOI = ExecuteManeuver(calcParams.tgt, GETbase, TimeofIgnition, DeltaV_LVLH, sv_LM, 0.0);

		dt_peri = OrbMech::timetoperi_integ(sv_DOI.R, sv_DOI.V, sv_DOI.MJD, sv_DOI.gravref, sv_DOI.gravref);
		t_Abort = (sv_DOI.MJD - GETbase)*24.0*3600.0 + dt_peri;

		t_TPI_guess = OrbMech::HHMMSSToSS(103, 9, 0);
		t_TPI_Abort = FindOrbitalMidnight(sv_CSM, GETbase, t_TPI_guess);

		dkiopt.DH = 15.0*1852.0;
		dkiopt.E = 26.6*RAD;
		dkiopt.GETbase = GETbase;
		dkiopt.sv_A = sv_DOI;
		dkiopt.sv_P = sv_CSM;
		dkiopt.t_TIG = t_Abort;
		dkiopt.t_TPI_guess = t_TPI_Abort;

		DockingInitiationProcessor(dkiopt, dkires);
		PoweredFlightProcessor(sv_DOI, GETbase, t_Abort, 1, 1, 0.0, dkires.DV_Phasing, false, P30TIG, dV_LVLH);

		opt.alt = LSAlt;
		opt.csmlmdocked = false;
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, AGCEpoch, LGCREFSAddrOffs);
		opt.TIG = P30TIG;
		opt.vessel = calcParams.tgt;
		opt.useSV = true;
		opt.RV_MCC = sv_DOI;

		AP11LMManeuverPAD(&opt, *form);
		sprintf(form->purpose, "PDI Abort");

		OrbMech::format_time_HHMMSS(GETbuffer, dkires.t_CSI);
		OrbMech::format_time_HHMMSS(GETbuffer2, t_TPI_Abort);
		sprintf(form->remarks, "CSI time: %s, TPI time: %s, N equal to 1", GETbuffer, GETbuffer2);
	}
	break;
	case 75: //PRELIMINARY CSM BACKUP INSERTION UPDATE
		preliminary = true;
	case 76: //CSM BACKUP INSERTION UPDATE
	{
		AP11ManPADOpt opt;
		LambertMan lamopt;
		TwoImpulseResuls res;
		SV sv_CSM, sv_LM, sv_Ins;
		VECTOR3 dV_LVLH;
		double GETbase, P30TIG;

		AP11MNV * form = (AP11MNV *)pad;

		sv_CSM = StateVectorCalc(calcParams.src);
		sv_LM = StateVectorCalc(calcParams.tgt);
		GETbase = getGETBase();

		lamopt.axis = RTCC_LAMBERT_MULTIAXIS;
		lamopt.GETbase = GETbase;
		lamopt.N = 0;
		lamopt.Offset = -_V(-110.0*1852.0, 0.0, 14.7*1852.0);
		lamopt.Perturbation = RTCC_LAMBERT_PERTURBED;
		lamopt.sv_A = sv_CSM;
		lamopt.sv_P = sv_LM;
		lamopt.T1 = calcParams.Insertion + 3.0*60.0;
		lamopt.T2 = calcParams.CSI;

		LambertTargeting(&lamopt, res);
		PoweredFlightProcessor(sv_CSM, GETbase, lamopt.T1, RTCC_VESSELTYPE_CSM, RTCC_ENGINETYPE_SPSDPS, 0.0, res.dV, false, P30TIG, dV_LVLH);

		opt.alt = LSAlt;
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_SPSDPS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		opt.TIG = P30TIG;
		opt.vessel = calcParams.src;
		opt.vesseltype = 0;

		AP11ManeuverPAD(&opt, *form);
		sprintf(form->purpose, "Backup Insertion");

		sv_Ins = ExecuteManeuver(calcParams.src, GETbase, P30TIG, dV_LVLH, sv_CSM, 0.0);

		SPQOpt coeopt;
		SPQResults coeres;
		char GETbuffer[64], GETbuffer2[64];

		coeopt.DH = -15.0*1852.0;
		coeopt.E = 208.3*RAD;
		coeopt.GETbase = GETbase;
		coeopt.maneuver = 0;
		coeopt.sv_A = sv_Ins;
		coeopt.sv_P = sv_LM;
		coeopt.type = 1;
		coeopt.t_TIG = calcParams.CSI;

		ConcentricRendezvousProcessor(&coeopt, coeres);

		OrbMech::format_time_HHMMSS(GETbuffer, calcParams.CSI);
		OrbMech::format_time_HHMMSS(GETbuffer2, coeres.t_TPI);
		sprintf(form->remarks, "CSI: %s, TPI: %s, N equals 1", GETbuffer, GETbuffer2);

		if (preliminary == false)
		{
			char buffer1[1000];

			AGCStateVectorUpdate(buffer1, sv_CSM, true, AGCEpoch, GETbase);

			sprintf(uplinkdata, "%s", buffer1);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector");
			}
		}
	}
	break;
	case 77: //PRELIMINARY LM INSERTION UPDATE
		preliminary = true;
	case 78: //LM INSERTION UPDATE
	{
		AP11LMManPADOpt opt;
		LambertMan lamopt;
		TwoImpulseResuls res;
		SV sv_CSM, sv_LM;
		VECTOR3 dV_LVLH;
		double GETbase, P30TIG;

		AP11LMMNV * form = (AP11LMMNV *)pad;

		sv_CSM = StateVectorCalc(calcParams.src);
		sv_LM = StateVectorCalc(calcParams.tgt);
		GETbase = getGETBase();

		//Without descent stage
		sv_LM.mass -= 2224.0 + 8000.0;

		lamopt.axis = RTCC_LAMBERT_MULTIAXIS;
		lamopt.GETbase = GETbase;
		lamopt.N = 0;
		lamopt.Offset = _V(-147.0*1852.0, 0.0, 14.7*1852.0);
		lamopt.Perturbation = RTCC_LAMBERT_PERTURBED;
		lamopt.sv_A = sv_LM;
		lamopt.sv_P = sv_CSM;
		lamopt.T1 = calcParams.Insertion;
		lamopt.T2 = calcParams.CSI;

		LambertTargeting(&lamopt, res);
		PoweredFlightProcessor(sv_LM, GETbase, lamopt.T1, RTCC_VESSELTYPE_LM, RTCC_ENGINETYPE_APS, 0.0, res.dV, false, P30TIG, dV_LVLH);

		opt.alt = LSAlt;
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_APS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, AGCEpoch, LGCREFSAddrOffs);
		opt.RV_MCC = sv_LM;
		opt.TIG = P30TIG;
		opt.useSV = true;
		opt.vessel = calcParams.tgt;

		AP11LMManeuverPAD(&opt, *form);
		sprintf(form->purpose, "Insertion");

		if (preliminary == false)
		{
			char buffer1[1000];

			sprintf(form->remarks, "LM ascent stage weight is %.0lf", form->LMWeight);

			AGCStateVectorUpdate(buffer1, sv_CSM, true, AGCEpoch, GETbase);
			sprintf(uplinkdata, "%s", buffer1);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "CSM state vector");
			}
		}
	}
	break;
	case 79: //CSI UPDATE
	{
		AP10CSIPADOpt manopt;
		SPQOpt opt;
		SPQResults res;
		SV sv_CSM, sv_LM, sv_CSI;
		MATRIX3 Q_Xx;
		VECTOR3 dV_LVLH;
		double GETbase, dt_apo, mu;

		AP10CSI * form = (AP10CSI *)pad;

		sv_CSM = StateVectorCalc(calcParams.src);
		sv_LM = StateVectorCalc(calcParams.tgt);
		GETbase = getGETBase();

		//CSI at apolune
		mu = GGRAV * oapiGetMass(sv_LM.gravref);
		dt_apo = OrbMech::timetoapo(sv_LM.R, sv_LM.V, mu);
		calcParams.CSI = OrbMech::GETfromMJD(sv_LM.MJD, GETbase) + dt_apo;

		opt.DH = 15.0*1852.0;
		opt.E = 26.6*RAD;
		opt.GETbase = GETbase;
		opt.maneuver = 0;
		opt.sv_A = sv_LM;
		opt.sv_P = sv_CSM;
		opt.type = 0;
		opt.t_TIG = calcParams.CSI;
		opt.t_TPI = calcParams.TPI;

		ConcentricRendezvousProcessor(&opt, res);

		sv_CSI = coast(sv_LM, opt.t_TIG - OrbMech::GETfromMJD(sv_LM.MJD, GETbase));
		Q_Xx = OrbMech::LVLH_Matrix(sv_CSI.R, sv_CSI.V);
		dV_LVLH = res.dV_CSI;

		manopt.dV_LVLH = dV_LVLH;
		manopt.enginetype = RTCC_ENGINETYPE_APS;
		manopt.GETbase = GETbase;
		manopt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, AGCEpoch, LGCREFSAddrOffs);
		manopt.sv0 = sv_LM;
		manopt.t_CSI = calcParams.CSI;
		manopt.t_TPI = calcParams.TPI;

		AP10CSIPAD(&manopt, *form);
	}
	break;
	case 80: //APS DEPLETION UPDATE
	{
		AP11LMManPADOpt opt;
		SV sv, sv1, sv2;
		MATRIX3 Q_Xx;
		VECTOR3 UX, UY, UZ, DV, DV_P, DV_C, V_G, dV_LVLH;
		double GETbase, MJD_depletion, t_Depletion_guess, t_Depletion, dv, theta_T;
		char buffer1[1000];

		AP11LMMNV * form = (AP11LMMNV *)pad;

		GETbase = getGETBase();
		t_Depletion_guess = OrbMech::HHMMSSToSS(108, 0, 0);
		dv = 4600.0*0.3048;

		sv = StateVectorCalc(calcParams.tgt);
		sv1 = coast(sv, t_Depletion_guess - OrbMech::GETfromMJD(sv.MJD, GETbase));

		MJD_depletion = OrbMech::P29TimeOfLongitude(sv1.R, sv1.V, sv1.MJD, sv1.gravref, 0.0);
		t_Depletion = OrbMech::GETfromMJD(MJD_depletion, GETbase);
		sv2 = coast(sv1, t_Depletion - t_Depletion_guess);

		UY = unit(crossp(sv2.V, sv2.R));
		UZ = unit(-sv2.R);
		UX = crossp(UY, UZ);
		Q_Xx = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		DV = UX * dv;
		DV_P = UX * dv;

		theta_T = -length(crossp(sv2.R, sv2.V))*dv*sv2.mass / OrbMech::power(length(sv2.R), 2.0) / APS_THRUST;
		DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
		V_G = DV_C;
		dV_LVLH = mul(Q_Xx, V_G);

		opt.alt = LSAlt;
		opt.dV_LVLH = dV_LVLH;
		opt.enginetype = RTCC_ENGINETYPE_APS;
		opt.GETbase = GETbase;
		opt.HeadsUp = false;
		opt.REFSMMAT = GetREFSMMATfromAGC(&mcc->lm->agc.vagc, AGCEpoch, LGCREFSAddrOffs);
		opt.TIG = t_Depletion;
		opt.vessel = calcParams.tgt;

		AP11LMManeuverPAD(&opt, *form);

		AGCStateVectorUpdate(buffer1, sv, false, AGCEpoch, GETbase);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "LM state vector");
		}
	}
	break;
	case 90: //MCC-5 UPDATE
	case 91: //PRELIMINARY MCC-6 UPDATE
	case 92: //MCC-6 UPDATE
	case 93: //MCC-7 DECISION
	case 94: //MCC-7 UPDATE
	{
		EntryOpt entopt;
		EntryResults res;
		AP11ManPADOpt opt;
		double GETbase, MCCtime;
		MATRIX3 REFSMMAT;
		char manname[8];
		SV sv;

		AP11MNV * form = (AP11MNV *)pad;

		//Just so things don't break
		if (calcParams.TEI == 0)
		{
			calcParams.TEI = OrbMech::HHMMSSToSS(137, 20, 0);
		}
		if (calcParams.EI == 0)
		{
			calcParams.EI = OrbMech::HHMMSSToSS(191, 50, 0);
		}

		if (fcn == 90)
		{
			MCCtime = calcParams.TEI + 15.0*3600.0;
			sprintf(manname, "MCC-5");
		}
		else if (fcn == 91 || fcn == 92)
		{
			MCCtime = calcParams.EI - 15.0*3600.0;
			sprintf(manname, "MCC-6");
		}
		else if (fcn == 93 || fcn == 94)
		{
			MCCtime = calcParams.EI - 2.0*3600.0;
			sprintf(manname, "MCC-7");
		}

		//Only corridor control after EI-24h
		if (MCCtime > calcParams.EI - 24.0*3600.0)
		{
			entopt.type = RTCC_ENTRY_CORRIDOR;
		}
		else
		{
			entopt.type = RTCC_ENTRY_MCC;
		}

		GETbase = getGETBase();

		sv = StateVectorCalc(calcParams.src); //State vector for uplink

		entopt.entrylongmanual = true;
		entopt.GETbase = GETbase;
		entopt.impulsive = RTCC_NONIMPULSIVE;
		entopt.lng = -165.0*RAD;
		entopt.ReA = 0;
		entopt.TIGguess = MCCtime;
		entopt.vessel = calcParams.src;

		EntryTargeting(&entopt, &res);//dV_LVLH, P30TIG, latitude, longitude, RET, RTGO, VIO, ReA, prec); //Target Load for uplink

									  //Apollo 10 Mission Rules
		if (MCCtime > calcParams.EI - 50.0*3600.0)
		{
			if (length(res.dV_LVLH) < 1.0*0.3048)
			{
				scrubbed = true;
			}
		}
		else
		{
			if (length(res.dV_LVLH) < 2.0*0.3048)
			{
				scrubbed = true;
			}
		}

		if (fcn == 94)
		{
			REFSMMATOpt refsopt;
			refsopt.GETbase = GETbase;
			refsopt.dV_LVLH = res.dV_LVLH;
			refsopt.P30TIG = res.P30TIG;
			refsopt.REFSMMATdirect = false;
			refsopt.REFSMMATopt = 3;
			refsopt.vessel = calcParams.src;

			REFSMMAT = REFSMMATCalc(&refsopt);
		}
		else
		{
			REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		}

		if (scrubbed)
		{
			//Entry prediction without maneuver
			EntryUpdateCalc(sv, entopt.GETbase, 0, true, &res);

			res.dV_LVLH = _V(0, 0, 0);
			res.P30TIG = entopt.TIGguess;
		}
		else
		{
			opt.dV_LVLH = res.dV_LVLH;
			opt.enginetype = SPSRCSDecision(SPS_THRUST / calcParams.src->GetMass(), res.dV_LVLH);
			opt.GETbase = GETbase;
			opt.HeadsUp = false;
			opt.REFSMMAT = REFSMMAT;
			opt.TIG = res.P30TIG;
			opt.vessel = calcParams.src;
			opt.vesseltype = 0;

			AP11ManeuverPAD(&opt, *form);
			sprintf(form->purpose, manname);
			form->lat = res.latitude*DEG;
			form->lng = res.longitude*DEG;
			form->RTGO = res.RTGO;
			form->VI0 = res.VIO / 0.3048;
			form->GET05G = res.GET05G;
		}

		if (scrubbed)
		{
			//Scrubbed MCC-5 and MCC-6
			if (fcn == 90 || fcn == 91 || fcn == 92)
			{
				char buffer1[1000];
				char buffer2[1000];

				sprintf(upMessage, "%s has been scrubbed", manname);
				sprintf(upDesc, "CSM state vector, entry target");

				AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
				CMCEntryUpdate(buffer2, res.latitude, res.longitude);

				sprintf(uplinkdata, "%s%s", buffer1, buffer2);
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
				}
			}
			//MCC-7 decision
			else if (fcn == 93)
			{
				sprintf(upMessage, "%s has been scrubbed", manname);
			}
			//Scrubbed MCC-7
			else if (fcn == 94)
			{
				char buffer1[1000];
				char buffer2[1000];
				char buffer3[1000];

				sprintf(upDesc, "%s has been scrubbed", manname);
				sprintf(upDesc, "CSM state vector, entry target, Entry REFSMMAT");

				AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
				CMCEntryUpdate(buffer2, res.latitude, res.longitude);
				AGCDesiredREFSMMATUpdate(buffer3, REFSMMAT, AGCEpoch);

				sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
				}
			}
		}
		else
		{
			//MCC-5 and MCC-6
			if (fcn == 90 || fcn == 91 || fcn == 92)
			{
				char buffer1[1000];
				char buffer2[1000];

				AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
				CMCRetrofireExternalDeltaVUpdate(buffer2, res.latitude, res.longitude, res.P30TIG, res.dV_LVLH);

				sprintf(uplinkdata, "%s%s", buffer1, buffer2);
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "CSM state vector, target load");
				}
			}
			//MCC-7 decision
			else if (fcn == 93)
			{
				sprintf(upMessage, "%s will be executed", manname);
			}
			//MCC-7
			else if (fcn == 94)
			{
				char buffer1[1000];
				char buffer2[1000];
				char buffer3[1000];

				AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);
				CMCRetrofireExternalDeltaVUpdate(buffer2, res.latitude, res.longitude, res.P30TIG, res.dV_LVLH);
				AGCDesiredREFSMMATUpdate(buffer3, REFSMMAT, AGCEpoch);

				sprintf(uplinkdata, "%s%s%s", buffer1, buffer2, buffer3);
				if (upString != NULL) {
					// give to mcc
					strncpy(upString, uplinkdata, 1024 * 3);
					sprintf(upDesc, "CSM state vector, target load, Entry REFSMMAT");
				}
			}
		}

		//Save for further use
		calcParams.EI = res.GET400K;
		DeltaV_LVLH = res.dV_LVLH;
		TimeofIgnition = res.P30TIG;
		SplashLatitude = res.latitude;
		SplashLongitude = res.longitude;
	}
	break;
	case 96: //ENTRY PAD (ASSUMES NO MCC-6, but MCC-7)
	case 97: //ENTRY PAD (ASSUMES MCC-6)
	case 98: //ENTRY PAD (ASSUMES MCC-7)
	case 99: //FINAL LUNAR ENTRY PAD
	{
		AP11ENT * form = (AP11ENT *)pad;

		SV sv;
		LunarEntryPADOpt entopt;
		MATRIX3 REFSMMAT;
		double GETbase;

		GETbase = getGETBase();
		sv = StateVectorCalc(calcParams.src);

		if (length(DeltaV_LVLH) != 0.0 && fcn != 99)
		{
			entopt.direct = false;
		}
		else
		{
			entopt.direct = true;
		}

		if (fcn == 99)
		{
			REFSMMAT = GetREFSMMATfromAGC(&mcc->cm->agc.vagc, AGCEpoch);
		}
		else
		{
			REFSMMATOpt refsopt;
			refsopt.GETbase = GETbase;
			refsopt.dV_LVLH = DeltaV_LVLH;
			refsopt.P30TIG = TimeofIgnition;
			refsopt.REFSMMATdirect = false;
			refsopt.REFSMMATopt = 3;
			refsopt.vessel = calcParams.src;

			REFSMMAT = REFSMMATCalc(&refsopt);
		}

		entopt.dV_LVLH = DeltaV_LVLH;
		entopt.GETbase = GETbase;
		entopt.lat = SplashLatitude;
		entopt.lng = SplashLongitude;
		entopt.P30TIG = TimeofIgnition;
		entopt.REFSMMAT = REFSMMAT;
		entopt.vessel = calcParams.src;

		LunarEntryPAD(&entopt, *form);
		sprintf(form->Area[0], "MIDPAC");
		if (entopt.direct == false)
		{
			if (fcn == 97)
			{
				sprintf(form->remarks[0], "Assumes MCC6");
			}
			else if (fcn == 98)
			{
				sprintf(form->remarks[0], "Assumes MCC7");
			}
		}

		if (fcn == 99)
		{
			char buffer1[1000];
			char buffer2[1000];

			AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase, true);
			CMCEntryUpdate(buffer2, SplashLatitude, SplashLongitude);

			sprintf(uplinkdata, "%s%s", buffer1, buffer2);
			if (upString != NULL) {
				// give to mcc
				strncpy(upString, uplinkdata, 1024 * 3);
				sprintf(upDesc, "State vectors, entry update");
			}
		}
	}
	break;
	case 100: //GENERIC CSM STATE VECTOR UPDATE
	{
		SV sv;
		double GETbase;
		char buffer1[1000];

		sv = StateVectorCalc(calcParams.src); //State vector for uplink
		GETbase = getGETBase();

		AGCStateVectorUpdate(buffer1, sv, true, AGCEpoch, GETbase);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM state vector");
		}
	}
	break;
	case 101: //GENERIC CSM AND LM STATE VECTOR UPDATE
	{
		SV sv_CSM, sv_LM;
		double GETbase;
		char buffer1[1000];
		char buffer2[1000];

		sv_CSM = StateVectorCalc(calcParams.src);
		sv_LM = StateVectorCalc(calcParams.tgt);
		GETbase = getGETBase();

		AGCStateVectorUpdate(buffer1, sv_CSM, true, AGCEpoch, GETbase);
		AGCStateVectorUpdate(buffer2, sv_LM, false, AGCEpoch, GETbase);

		sprintf(uplinkdata, "%s%s", buffer1, buffer2);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "CSM and LM state vectors");
		}
	}
	break;
	case 102: //GENERIC LM STATE VECTOR UPDATE
	{
		SV sv;
		double GETbase;
		char buffer1[1000];

		sv = StateVectorCalc(calcParams.tgt); //State vector for uplink
		GETbase = getGETBase();

		AGCStateVectorUpdate(buffer1, sv, false, AGCEpoch, GETbase);

		sprintf(uplinkdata, "%s", buffer1);
		if (upString != NULL) {
			// give to mcc
			strncpy(upString, uplinkdata, 1024 * 3);
			sprintf(upDesc, "LM state vector");
		}
	}
	break;
	}

	return scrubbed;
}

void RTCC::FMissionRendezvousPlan(VESSEL *chaser, VESSEL *target, SV sv_A0, double GETbase, double t_TIG, double t_TPI, double &t_Ins, double &CSI)
{
	//Plan: Phasing (fixed TIG), Insertion, CSI at apolune, CDH, TPI at midnight (Apollo 10)

	LambertMan lamopt, lamopt2;
	TwoImpulseResuls lamres;
	double t_sv0, t_Phasing, t_Insertion, dt, t_CSI, dt2, mu, ddt, ddt2, T_P, DH, dv_CSI, t_CDH, dt_TPI, t_TPI_apo;
	VECTOR3 dV_Phasing, dV_Insertion, dV_CDH, DVX;
	MATRIX3 Q_Xx;
	SV sv_P0, sv_P_CSI, sv_Phasing, sv_Phasing_apo, sv_Insertion, sv_Insertion_apo, sv_CSI, sv_CSI_apo, sv_CDH, sv_CDH_apo, sv_P_CDH;

	mu = GGRAV * oapiGetMass(sv_A0.gravref);

	t_Phasing = t_TIG;
	dt = 7017.0;
	dt2 = 3028.0;
	dv_CSI = 50.0*0.3048;
	DH = 15.0*1852.0;
	ddt = 10.0;

	sv_P0 = StateVectorCalc(target);

	lamopt.GETbase = GETbase;
	lamopt.N = 0;
	lamopt.Offset = _V(-270.0*1852.0, 0.0, 60.0*1852.0 - 60000.0*0.3048);
	lamopt.Perturbation = RTCC_LAMBERT_PERTURBED;
	lamopt.T1 = t_Phasing;
	lamopt.sv_P = sv_P0;

	lamopt2 = lamopt;
	lamopt2.Offset = _V(-147.0*1852.0, 0.0, 14.7*1852.0);

	t_sv0 = OrbMech::GETfromMJD(sv_A0.MJD, GETbase);
	sv_Phasing = coast(sv_A0, t_Phasing - t_sv0);

	//Loop
	while (abs(ddt) > 1.0)
	{
		t_Insertion = t_Phasing + dt;

		lamopt.T2 = t_Insertion;
		lamopt.sv_A = sv_Phasing;

		LambertTargeting(&lamopt, lamres);
		dV_Phasing = lamres.dV;

		sv_Phasing_apo = sv_Phasing;
		sv_Phasing_apo.V += dV_Phasing;

		ddt2 = 1.0;

		//Loop
		while (abs(ddt2) > 0.1)
		{
			t_CSI = t_Insertion + dt2;

			lamopt2.T1 = t_Insertion;
			lamopt2.T2 = t_CSI;
			lamopt2.sv_A = sv_Phasing_apo;

			LambertTargeting(&lamopt2, lamres);
			dV_Insertion = lamres.dV;

			sv_Insertion = coast(sv_Phasing_apo, t_Insertion - t_Phasing);
			sv_Insertion_apo = sv_Insertion;
			sv_Insertion_apo.V += dV_Insertion;

			sv_CSI = coast(sv_Insertion_apo, t_CSI - t_Insertion);
			T_P = OrbMech::period(sv_CSI.R, sv_CSI.V, mu);
			ddt2 = OrbMech::timetoapo(sv_CSI.R, sv_CSI.V, mu);

			if (ddt2 > T_P / 2.0)
			{
				ddt2 = ddt2 - T_P;
			}
			dt2 += ddt2;
		}

		//CSI Targeting
		sv_P_CSI = coast(sv_P0, t_CSI - OrbMech::GETfromMJD(sv_P0.MJD, GETbase));
		OrbMech::CSIToDH(sv_CSI.R, sv_CSI.V, sv_P_CSI.R, sv_P_CSI.V, DH, mu, dv_CSI);
		sv_CSI_apo = sv_CSI;
		sv_CSI_apo.V = sv_CSI.V + OrbMech::ApplyHorizontalDV(sv_CSI.R, sv_CSI.V, dv_CSI);

		//CDH Targeting
		T_P = OrbMech::period(sv_CSI_apo.R, sv_CSI_apo.V, mu);
		t_CDH = t_CSI + T_P / 2.0;
		NSRProgram(sv_CSI_apo, sv_P_CSI, GETbase, 0.0, t_CDH, 0.0, dV_CDH);
		sv_CDH = coast(sv_CSI_apo, t_CDH - t_CSI);
		Q_Xx = OrbMech::LVLH_Matrix(sv_CDH.R, sv_CDH.V);
		DVX = tmul(Q_Xx, dV_CDH);
		sv_CDH_apo = sv_CDH;
		sv_CDH_apo.V += DVX;
		sv_P_CDH = coast(sv_P_CSI, t_CDH - t_CSI);

		//Find TPI time and recycle
		dt_TPI = OrbMech::findelev(sv_CDH_apo.R, sv_CDH_apo.V, sv_P_CDH.R, sv_P_CDH.V, sv_CDH_apo.MJD, 26.6*RAD, sv_CDH_apo.gravref);
		t_TPI_apo = t_CDH + dt_TPI;
		ddt = t_TPI - t_TPI_apo;
		dt += ddt;
	}

	t_Ins = t_Insertion;
	CSI = t_CSI;
}