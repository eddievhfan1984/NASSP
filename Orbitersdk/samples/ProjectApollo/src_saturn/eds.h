/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Saturn Emergency Detection System (Header)

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

class LVRG;
class IU;

class EDS
{
public:
	EDS(LVRG &rg);
	virtual ~EDS() {}
	virtual void Timestep(double simdt) = 0;
	void Init(IU *i);
	void SetPlatformFailureParameters(bool PlatFail, double PlatFailTime);

	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	void SetTwoEngOutAutoAbortInhibit(bool set) { TwoEngOutAutoAbortInhibit = set; }
	void SetExcessiveRatesAutoAbortInhibit(bool set) { ExcessiveRatesAutoAbortInhibit = set; }
	void SetSIEngineOutIndicationA(bool set) { SIEngineOutIndicationA = set; }
	void SetSIEngineOutIndicationB(bool set) { SIEngineOutIndicationB = set; }
	void SetSIVBEngineOutIndicationA(bool set) { SIVBEngineOutIndicationA = set; }
	void SetSIVBEngineOutIndicationB(bool set) { SIVBEngineOutIndicationB = set; }
	void SetRateGyroSCIndicationSwitchA(bool set) { RateGyroSCIndicationSwitchA = set; }
	void SetRateGyroSCIndicationSwitchB(bool set) { RateGyroSCIndicationSwitchB = set; }
	void SetLVEnginesCutoffEnable(bool set) { LVEnginesCutoffEnable = set; }
	void ResetAutoAbortRelays() { AutoAbortEnableRelayA = false; AutoAbortEnableRelayB = false; }
	void SetSIVBEngineCutoffDisabled() { SIVBEngineCutoffDisabled = true; }

	bool GetLiftoffCircuitA() { return LiftoffA; }
	bool GetLiftoffCircuitB() { return LiftoffB; }
	bool GetEDSAbort(int n);
protected:
	LVRG &lvrg;

	IU* iu;

	//Common Relays:
	
	//K29
	bool AutoAbortInitiate;
	//K43
	bool TwoEngOutAutoAbortDeactivate;
	//K46
	bool ExcessRatesAutoAbortDeactivatePY;
	//K47
	bool ExcessRatesAutoAbortDeactivateR;

	bool TwoEngOutAutoAbortInhibit;
	bool ExcessiveRatesAutoAbortInhibit;
	bool LVEnginesCutoffEnable;
	bool RateGyroSCIndicationSwitchA;
	bool RateGyroSCIndicationSwitchB;
	bool SIEngineOutIndicationA;
	bool SIEngineOutIndicationB;
	bool SIIEngineOutIndicationA;
	bool SIIEngineOutIndicationB;
	bool SIVBEngineOutIndicationA;
	bool SIVBEngineOutIndicationB;
	//K235
	bool AutoAbortInhibitRelayA;
	//K236
	bool AutoAbortInhibitRelayB;
	bool AutoAbortEnableRelayA;
	bool AutoAbortEnableRelayB;
	bool LiftoffA;
	bool LiftoffB;
	bool LVEnginesCutoff1;
	bool LVEnginesCutoff2;
	bool LVEnginesCutoff3;
	bool SecondPlaneSeparationMonitorRelay;
	bool SIVBEngineCutoffDisabled;
	bool SIEDSCutoff;
	bool SIIEDSCutoff;
	bool SIVBEDSCutoff;
	bool EDSAbortSignal1;
	bool EDSAbortSignal2;
	bool EDSAbortSignal3;

	//Common Saturn Failures
	bool PlatformFailure;
	double PlatformFailureTime;
};

class EDS1B : public EDS
{
public:
	EDS1B(LVRG &rg);
	void Timestep(double simdt);
	void LVIndicatorsOff();
	bool ThrustCommitEval();
protected:

	bool SIThrustOK[8];
};

class EDSSV : public EDS
{
public:
	EDSSV(LVRG &rg);
	void Timestep(double simdt);
	void LVIndicatorsOff();
	bool ThrustCommitEval();
	void SetSIIEngineOutIndicationA(bool set) { SIIEngineOutIndicationA = set; }
	void SetSIIEngineOutIndicationB(bool set) { SIIEngineOutIndicationB = set; }
protected:
	bool SIThrustOK[5];
	bool SIIThrustOK[5];

private:
	const int SIIEngInd[5] = { 2,4,1,3,5 };
};