/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER vessel module: Saturn-specific switches

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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.12  2006/03/27 19:22:44  quetalsi
  *	Bugfix RCS PRPLNT switches and wired to brakers.
  *	
  *	Revision 1.11  2006/02/22 18:54:57  tschachim
  *	Bugfixes for Apollo 4-6.
  *	
  *	Revision 1.10  2006/02/21 11:54:04  tschachim
  *	Moved TLI sequence to the IU.
  *	
  *	Revision 1.9  2006/02/02 18:52:35  tschachim
  *	Improved Accel G meter.
  *	
  *	Revision 1.8  2006/02/01 18:19:55  tschachim
  *	Added SaturnValveSwitch::SwitchTo function.
  *	
  *	Revision 1.7  2006/01/11 02:59:43  movieman523
  *	Valve talkbacks now check the valve state directlry. This means they barberpole on SM sep and can't then be changed.
  *	
  *	Revision 1.6  2006/01/11 02:16:25  movieman523
  *	Added RCS propellant quantity gauge.
  *	
  *	Revision 1.5  2005/10/31 10:18:13  tschachim
  *	Offset for SaturnToggleSwitch, SPSSwitch is now 2-pos.
  *	
  *	Revision 1.4  2005/09/30 11:22:40  tschachim
  *	Added ECS meters.
  *	
  *	Revision 1.3  2005/08/23 22:18:47  movieman523
  *	SPS switch now works.
  *	
  *	Revision 1.2  2005/08/23 20:13:12  movieman523
  *	Added RCS talkbacks and changed AGC to use octal addresses for EMEM.
  *	
  *	Revision 1.1  2005/08/16 20:55:23  movieman523
  *	Added first saturn-specific switch for Xlunar Inject.
  *	
  **************************************************************************/

#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "OrbiterSoundSDK3.h"
#include "soundlib.h"

#include "resource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"

#include "saturn.h"


void SaturnToggleSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, Saturn *s, int xoffset, int yoffset)

{
	ToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row, xoffset, yoffset);
	sat = s;
}

bool XLunarSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (ToggleSwitch::CheckMouseClick(event, mx, my)) {
		// Do nothing, handling is done in the IU
		return true;
	}
	return false;
}

void SaturnThreePosSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, Saturn *s)

{
	ThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	sat = s;
}

SaturnValveTalkback::SaturnValveTalkback()

{
	Valve = 0;
	our_vessel = 0;
}

void SaturnValveTalkback::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, int vlv, Saturn *s)

{
	IndicatorSwitch::Init(xp, yp, w, h, surf, row);

	Valve = vlv;
	our_vessel = s;
}

int SaturnValveTalkback::GetState()

{
	if (our_vessel && (SRC->Voltage() > 20))
		return our_vessel->GetValveState(Valve) ? 1 : 0;

	return 0;
}

SaturnPropValveTalkback::SaturnPropValveTalkback()

{
	Valve1 = 0;
	Valve2 = 0;
	our_vessel = 0;
}

void SaturnPropValveTalkback::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, int vlv1, int vlv2, Saturn *s)

{
	IndicatorSwitch::Init(xp, yp, w, h, surf, row);

	Valve1 = vlv1;
	Valve2 = vlv2;
	our_vessel = s;
}

int SaturnPropValveTalkback::GetState()

{
	if (our_vessel && (SRC->Voltage() > 20))
		return (our_vessel->GetValveState(Valve1) ? 1 : 0) && (our_vessel->GetValveState(Valve2) ? 1 : 0);

	return 0;
}

void SaturnValveSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, Saturn *s, int valve, IndicatorSwitch *ind)

{
	SaturnThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row, s);

	Valve = valve;
	Indicator = ind;
}

bool SaturnValveSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (SaturnThreePosSwitch::CheckMouseClick(event, mx, my)) {
		CheckValve(GetState());
		return true;
	}

	return false;
}

bool SaturnValveSwitch::SwitchTo(int newState)

{
	if (SaturnThreePosSwitch::SwitchTo(newState)) {
		// some of these switches are spring-loaded, 
		// so we have to use newState here
		CheckValve(newState);
		return true;
	}

	return false;
}

void SaturnValveSwitch::CheckValve(int s) 

{
	if (sat && (SRC->Voltage() > 20)) {
		if (s == THREEPOSSWITCH_UP) {
			sat->SetValveState(Valve, true);
			if (Indicator)
				*Indicator = true;
		}
		else if (s == THREEPOSSWITCH_DOWN) {
			sat->SetValveState(Valve, false);
			if (Indicator)
				*Indicator = false;
		}
	}
}

void SaturnPropValveSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, Saturn *s, int valve1, int valve2, int valve3,
		int valve4,	IndicatorSwitch *ind1, IndicatorSwitch *ind2)

{
	SaturnThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row, s);

	Valve1 = valve1;
	Valve2 = valve2;
	Valve3 = valve3;
	Valve4 = valve4;
	Indicator1 = ind1;
	Indicator2 = ind2;
}

bool SaturnPropValveSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (SaturnThreePosSwitch::CheckMouseClick(event, mx, my)) {
		CheckValve(GetState());
		return true;
	}

	return false;
}

bool SaturnPropValveSwitch::SwitchTo(int newState)

{
	if (SaturnThreePosSwitch::SwitchTo(newState)) {
		// some of these switches are spring-loaded, 
		// so we have to use newState here
		CheckValve(newState);
		return true;
	}

	return false;
}

void SaturnPropValveSwitch::CheckValve(int s) 

{
	if (sat && (SRC->Voltage() > 20)) {
		if (s == THREEPOSSWITCH_UP) {
			sat->SetValveState(Valve1, true);
			sat->SetValveState(Valve2, true);
			sat->SetValveState(Valve3, true);
			sat->SetValveState(Valve4, true);
			if (Indicator1)
				*Indicator1 = true;
			if (Indicator2)
				*Indicator2 = true;
		}
		else if (s == THREEPOSSWITCH_DOWN) {
			sat->SetValveState(Valve1, false);
			sat->SetValveState(Valve2, false);
			sat->SetValveState(Valve3, false);
			sat->SetValveState(Valve4, false);
			if (Indicator1)
				*Indicator1 = false;
			if (Indicator2)
				*Indicator2 = false;
		}
	}
}

bool SaturnSPSSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (SaturnToggleSwitch::CheckMouseClick(event, mx, my)) {
		if (sat) {
			sat->CheckSPSState();
		}
		return true;
	}

	return false;
}

void SaturnSPSSwitch::SetState(bool s)

{
	SaturnToggleSwitch::SetState(s);
	if (sat) {
		sat->CheckSPSState();
	}
}

void SaturnH2PressureMeter::Init(int i, SURFHANDLE surf, SwitchRow &row, Saturn *s)

{
	MeterSwitch::Init(row);
	Index = i;
	NeedleSurface = surf;
	Sat = s;
}

double SaturnH2PressureMeter::QueryValue()

{
	TankPressures press;
	Sat->GetTankPressures(press);

	if (Index == 1) 
		return press.H2Tank1PressurePSI;
	else
		return press.H2Tank2PressurePSI;
}

void SaturnH2PressureMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	if (Index == 1) 
		oapiBlt(drawSurface, NeedleSurface,  0, (110 - (int)(v / 400.0 * 104.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
	else
		oapiBlt(drawSurface, NeedleSurface, 53, (110 - (int)(v / 400.0 * 104.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
}


void SaturnO2PressureMeter::Init(int i, SURFHANDLE surf, SwitchRow &row, Saturn *s, ToggleSwitch *o2PressIndSwitch)

{
	MeterSwitch::Init(row);
	Index = i;
	NeedleSurface = surf;
	Sat = s;
	O2PressIndSwitch = o2PressIndSwitch;
}

double SaturnO2PressureMeter::QueryValue()

{
	TankPressures press;
	Sat->GetTankPressures(press);

	if (Index == 1) 
		if (O2PressIndSwitch->IsUp())  
			return press.O2Tank1PressurePSI;
		else
			return press.O2SurgeTankPressurePSI;
	else
		return press.O2Tank2PressurePSI;
}

void SaturnO2PressureMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	if (Index == 1) 
		DoDrawSwitch(drawSurface, NeedleSurface, v, 86, 0);
	else
		DoDrawSwitch(drawSurface, NeedleSurface, v, 139, 10);
}

void SaturnO2PressureMeter::DoDrawSwitch(SURFHANDLE surf, SURFHANDLE needle, double value, int xOffset, int xNeedle)

{
	if (value < 100.0)
		oapiBlt(surf, needle, xOffset, 110, xNeedle, 0, 10, 10, SURF_PREDEF_CK);

	else if (value <= 500.0) 
		oapiBlt(surf, needle, xOffset, 110 - (int)((value - 100.0) * 0.065), xNeedle, 0, 10, 10, SURF_PREDEF_CK);

	else if (value <= 850.0)
		oapiBlt(surf, needle, xOffset, 84 - (int)((value - 500.0) * 0.07714), xNeedle, 0, 10, 10, SURF_PREDEF_CK);

	else if (value <= 900.0)
		oapiBlt(surf, needle, xOffset, 57 - (int)((value - 850.0) * 0.38), xNeedle, 0, 10, 10, SURF_PREDEF_CK);

	else if (value <= 950.0)
		oapiBlt(surf, needle, xOffset, 38 - (int)((value - 900.0) * 0.42), xNeedle, 0, 10, 10, SURF_PREDEF_CK);

	else if (value <= 1050.0)
		oapiBlt(surf, needle, xOffset, 17 - (int)((value - 950.0) * 0.13), xNeedle, 0, 10, 10, SURF_PREDEF_CK);

	else
		oapiBlt(surf, needle, xOffset, 4, xNeedle, 0, 10, 10, SURF_PREDEF_CK);
}


void SaturnCryoQuantityMeter::Init(char *sub, int i, SURFHANDLE surf, SwitchRow &row, Saturn *s)

{
	MeterSwitch::Init(row);
	Substance = sub;
	Index = i;
	NeedleSurface = surf;
	Sat = s;
}

double SaturnCryoQuantityMeter::QueryValue()

{
	TankQuantities q;
	Sat->GetTankQuantities(q);

	if (!strcmp("H2", Substance)) {
		if (Index == 1) 
			return q.H2Tank1Quantity;
		else
			return q.H2Tank2Quantity;
	} else {
		if (Index == 1) 
			return q.O2Tank1Quantity;
		else
			return q.O2Tank2Quantity;
	}
}

void SaturnCryoQuantityMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	if (!strcmp("H2", Substance)) {
		if (Index == 1) 
			oapiBlt(drawSurface, NeedleSurface,  172, (110 - (int)(v * 104.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
		else
			oapiBlt(drawSurface, NeedleSurface,  225, (110 - (int)(v * 104.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
	} else {
		if (Index == 1) 
			oapiBlt(drawSurface, NeedleSurface,  258, (110 - (int)(v * 104.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
		else {
			//
			// Apollo 13 O2 tank 2 quantity display failed offscale high around 46:45.
			//

			#define O2FAILURETIME	(46.0 * 3600.0 + 45.0 * 60.0)

			if (Sat->GetApolloNo() == 13) {
				if (Sat->GetMissionTime() >= (O2FAILURETIME + 5.0)) {
					v = 1.05;
				}
				else if (Sat->GetMissionTime() >= O2FAILURETIME) {
					v += (1.05 - value) * ((Sat->GetMissionTime() - O2FAILURETIME) / 5.0);
				}
			}
			oapiBlt(drawSurface, NeedleSurface,  311, (110 - (int)(v * 104.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
		}
	}
}

RCSQuantityMeter::RCSQuantityMeter()

{
	source = 0;
	NeedleSurface = 0;
}

void RCSQuantityMeter::Init(SURFHANDLE surf, SwitchRow &row, PropellantRotationalSwitch *s, Saturn *v)

{
	MeterSwitch::Init(row);
	source = s;
	NeedleSurface = surf;
	our_vessel = v;
}

double RCSQuantityMeter::QueryValue()

{
	if (!source)
		return 0.0;

	PropellantSource *ps = source->GetSource();

	//
	// Offscale high error return if we're not connected.
	//

	if (!ps)
		return 1.1;

	if (our_vessel->DisplayingPropellantQuantity()) {		
		return ps->Quantity();
	}

	return ps->Temperature();
}

void RCSQuantityMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	oapiBlt(drawSurface, NeedleSurface,  150, (110 - (int)(v * 104.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
}

CMRCSPropellant::CMRCSPropellant(PROPELLANT_HANDLE &h) : PropellantSource(h)

{
}

double CMRCSPropellant::Quantity()

{
	if (our_vessel && our_vessel->GetStage() < CM_STAGE)
		return 1.0;

	return PropellantSource::Quantity();
}

SMRCSPropellant::SMRCSPropellant(PROPELLANT_HANDLE &h) : PropellantSource(h)

{
}

double SMRCSPropellant::Quantity()

{
	if (our_vessel) {
		int stage = our_vessel->GetStage();
		if (stage < CSM_LEM_STAGE)
			return 1.0;
		if (stage > CSM_LEM_STAGE)
			return 0.0;
	}

	return PropellantSource::Quantity();
}

PropellantSource::PropellantSource(PROPELLANT_HANDLE &h) : source_prop(h)

{
	our_vessel = 0;
}

PROPELLANT_HANDLE PropellantSource::Handle()

{
	return source_prop;
}

double PropellantSource::Quantity()

{
	if (source_prop && our_vessel) {
		return our_vessel->GetPropellantMass(source_prop) / our_vessel->GetPropellantMaxMass(source_prop);
	}

	return 0.0;
}

PropellantRotationalSwitch::PropellantRotationalSwitch()

{
	int i;

	for (i = 0; i < 10; i++)
		sources[i] = 0;
}

void PropellantRotationalSwitch::SetSource(int num, PropellantSource *s)

{
	if (num >= 0 && num < 16)
		sources[num] = s; 
}

PropellantSource *PropellantRotationalSwitch::GetSource()

{
	PropellantSource *ps = sources[GetState()];
	return ps;
}

void SaturnFuelCellMeter::Init(SURFHANDLE surf, SwitchRow &row, Saturn *s, RotationalSwitch *fuelCellIndicatorsSwitch)

{
	MeterSwitch::Init(row);
	NeedleSurface = surf;
	Sat = s;
	FuelCellIndicatorsSwitch = fuelCellIndicatorsSwitch;
}


double SaturnFuelCellH2FlowMeter::QueryValue()

{
	FuelCellStatus fc;
	Sat->GetFuelCellStatus(FuelCellIndicatorsSwitch->GetState(), fc);

	return fc.H2FlowLBH; 
}

void SaturnFuelCellH2FlowMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	if (v < 0.05)
		oapiBlt(drawSurface, NeedleSurface, 0, (111 - (int)(v / 0.05 * 21.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
	else if (v < 0.15)
		oapiBlt(drawSurface, NeedleSurface, 0, (90 - (int)((v - 0.05) / 0.1 * 65.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
	else
		oapiBlt(drawSurface, NeedleSurface, 0, (25 - (int)((v - 0.15) / 0.05 * 21.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
}


double SaturnFuelCellO2FlowMeter::QueryValue()

{
	FuelCellStatus fc;
	Sat->GetFuelCellStatus(FuelCellIndicatorsSwitch->GetState(), fc);

	return fc.O2FlowLBH; 
}

void SaturnFuelCellO2FlowMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	if (v < 0.4)
		oapiBlt(drawSurface, NeedleSurface, 53, (111 - (int)(v / 0.4 * 21.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
	else if (v < 1.2)
		oapiBlt(drawSurface, NeedleSurface, 53, (90 - (int)((v - 0.4) / 0.8 * 65.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
	else
		oapiBlt(drawSurface, NeedleSurface, 53, (25 - (int)((v - 1.2) / 0.4 * 21.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
}


double SaturnFuelCellTempMeter::QueryValue()

{
	FuelCellStatus fc;
	Sat->GetFuelCellStatus(FuelCellIndicatorsSwitch->GetState(), fc);

	return fc.TempF; 
}

void SaturnFuelCellTempMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	if (v < 400.0)
		oapiBlt(drawSurface, NeedleSurface, 86, (109 - (int)((v - 100.0) / 300.0 * 53.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
	else if (v < 500.0)
		oapiBlt(drawSurface, NeedleSurface, 86, (56 - (int)((v - 400.0) / 100.0 * 40.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
	else
		oapiBlt(drawSurface, NeedleSurface, 86, (16 - (int)((v - 500.0) / 50.0 * 12.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
}


double SaturnFuelCellCondenserTempMeter::QueryValue()

{
	FuelCellStatus fc;
	Sat->GetFuelCellStatus(FuelCellIndicatorsSwitch->GetState(), fc);

	return fc.CondenserTempF; 
}

void SaturnFuelCellCondenserTempMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	oapiBlt(drawSurface, NeedleSurface, 139, (109 - (int)((v - 150.0) / 100.0 * 103.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
}


void SaturnCabinMeter::Init(SURFHANDLE surf, SwitchRow &row, Saturn *s)

{
	MeterSwitch::Init(row);
	NeedleSurface = surf;
	Sat = s;
}


double SaturnSuitTempMeter::QueryValue()

{
	AtmosStatus atm;
	Sat->GetAtmosStatus(atm);

	return KelvinToFahrenheit(atm.SuitTempK);
}

void SaturnSuitTempMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	oapiBlt(drawSurface, NeedleSurface,  1, (110 - (int)((v - 20.0) / 75.0 * 104.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
}


double SaturnCabinTempMeter::QueryValue()

{
	AtmosStatus atm;
	Sat->GetAtmosStatus(atm);

	return KelvinToFahrenheit(atm.CabinTempK);
}

void SaturnCabinTempMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	oapiBlt(drawSurface, NeedleSurface,  53, (110 - (int)((v - 40.0) / 80.0 * 104.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
}


double SaturnSuitPressMeter::QueryValue()

{
	AtmosStatus atm;
	Sat->GetAtmosStatus(atm);

	return atm.SuitPressurePSI;
}

void SaturnSuitPressMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	if (v < 6.0)
		oapiBlt(drawSurface, NeedleSurface,  101, (108 - (int)(v / 6.0 * 55.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
	else
		oapiBlt(drawSurface, NeedleSurface,  101, (53 - (int)((v - 6.0) / 10.0 * 45.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
}


double SaturnCabinPressMeter::QueryValue()

{
	AtmosStatus atm;
	Sat->GetAtmosStatus(atm);

	return atm.CabinPressurePSI;
}

void SaturnCabinPressMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	if (v < 6.0)
		oapiBlt(drawSurface, NeedleSurface,  153, (108 - (int)(v / 6.0 * 55.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
	else
		oapiBlt(drawSurface, NeedleSurface,  153, (53 - (int)((v - 6.0) / 10.0 * 45.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
}


double SaturnPartPressCO2Meter::QueryValue()

{
	AtmosStatus atm;
	Sat->GetAtmosStatus(atm);

	return atm.SuitCO2MMHG;
}

void SaturnPartPressCO2Meter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	if (v < 10.0)
		oapiBlt(drawSurface, NeedleSurface,  215, (109 - (int)(v / 10.0 * 55.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
	else if (v < 15.0)
		oapiBlt(drawSurface, NeedleSurface,  215, (54 - (int)((v - 10.0) / 5.0 * 19.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
	else if (v < 20.0)
		oapiBlt(drawSurface, NeedleSurface,  215, (35 - (int)((v - 15.0) / 5.0 * 15.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
	else
		oapiBlt(drawSurface, NeedleSurface,  215, (20 - (int)((v - 20.0) / 10.0 * 14.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
}


void SaturnRoundMeter::Init(HPEN p0, HPEN p1, SwitchRow &row, Saturn *s)

{
	MeterSwitch::Init(row);
	Pen0 = p0;
	Pen1 = p1;
	Sat = s;
}

void SaturnRoundMeter::DrawNeedle (SURFHANDLE surf, int x, int y, double rad, double angle)

{
	// Needle function by Rob Conley from Mercury code
	
	double dx = rad * cos(angle), dy = rad * sin(angle);
	HGDIOBJ oldObj;

	HDC hDC = oapiGetDC (surf);
	oldObj = SelectObject (hDC, Pen1);
	MoveToEx (hDC, x, y, 0); LineTo (hDC, x + (int)(0.85*dx+0.5), y - (int)(0.85*dy+0.5));
	SelectObject (hDC, oldObj);
	oldObj = SelectObject (hDC, Pen0);
	MoveToEx (hDC, x, y, 0); LineTo (hDC, x + (int)(dx+0.5), y - (int)(dy+0.5));
	SelectObject (hDC, oldObj);
	oapiReleaseDC (surf, hDC);
}


double SaturnSuitComprDeltaPMeter::QueryValue()

{
	AtmosStatus atm;
	Sat->GetAtmosStatus(atm);

	// Suit compressor pressure difference
	return (atm.SuitPressurePSI - atm.SuitReturnPressurePSI);
}

void SaturnSuitComprDeltaPMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	v = (v - .5) / .5 * 60.0;
	DrawNeedle(drawSurface, 45, 22, 20.0, (180.0 - v) * RAD);
}


double SaturnLeftO2FlowMeter::QueryValue()

{
	AtmosStatus atm;
	Sat->GetAtmosStatus(atm);

	// O2 main regulator output flow 
	// TODO: Is this the correct flow for that meter? No documentation found yet...
	
	return atm.CabinRegulatorFlowLBH + atm.O2DemandFlowLBH + atm.DirectO2FlowLBH;
}

void SaturnLeftO2FlowMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	v = (v - .6) / .4 * 60.0;	
	DrawNeedle(drawSurface, 0, 22, 20.0, v * RAD);
}


double SaturnSuitCabinDeltaPMeter::QueryValue()

{
	AtmosStatus atm;
	Sat->GetAtmosStatus(atm);

	// Suit cabin pressure difference
	return (atm.SuitReturnPressurePSI - atm.CabinPressurePSI) * (INH2O / PSI);
}

void SaturnSuitCabinDeltaPMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	v = (v / 5.0) * 60.0;
	DrawNeedle(drawSurface, 0, 22, 20.0, v * RAD);
	oapiBlt(drawSurface, FrameSurface, 0, 13, 0, 0, 46, 18, SURF_PREDEF_CK);
}


double SaturnRightO2FlowMeter::QueryValue()

{
	AtmosStatus atm;
	Sat->GetAtmosStatus(atm);

	// O2 main regulator output flow 	
	return atm.CabinRegulatorFlowLBH + atm.O2DemandFlowLBH + atm.DirectO2FlowLBH;
}

void SaturnRightO2FlowMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	v = (v - .6) / .4 * 60.0;
	DrawNeedle(drawSurface, 45, 22, 20.0, (180.0 - v) * RAD);
	oapiBlt(drawSurface, FrameSurface, 0, 13, 0, 0, 46, 18, SURF_PREDEF_CK);
}


double SaturnEcsRadTempInletMeter::QueryValue()

{
	if (Sat->GetRotationalSwitchState("ECSIndicatorsSwitch") == 1) {
		PrimECSCoolingStatus pcs;
		Sat->GetPrimECSCoolingStatus(pcs);
		return pcs.RadiatorInletTempF; 
	} else {
		SecECSCoolingStatus scs;
		Sat->GetSecECSCoolingStatus(scs);
		return scs.RadiatorInletTempF;
	}
}

void SaturnEcsRadTempInletMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	v = (v - 90.0) / 30.0 * 60.0;	
	DrawNeedle(drawSurface, 0, 22, 20.0, v * RAD);
}


double SaturnEcsRadTempPrimOutletMeter::QueryValue()

{
	PrimECSCoolingStatus pcs;
	Sat->GetPrimECSCoolingStatus(pcs);

	return pcs.RadiatorOutletTempF; 
}

void SaturnEcsRadTempPrimOutletMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	v = (v - 25.0) / 75.0 * 60.0;	
	DrawNeedle(drawSurface, 45, 22, 20.0, (180.0 - v) * RAD);
}


double SaturnEcsRadTempSecOutletMeter::QueryValue()

{
	SecECSCoolingStatus scs;
	Sat->GetSecECSCoolingStatus(scs);

	return scs.RadiatorOutletTempF; 
}

void SaturnEcsRadTempSecOutletMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	v = (v - 50.0) / 20.0 * 60.0;	
	DrawNeedle(drawSurface, 0, 22, 20.0, v * RAD);
}


double SaturnGlyEvapTempOutletMeter::QueryValue()

{
	if (Sat->GetRotationalSwitchState("ECSIndicatorsSwitch") == 1) {
		PrimECSCoolingStatus pcs;
		Sat->GetPrimECSCoolingStatus(pcs);
		return pcs.EvaporatorOutletTempF; 
	} else {
		SecECSCoolingStatus scs;
		Sat->GetSecECSCoolingStatus(scs);
		return scs.EvaporatorOutletTempF;
	}
}

void SaturnGlyEvapTempOutletMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	v = (v - 50.0) / 20.0 * 60.0;	
	DrawNeedle(drawSurface, 45, 22, 20.0, (180.0 - v) * RAD);
}

double SaturnGlyEvapSteamPressMeter::QueryValue()

{
	if (Sat->GetRotationalSwitchState("ECSIndicatorsSwitch") == 1) {
		PrimECSCoolingStatus pcs;
		Sat->GetPrimECSCoolingStatus(pcs);
		return pcs.EvaporatorSteamPressurePSI; 
	} else {
		SecECSCoolingStatus scs;
		Sat->GetSecECSCoolingStatus(scs);
		return scs.EvaporatorSteamPressurePSI;
	}
}

void SaturnGlyEvapSteamPressMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	v = (v - 0.15) / 0.1 * 60.0;	
	DrawNeedle(drawSurface, 0, 22, 20.0, v * RAD);
}


double SaturnGlycolDischPressMeter::QueryValue()

{
	if (Sat->GetRotationalSwitchState("ECSIndicatorsSwitch") == 1) {
		PrimECSCoolingStatus pcs;
		Sat->GetPrimECSCoolingStatus(pcs);
		return pcs.RadiatorInletPressurePSI; 
	} else {
		SecECSCoolingStatus scs;
		Sat->GetSecECSCoolingStatus(scs);
		return scs.RadiatorInletPressurePSI;
	}
}

void SaturnGlycolDischPressMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	v = (v - 30.0) / 30.0 * 60.0;	
	DrawNeedle(drawSurface, 45, 22, 20.0, (180.0 - v) * RAD);
}


double SaturnAccumQuantityMeter::QueryValue()

{
	if (Sat->GetRotationalSwitchState("ECSIndicatorsSwitch") == 1) {
		PrimECSCoolingStatus pcs;
		Sat->GetPrimECSCoolingStatus(pcs);
		return pcs.AccumulatorQuantityPercent; 
	} else {
		SecECSCoolingStatus scs;
		Sat->GetSecECSCoolingStatus(scs);
		return scs.AccumulatorQuantityPercent;
	}
}

void SaturnAccumQuantityMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	v = (v - 0.5) / 0.5 * 60.0;	
	DrawNeedle(drawSurface, 0, 22, 20.0, v * RAD);
}


double SaturnH2oQuantityMeter::QueryValue()

{
	ECSWaterStatus ws;
	Sat->GetECSWaterStatus(ws);

	if (Sat->GetSwitchState("H2oQtyIndSwitch"))
		return ws.PotableH2oTankQuantityPercent; 
	else
		return ws.WasteH2oTankQuantityPercent;
}

void SaturnH2oQuantityMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	v = (v - 0.5) / 0.5 * 60.0;	
	DrawNeedle(drawSurface, 45, 22, 20.0, (180.0 - v) * RAD);
}

double SaturnAccelGMeter::QueryValue()

{
	return Sat->GetAccelG();
}

void SaturnAccelGMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	v = (-v * 180.0 / 12.0) + 180.0;
	DrawNeedle (drawSurface, 40, 40, 35.0, v * RAD);
}
