/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Lunar Module Descent Propulsion System (Header)

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

#pragma once

class LEM;
class AGCIOSwitch;
class e_object;

class DPSValve {

public:
	DPSValve();
	void SetState(bool open);
	bool IsOpen() { return isOpen; };
	void SwitchToggled(PanelSwitchItem *s);

protected:
	bool isOpen;
};

class LEMPropellantSource {
public:
	LEMPropellantSource(PROPELLANT_HANDLE &h);
	void SetVessel(LEM *v) { our_vessel = v; };
	PROPELLANT_HANDLE Handle();
	virtual double Quantity();
	virtual double Temperature() { return 0.0; };
	virtual double Pressure() { return 0.0; };

protected:
	PROPELLANT_HANDLE &source_prop;
	LEM *our_vessel;
};

class DPSPropellantSource : public LEMPropellantSource {
public:
	DPSPropellantSource(PROPELLANT_HANDLE &ph, PanelSDK &p);

	void Init(e_object *dc1);
	void Timestep(double simt, double simdt);
	void SystemTimestep(double simdt);
	double GetFuelPercent();
	double GetOxidPercent();
	double GetAmbientHeliumPressPSI();
	double GetSupercriticalHeliumPressPSI();
	double GetHeliumRegulatorManifoldPressurePSI();
	double GetFuelTankUllagePressurePSI();
	double GetOxidizerTankUllagePressurePSI();
	double GetFuelEngineInletPressurePSI() { return FuelEngineInletPressurePSI; }
	bool PropellantLevelLow();

	DPSValve *GetHeliumValve1() { return &PrimaryHeRegulatorShutoffValve; }
	DPSValve *GetHeliumValve2() { return &SecondaryHeRegulatorShutoffValve; }
	DPSValve *GetOxidVentValve2() { return &OxidVentValve2; }
	DPSValve *GetFuelVentValve2() { return &FuelVentValve2; }

	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);
protected:
	bool IsGaugingPowered();

	double propellantMassToDisplay;
	double propellantMaxMassToDisplay;
	double ambientHeliumPressurePSI;
	double supercriticalHeliumPressurePSI;
	double heliumRegulatorManifoldPressurePSI;
	double FuelTankUllagePressurePSI;
	double OxidTankUllagePressurePSI;
	double FuelEngineInletPressurePSI;

	bool fuel1LevelLow;
	bool fuel2LevelLow;
	bool oxid1LevelLow;
	bool oxid2LevelLow;

	DPSValve PrimaryHeRegulatorShutoffValve;
	DPSValve SecondaryHeRegulatorShutoffValve;
	DPSValve AmbientHeIsolValve;
	DPSValve SupercritHeIsolValve;
	DPSValve OxidCompatibilityValve;
	DPSValve FuelCompatibilityValve;
	DPSValve OxidVentValve1;
	DPSValve OxidVentValve2;
	DPSValve FuelVentValve1;
	DPSValve FuelVentValve2;

	e_object *GaugingPower;
};


class DPSGimbalActuator {

public:
	DPSGimbalActuator();
	virtual ~DPSGimbalActuator();

	void Init(LEM *s, AGCIOSwitch *m1Switch, e_object *m1Source);
	void Timestep(double simt, double simdt);
	void SystemTimestep(double simdt);
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);
	double GetPosition() { return position; }
	void ChangeCmdPosition(int pos);
	void ZeroCmdPosition() { commandedPosition = 0; }
	int GetCmdPosition() { return commandedPosition; }
	bool GimbalFail() { return gimbalfail; }

	void GimbalTimestep(double simdt);

protected:
	bool IsSystemPowered();
	void DrawSystemPower();

	double position;
	int commandedPosition;
	bool motorRunning;
	bool gimbalfail;

	LEM *lem;
	AGCIOSwitch *gimbalMotorSwitch;
	e_object *motorSource;
};

// Descent Engine
class LEM_DPS {
public:
	LEM_DPS(THRUSTER_HANDLE *dps);
	void Init(LEM *s);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void TimeStep(double simt, double simdt);
	void SystemTimestep(double simdt);

	void ThrottleActuator(double manthrust, double autothrust);

	LEM *lem;					// Pointer at LEM
	bool thrustOn;				// Engine "On" Command
	bool engArm;				// Engine Arm Command
	bool engPreValvesArm;		// Engine Prevalves Arm Command
	double thrustcommand;		// DPS Thrust Command

	DPSGimbalActuator pitchGimbalActuator;
	DPSGimbalActuator rollGimbalActuator;

protected:

	THRUSTER_HANDLE *dpsThruster;

};

#define DPSPROPELLANT_START_STRING     "DPSPROPELLANT_BEGIN"
#define DPSPROPELLANT_END_STRING     "DPSPROPELLANT_END"