/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant
  Copyright 2002-2005 Chris Knestrick


  ORBITER vessel module: LEM systems code

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

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "lmresource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "lm_channels.h"
#include "dsky.h"
#include "PanelSDK/Internals/Esystems.h"

#include "LEM.h"

#include "CollisionSDK/CollisionSDK.h"
#include "papi.h"


#define RR_SHAFT_STEP 0.000191747598876953125 
#define RR_TRUNNION_STEP 0.00004793689959716796875


void LEM::ResetThrusters()

{
	SetAttitudeRotLevel(0, 0);
	SetAttitudeRotLevel(1, 0);
	SetAttitudeRotLevel(2, 0);
	SetAttitudeLinLevel(0, 0);
	SetAttitudeLinLevel(1, 0);
	SetAttitudeLinLevel(2, 0);
	ActivateNavmode(NAVMODE_KILLROT);
}

void LEM::AddRCS_LMH(double TRANZ)
{
	const double ATTCOOR = 1.78;
	const double ATTCOOR2 = 1.35;
	const double ATTZ = 2.85;
	const double TRANCOOR = 0;
	const double TRANCOOR2 = 0.1;
	const double ATTWIDTH=.15;
	const double ATTHEIGHT=3;
	const double TRANWIDTH=.2;
	const double TRANHEIGHT=1;
	const double RCSOFFSET=0.75;
	const double RCSOFFSETM=0.30;
	const double RCSOFFSETM2=0.47;

	double MaxThrust=445.0;
	double RCSISP=2840.0;

	// A1U
	th_rcs[0]=CreateThruster(_V(-ATTCOOR,ATTCOOR2+0.3,TRANZ+RCSOFFSETM2-0.12+3.3), _V(0,-1,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[0],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// A1F
	th_rcs[1]=CreateThruster(_V(-ATTCOOR+.05,ATTCOOR2-0.18,TRANZ+RCSOFFSET+3.10), _V(0,0,-1), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[1],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// B1L
	th_rcs[2]=CreateThruster(_V(-ATTCOOR-0.30,ATTCOOR2-0.18,TRANZ+RCSOFFSETM2+3.17), _V(1,0,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[2],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// B1D
	th_rcs[3]=CreateThruster(_V(-ATTCOOR,ATTCOOR2-.55,TRANZ+RCSOFFSETM2-0.12+3.3), _V(0,1,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[3],ATTHEIGHT,ATTWIDTH, exhaustTex);

	// B2U
	th_rcs[4]=CreateThruster(_V(-ATTCOOR+.05,ATTCOOR2+0.0,TRANZ+RCSOFFSETM2-0.30), _V(0,-1,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[4],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// B2L
	th_rcs[5]=CreateThruster(_V(-ATTCOOR-0.2,ATTCOOR2-0.35,TRANZ+RCSOFFSETM2-.25), _V(1,0,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[5],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// A2A
	th_rcs[6]=CreateThruster(_V(-ATTCOOR+.10,ATTCOOR2-0.35,TRANZ+RCSOFFSET-0.8), _V(0,0,1), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[6],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// A2D
	th_rcs[7]=CreateThruster(_V(-ATTCOOR+.1,ATTCOOR2-.65,TRANZ+RCSOFFSETM2-0.3), _V(0,1,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[7],ATTHEIGHT,ATTWIDTH, exhaustTex);

	// A3U
	th_rcs[8]=CreateThruster(_V(ATTCOOR-0.05,ATTCOOR2+0.0,TRANZ+RCSOFFSETM2-0.35), _V(0,-1,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[8],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// A3R
	th_rcs[9]=CreateThruster(_V(ATTCOOR+0.2,ATTCOOR2-0.35,TRANZ+RCSOFFSETM2-.25), _V(-1,0,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[9],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// B3A
	th_rcs[10]=CreateThruster(_V(ATTCOOR-.10,ATTCOOR2-0.35,TRANZ+RCSOFFSET-0.8), _V(0,0,1), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[10],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// B3D
	th_rcs[11]=CreateThruster(_V(ATTCOOR-0.1,ATTCOOR2-.65,TRANZ+RCSOFFSETM2-0.35), _V(0,1,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[11],ATTHEIGHT,ATTWIDTH, exhaustTex);

	// B4U
	th_rcs[12]=CreateThruster(_V(ATTCOOR,ATTCOOR2+0.3,TRANZ+RCSOFFSETM2-0.12+3.3), _V(0,-1,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[12],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// B4F
	th_rcs[13]=CreateThruster(_V(ATTCOOR-.001,ATTCOOR2-0.18,TRANZ+RCSOFFSET+3.10), _V(0,0,-1), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[13],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// A4R
	th_rcs[14]=CreateThruster(_V(ATTCOOR+0.30,ATTCOOR2-0.18,TRANZ+RCSOFFSETM2+3.17), _V(-1,0,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[14],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// A4D
	th_rcs[15]=CreateThruster(_V(ATTCOOR,ATTCOOR2-.55,TRANZ+RCSOFFSETM2-0.12+3.3), _V(0,1,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[15],ATTHEIGHT,ATTWIDTH, exhaustTex);

	/* THRUSTER TABLE:
		0	A1U		8	A3U
		1	A1F		9	A3R
		2	B1L		10	B3A
		3	B1D		11	B3D

		4	B2U		12	B4U
		5	B2L		13	B4F
		6	A2A		14	A4R
		7	A2D		15	A4D
	*/

//	CreateThrusterGroup (th_rcs,   1, THGROUP_ATT_YAWLEFT);
}

void LEM::AddRCS_LMH2(double TRANZ)
{
	const double ATTCOOR = 1.78;
	const double ATTCOOR2 = -0.50;
	const double ATTZ = 2.85;
	const double TRANCOOR = 0;
	const double TRANCOOR2 = 0.1;
	const double ATTWIDTH=.15;
	const double ATTHEIGHT=3;
	const double TRANWIDTH=.2;
	const double TRANHEIGHT=1;
	const double RCSOFFSET=0.75;
	const double RCSOFFSETM=0.30;
	const double RCSOFFSETM2=0.47;

	double MaxThrust=445.0;
	double RCSISP=2840.0;

	// A1U
	th_rcs[0]=CreateThruster(_V(-ATTCOOR,ATTCOOR2+0.3,TRANZ+RCSOFFSETM2-0.12+3.3), _V(0,-1,0), MaxThrust, ph_RCSA, RCSISP, RCSISP);
	AddExhaust(th_rcs[0],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// A1F
	th_rcs[1]=CreateThruster(_V(-ATTCOOR+.05,ATTCOOR2-0.18,TRANZ+RCSOFFSET+3.10), _V(0,0,-1), MaxThrust, ph_RCSA, RCSISP, RCSISP);
	AddExhaust(th_rcs[1],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// B1L
	th_rcs[2]=CreateThruster(_V(-ATTCOOR-0.30,ATTCOOR2-0.18,TRANZ+RCSOFFSETM2+3.17), _V(1,0,0), MaxThrust, ph_RCSB, RCSISP, RCSISP);
	AddExhaust(th_rcs[2],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// B1D
	th_rcs[3]=CreateThruster(_V(-ATTCOOR,ATTCOOR2-.55,TRANZ+RCSOFFSETM2-0.12+3.3), _V(0,1,0), MaxThrust, ph_RCSB, RCSISP, RCSISP);
	AddExhaust(th_rcs[3],ATTHEIGHT,ATTWIDTH, exhaustTex);

	// B2U
	th_rcs[4]=CreateThruster(_V(-ATTCOOR+.05,ATTCOOR2+0.0,TRANZ+RCSOFFSETM2-0.30), _V(0,-1,0), MaxThrust, ph_RCSB, RCSISP, RCSISP);
	AddExhaust(th_rcs[4],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// B2L
	th_rcs[5]=CreateThruster(_V(-ATTCOOR-0.2,ATTCOOR2-0.35,TRANZ+RCSOFFSETM2-.25), _V(1,0,0), MaxThrust, ph_RCSB, RCSISP, RCSISP);
	AddExhaust(th_rcs[5],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// A2A
	th_rcs[6]=CreateThruster(_V(-ATTCOOR+.10,ATTCOOR2-0.35,TRANZ+RCSOFFSET-0.8), _V(0,0,1), MaxThrust, ph_RCSA, RCSISP, RCSISP);
	AddExhaust(th_rcs[6],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// A2D
	th_rcs[7]=CreateThruster(_V(-ATTCOOR+.1,ATTCOOR2-.65,TRANZ+RCSOFFSETM2-0.3), _V(0,1,0), MaxThrust, ph_RCSA, RCSISP, RCSISP);
	AddExhaust(th_rcs[7],ATTHEIGHT,ATTWIDTH, exhaustTex);

	// A3U
	th_rcs[8]=CreateThruster(_V(ATTCOOR-0.05,ATTCOOR2+0.0,TRANZ+RCSOFFSETM2-0.35), _V(0,-1,0), MaxThrust, ph_RCSA, RCSISP, RCSISP);
	AddExhaust(th_rcs[8],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// A3R
	th_rcs[9]=CreateThruster(_V(ATTCOOR+0.2,ATTCOOR2-0.35,TRANZ+RCSOFFSETM2-.25), _V(-1,0,0), MaxThrust, ph_RCSA, RCSISP, RCSISP);
	AddExhaust(th_rcs[9],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// B3A
	th_rcs[10]=CreateThruster(_V(ATTCOOR-.10,ATTCOOR2-0.35,TRANZ+RCSOFFSET-0.8), _V(0,0,1), MaxThrust, ph_RCSB, RCSISP, RCSISP);
	AddExhaust(th_rcs[10],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// B3D
	th_rcs[11]=CreateThruster(_V(ATTCOOR-0.1,ATTCOOR2-.65,TRANZ+RCSOFFSETM2-0.35), _V(0,1,0), MaxThrust, ph_RCSB, RCSISP, RCSISP);
	AddExhaust(th_rcs[11],ATTHEIGHT,ATTWIDTH, exhaustTex);

	// B4U
	th_rcs[12]=CreateThruster(_V(ATTCOOR,ATTCOOR2+0.3,TRANZ+RCSOFFSETM2-0.12+3.3), _V(0,-1,0), MaxThrust, ph_RCSB, RCSISP, RCSISP);
	AddExhaust(th_rcs[12],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// B4F
	th_rcs[13]=CreateThruster(_V(ATTCOOR-.001,ATTCOOR2-0.18,TRANZ+RCSOFFSET+3.10), _V(0,0,-1), MaxThrust, ph_RCSB, RCSISP, RCSISP);
	AddExhaust(th_rcs[13],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// A4R
	th_rcs[14]=CreateThruster(_V(ATTCOOR+0.30,ATTCOOR2-0.18,TRANZ+RCSOFFSETM2+3.17), _V(-1,0,0), MaxThrust, ph_RCSA, RCSISP, RCSISP);
	AddExhaust(th_rcs[14],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// A4D
	th_rcs[15]=CreateThruster(_V(ATTCOOR,ATTCOOR2-.55,TRANZ+RCSOFFSETM2-0.12+3.3), _V(0,1,0), MaxThrust, ph_RCSA, RCSISP, RCSISP);
	AddExhaust(th_rcs[15],ATTHEIGHT,ATTWIDTH, exhaustTex);
}

void LEM::SystemsInit()

{
	Panelsdk.RegisterVessel(this);
	Panelsdk.InitFromFile("ProjectApollo/LEMSystems");

	// DS20060407 Start wiring things together

	// Batteries
	Battery1 = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:DSC_BATTERY_A");
	Battery2 = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:DSC_BATTERY_B");
	Battery3 = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:DSC_BATTERY_C");
	Battery4 = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:DSC_BATTERY_D");
	Battery5 = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:ASC_BATTERY_A");
	Battery6 = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:ASC_BATTERY_B");
	//LunarBattery = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:LUNAR_BATTERY");  //Lunar battery not used on early LM's and will stay commented out until a J Mission LM is created
	EDBatteryA = (Battery *)Panelsdk.GetPointerByString("ELECTRIC:BATTERY_ED_A");
	EDBatteryB = (Battery *)Panelsdk.GetPointerByString("ELECTRIC:BATTERY_ED_B");

	// Batteries 1-4 and the Lunar Stay Battery are jettisoned with the descent stage.

	// ECA #1 (DESCENT stage, LMP 28V DC bus)
	ECA_1a.Init(this, Battery1, 2); // Battery 1 starts on LV
	ECA_1b.Init(this, Battery2, 0);
	ECA_1a.dc_source_tb = &DSCBattery1TB;
	ECA_1a.dc_source_tb->SetState(2); // Initialize to LV
	ECA_1b.dc_source_tb = &DSCBattery2TB;
	ECA_1b.dc_source_tb->SetState(0); // Initialize to off

	// ECA #2 (DESCENT stage, CDR 28V DC bus)
	ECA_2a.Init(this, Battery3, 0);
	ECA_2b.Init(this, Battery4, 2); 
	ECA_2a.dc_source_tb = &DSCBattery3TB;
	ECA_2a.dc_source_tb->SetState(0); 
	ECA_2b.dc_source_tb = &DSCBattery4TB;
	ECA_2b.dc_source_tb->SetState(2);

	// ECA #1 and #2 are JETTISONED with the descent stage.
	// ECA #3 and #4 have no low voltage taps and can feed either bus.
	ECA_3a.Init(this, Battery5, 0);
	ECA_3b.Init(this, Battery5, 0);
	ECA_3a.dc_source_tb = &ASCBattery5ATB;
	ECA_3a.dc_source_tb->SetState(0); // Initialize to off
	ECA_3b.dc_source_tb = &ASCBattery5BTB;
	ECA_3b.dc_source_tb->SetState(0); // Initialize to off
	ECA_4a.Init(this, Battery6, 0);
	ECA_4b.Init(this, Battery6, 0);
	ECA_4a.dc_source_tb = &ASCBattery6ATB;
	ECA_4a.dc_source_tb->SetState(0); // Initialize to off
	ECA_4b.dc_source_tb = &ASCBattery6BTB;
	ECA_4b.dc_source_tb->SetState(0); // Initialize to off

	// Descent Stage Deadface Bus Stubs wire to the ECAs
	// stage is not defined here, so we can't do this.

	// Bus Tie Blocks (Not real objects)
	BTB_LMP_B.Init(this,&DES_LMPs28VBusA,&ECA_4b);
	BTB_LMP_C.Init(this,&DES_LMPs28VBusB,&ECA_3a);
	BTB_CDR_B.Init(this,&DES_CDRs28VBusA,&ECA_3b);
	BTB_CDR_C.Init(this,&DES_CDRs28VBusB,&ECA_4a);

	// Bus feed tie breakers are sourced from the descent busses AND from ECA 3/4
	// via ficticious Bus Tie Blocks
	LMPBatteryFeedTieCB1.MaxAmps = 100.0;
	LMPBatteryFeedTieCB1.WireTo(&BTB_LMP_B);
	LMPBatteryFeedTieCB2.MaxAmps = 100.0;
	LMPBatteryFeedTieCB2.WireTo(&BTB_LMP_C);
	CDRBatteryFeedTieCB1.MaxAmps = 100.0;
	CDRBatteryFeedTieCB1.WireTo(&BTB_CDR_B);
	CDRBatteryFeedTieCB2.MaxAmps = 100.0;
	CDRBatteryFeedTieCB2.WireTo(&BTB_CDR_C);

	// Set up XLunar system
	BTC_XLunar.Init(this);

	// Main busses can be fed from the ECAs via the BAT FEED TIE CBs,
	// the other bus via the CROSS TIE BUS / CROSS TIE BAL LOADS CBs,
	// or the CSM via the XLUNAR bus floating ground return.
	// At this point, we have the sum of all battery feeds on BTB D and the XLUNAR feeds on BTB A
	BTB_CDR_D.Init(this,&CDRBatteryFeedTieCB1,&CDRBatteryFeedTieCB2); // Tie dual CBs together for CDR bus
	BTB_LMP_D.Init(this,&LMPBatteryFeedTieCB1,&LMPBatteryFeedTieCB2); // Tie dual CBs together for LMP bus

	BTB_CDR_A.Init(this,&BTB_CDR_D,&BTC_XLunar.dc_output);		      // Tie batteries and XLUNAR feed for CDR bus
	BTB_LMP_A.Init(this,&BTB_LMP_D,NULL);							  // Not really necessary but keeps things even

	// Bus cross-tie breakers
	CDRCrossTieBalCB.MaxAmps = 30.0;
	CDRCrossTieBalCB.WireTo(&CDRs28VBus);
	CDRCrossTieBusCB.MaxAmps = 100.0;
	CDRCrossTieBusCB.WireTo(&CDRs28VBus);
	LMPCrossTieBalCB.MaxAmps = 30.0;
	LMPCrossTieBalCB.WireTo(&LMPs28VBus);
	LMPCrossTieBusCB.MaxAmps = 100.0;
	LMPCrossTieBusCB.WireTo(&LMPs28VBus);

	// Bus cross-tie setup: Wire the two busses to the multiplexer and tell it
	// where the CBs are. The multiplexer will do the rest.
	BTC_MPX.Init(this,&LMPs28VBus,&CDRs28VBus,&LMPCrossTieBalCB,&LMPCrossTieBusCB,&CDRCrossTieBalCB,&CDRCrossTieBusCB);
	
	// At this point, the sum of bus feeds are on BTB A, and the cross-tie sources are on the mpx.

	// Join cross-ties and main-ties for bus source
	BTB_LMP_E.Init(this,&BTB_LMP_A,&BTC_MPX.dc_output_lmp);
	BTB_CDR_E.Init(this,&BTB_CDR_A,&BTC_MPX.dc_output_cdr);

	// EPS Displays
	EPS_DISP_CB.MaxAmps = 2.0;
	EPS_DISP_CB.WireTo(&LMPs28VBus);
	EPSMonitorSelectRotary.WireTo(&EPS_DISP_CB);

	// Descent battery TBs
	DSCBattery1TB.WireTo(&EPS_DISP_CB);
	DSCBattery2TB.WireTo(&EPS_DISP_CB);
	DSCBattery3TB.WireTo(&EPS_DISP_CB);
	DSCBattery4TB.WireTo(&EPS_DISP_CB);
	DSCBattFeedTB.WireTo(&EPS_DISP_CB);
	// Ascent battery TBs
	ASCBattery5ATB.WireTo(&EPS_DISP_CB);
	ASCBattery5BTB.WireTo(&EPS_DISP_CB);
	ASCBattery6ATB.WireTo(&EPS_DISP_CB);
	ASCBattery6BTB.WireTo(&EPS_DISP_CB);

	// ECA stuff
	CDRAscECAContCB.MaxAmps = 3.0;
	CDRAscECAContCB.WireTo(&CDRs28VBus);
	CDRAscECAMainCB.MaxAmps = 2.0;
	CDRAscECAMainCB.WireTo(&CDRs28VBus);
	CDRDesECAContCB.MaxAmps = 5.0;
	CDRDesECAContCB.WireTo(&CDRs28VBus);
	CDRDesECAMainCB.MaxAmps = 3.0;
	CDRDesECAMainCB.WireTo(&CDRs28VBus);
	LMPAscECAContCB.MaxAmps = 3.0;
	LMPAscECAContCB.WireTo(&LMPs28VBus);
	LMPAscECAMainCB.MaxAmps = 2.0;
	LMPAscECAMainCB.WireTo(&LMPs28VBus);
	LMPDesECAContCB.MaxAmps = 5.0;
	LMPDesECAContCB.WireTo(&LMPs28VBus);
	LMPDesECAMainCB.MaxAmps = 3.0;
	LMPDesECAMainCB.WireTo(&LMPs28VBus);

	// REACTION CONTROL SYSTEM
	RCS_B_PQGS_DISP_CB.MaxAmps = 2.0;
	RCS_B_PQGS_DISP_CB.WireTo(&LMPs28VBus);
	LMRCSATempInd.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	LMRCSBTempInd.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	LMRCSAPressInd.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	LMRCSBPressInd.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	LMRCSAQtyInd.WireTo(&RCS_B_PQGS_DISP_CB);
	LMRCSBQtyInd.WireTo(&RCS_B_PQGS_DISP_CB);
	
	// CDR and LMP 28V DC busses.
	// Wire to ficticious bus tie block
	CDRs28VBus.WireTo(&BTB_CDR_E); 
	LMPs28VBus.WireTo(&BTB_LMP_E);

	// DC voltmeter CBs
	CDRDCBusVoltCB.MaxAmps = 2.0;
	CDRDCBusVoltCB.WireTo(&CDRs28VBus);
	LMPDCBusVoltCB.MaxAmps = 2.0;
	LMPDCBusVoltCB.WireTo(&LMPs28VBus);

	// AC Inverter CBs
	CDRInverter1CB.MaxAmps = 30.0;
	CDRInverter1CB.WireTo(&CDRs28VBus);
	LMPInverter2CB.MaxAmps = 30.0;
	LMPInverter2CB.WireTo(&LMPs28VBus);
	// AC Inverters
	INV_1.dc_input = &CDRInverter1CB;	
	INV_2.dc_input = &LMPInverter2CB; 	
	// AC bus voltmeter breaker
	AC_A_BUS_VOLT_CB.MaxAmps = 2.0;
	AC_A_BUS_VOLT_CB.WireTo(&ACBusA);
	// AC bus input breakers
	AC_A_INV_1_FEED_CB.MaxAmps = 5.0;
	AC_A_INV_1_FEED_CB.WireTo(&INV_1);
	AC_B_INV_1_FEED_CB.MaxAmps = 5.0;
	AC_B_INV_1_FEED_CB.WireTo(&INV_1);
	AC_A_INV_2_FEED_CB.MaxAmps = 5.0;
	AC_A_INV_2_FEED_CB.WireTo(&INV_2);
	AC_B_INV_2_FEED_CB.MaxAmps = 5.0;
	AC_B_INV_2_FEED_CB.WireTo(&INV_2);

	// AC busses
	ACBusA.Disconnect();
	ACBusB.Disconnect();
	// Situation load will wire these to their breakers later if needed

	// AC bus attenuator.
	ACVoltsAttenuator.WireTo(&AC_A_BUS_VOLT_CB);

	// RCS valves
	RCSMainSovASwitch.WireTo(&RCS_A_MAIN_SOV_CB);
	RCSMainSovBSwitch.WireTo(&RCS_B_MAIN_SOV_CB);
	RCSMainSovATB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	RCSMainSovBTB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);

	RCSQuad1ACmdEnableSwitch.WireTo(&RCS_A_ISOL_VLV_CB);
	RCSQuad2ACmdEnableSwitch.WireTo(&RCS_A_ISOL_VLV_CB);
	RCSQuad3ACmdEnableSwitch.WireTo(&RCS_A_ISOL_VLV_CB);
	RCSQuad4ACmdEnableSwitch.WireTo(&RCS_A_ISOL_VLV_CB);
	RCSQuad1BCmdEnableSwitch.WireTo(&RCS_B_ISOL_VLV_CB);
	RCSQuad2BCmdEnableSwitch.WireTo(&RCS_B_ISOL_VLV_CB);
	RCSQuad3BCmdEnableSwitch.WireTo(&RCS_B_ISOL_VLV_CB);
	RCSQuad4BCmdEnableSwitch.WireTo(&RCS_B_ISOL_VLV_CB);
	RCSQuad1ACmdEnableTB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	RCSQuad2ACmdEnableTB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	RCSQuad3ACmdEnableTB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	RCSQuad4ACmdEnableTB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	RCSQuad1BCmdEnableTB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	RCSQuad2BCmdEnableTB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	RCSQuad3BCmdEnableTB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	RCSQuad4BCmdEnableTB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);

	RCSAscFeed1ASwitch.WireTo(&RCS_A_ASC_FEED_1_CB);
	RCSAscFeed2ASwitch.WireTo(&RCS_A_ASC_FEED_2_CB);
	RCSAscFeed1BSwitch.WireTo(&RCS_B_ASC_FEED_1_CB);
	RCSAscFeed2BSwitch.WireTo(&RCS_B_ASC_FEED_2_CB);
	RCSAscFeed1ATB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	RCSAscFeed2ATB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	RCSAscFeed1BTB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	RCSAscFeed2BTB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	RCSXFeedSwitch.WireTo(&RCS_B_CRSFD_CB);
	RCSXFeedTB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);

	// Lighting
	CDR_LTG_UTIL_CB.MaxAmps = 2.0;
	CDR_LTG_UTIL_CB.WireTo(&CDRs28VBus);
	CDR_LTG_ANUN_DOCK_COMPNT_CB.MaxAmps = 2.0;
	CDR_LTG_ANUN_DOCK_COMPNT_CB.WireTo(&CDRs28VBus);
	LTG_ANUN_DOCK_COMPNT_CB.MaxAmps = 2.0;
	LTG_ANUN_DOCK_COMPNT_CB.WireTo(&LMPs28VBus);
	LTG_FLOOD_CB.MaxAmps = 5.0;
	LTG_FLOOD_CB.WireTo(&LMPs28VBus);
	NUM_LTG_AC_CB.MaxAmps = 2.0;
	NUM_LTG_AC_CB.WireTo(&ACBusB);

	// LGC and DSKY
	LGC_DSKY_CB.MaxAmps = 7.5;
	LGC_DSKY_CB.WireTo(&CDRs28VBus);
	agc.WirePower(&LGC_DSKY_CB, NULL);
	// The DSKY brightness IS controlled by the ANUN/NUM knob on panel 5, but by means of an isolated section of it.
	// The source of the isolated section is coming from the LGC supply.
	dsky.Init(&LGC_DSKY_CB, &LtgAnunNumKnob);

	// AGS stuff
	asa.Init(this, &AGSOperateSwitch, (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:LEM-ASA-FastHeater"),
									  (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:LEM-ASA-FineHeater"),
									  (h_Radiator *)Panelsdk.GetPointerByString("HYDRAULIC:LEM-ASA-HSink"),
									  (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:ASAHEAT"));

	aea.Init(this, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:AEAHEAT"), (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:SECAEAHEAT"));
	aea.WireToBuses(&CDR_SCS_AEA_CB, &SCS_AEA_CB, &AGSOperateSwitch);
	deda.Init(&SCS_AEA_CB);
	rga.Init(this, &SCS_ATCA_CB, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:RGAHEAT"), (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:SECRGAHEAT"));

	// Set up IMU heater stuff
	imucase = (h_Radiator *)Panelsdk.GetPointerByString("HYDRAULIC:LM-IMU-Case");
	imucase->isolation = 0.0000001;
	imucase->Area = 3165.31625; // Surface area of 12.5 inch diameter sphere in cm
	imuheater = (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:LM-IMU-Heater");
	imublower = (h_HeatExchanger *)Panelsdk.GetPointerByString("HYDRAULIC:IMUBLOWER");

	//IMU
	imu.WireToBuses(&IMU_OPR_CB, NULL, NULL);
	imu.WireHeaterToBuses(imuheater, &IMU_SBY_CB, NULL);
	imu.InitThermals((h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:IMUHEAT"), imucase);

	// Main Propulsion
	PROP_DISP_ENG_OVRD_LOGIC_CB.MaxAmps = 2.0;
	PROP_DISP_ENG_OVRD_LOGIC_CB.WireTo(&LMPs28VBus);
	THRUST_DISP_CB.MaxAmps = 2.0;
	THRUST_DISP_CB.WireTo(&CDRs28VBus);
	MainFuelTempInd.WireTo(&PROP_DISP_ENG_OVRD_LOGIC_CB);
	MainFuelPressInd.WireTo(&PROP_DISP_ENG_OVRD_LOGIC_CB);
	MainOxidizerTempInd.WireTo(&PROP_DISP_ENG_OVRD_LOGIC_CB);
	MainOxidizerPressInd.WireTo(&PROP_DISP_ENG_OVRD_LOGIC_CB);
	EngineThrustInd.WireTo(&THRUST_DISP_CB);
	CommandedThrustInd.WireTo(&THRUST_DISP_CB);
	DPSOxidPercentMeter.WireTo(&HE_PQGS_PROP_DISP_AC_CB);
	DPSFuelPercentMeter.WireTo(&HE_PQGS_PROP_DISP_AC_CB);
	MainHeliumPressureMeter.WireTo(&HE_PQGS_PROP_DISP_AC_CB);

	// The FDAI has two CBs, AC and DC, and both are 2 amp CBs
	// CDR FDAI
	CDR_FDAI_DC_CB.MaxAmps = 2.0;
	CDR_FDAI_DC_CB.WireTo(&CDRs28VBus);
	CDR_FDAI_AC_CB.MaxAmps = 2.0;
	CDR_FDAI_AC_CB.WireTo(&ACBusA);
	// And the CDR FDAI itself	
	fdaiLeft.WireTo(&CDR_FDAI_DC_CB,&CDR_FDAI_AC_CB);
	// LMP FDAI stuff
	LMP_EVT_TMR_FDAI_DC_CB.MaxAmps = 2.0;
	LMP_EVT_TMR_FDAI_DC_CB.WireTo(&LMPs28VBus);
	LMP_FDAI_AC_CB.MaxAmps = 2.0;
	LMP_FDAI_AC_CB.WireTo(&ACBusB);
	fdaiRight.WireTo(&LMP_EVT_TMR_FDAI_DC_CB,&LMP_FDAI_AC_CB);
	EventTimerDisplay.Init(&LMP_EVT_TMR_FDAI_DC_CB, NULL, &LtgAnunNumKnob, &NUM_LTG_AC_CB);

	// HEATERS
	TempMonitorInd.WireTo(&HTR_DISP_CB);

	// Landing Radar
	LR.Init(this, &PGNS_LDG_RDR_CB, (h_Radiator *)Panelsdk.GetPointerByString("HYDRAULIC:LEM-LR-Antenna"), (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:LEM-LR-Antenna-Heater"), (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:LRHEAT"));

	// Rdz Radar
	RR.Init(this, &PGNS_RNDZ_RDR_CB, &RDZ_RDR_AC_CB, (h_Radiator *)Panelsdk.GetPointerByString("HYDRAULIC:LEM-RR-Antenna"), (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:LEM-RR-Antenna-Heater"), (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:LEM-RR-Antenna-StbyHeater"), (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:RREHEAT"), (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:SECRREHEAT"));
	RadarTape.Init(this, &RNG_RT_ALT_RT_DC_CB, &RNG_RT_ALT_RT_AC_CB);
	crossPointerLeft.Init(this, &CDR_XPTR_CB, &LeftXPointerSwitch, &RateErrorMonSwitch);
	crossPointerRight.Init(this, &SE_XPTR_DC_CB, &RightXPointerSwitch, &RightRateErrorMonSwitch);
	// CWEA
	CWEA.Init(this);

	// COMM
	omni_fwd.Init(this);
	omni_aft.Init(this);
	// S-Band Steerable Ant
	SBandSteerable.Init(this, (h_Radiator *)Panelsdk.GetPointerByString("HYDRAULIC:LEM-SBand-Steerable-Antenna"), (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:LEM-SBand-Steerable-Antenna-Heater"));
	// SBand System
	SBand.Init(this, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:SBXHEAT"), (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:SECSBXHEAT"), (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:SBPHEAT"), (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:SECSBPHEAT"));
	// VHF System
	VHF.Init(this, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:VHFHEAT"), (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:SECVHFHEAT"), (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:PCMHEAT"), (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:SECPCMHEAT"));
	// CBs
	INST_SIG_CONDR_1_CB.MaxAmps = 2.0;
	INST_SIG_CONDR_1_CB.WireTo(&CDRs28VBus);
	INST_SIG_CONDR_2_CB.MaxAmps = 2.0;
	INST_SIG_CONDR_2_CB.WireTo(&LMPs28VBus);
	COMM_VHF_RCVR_A_CB.MaxAmps = 2.0;
	COMM_VHF_RCVR_A_CB.WireTo(&CDRs28VBus);
	COMM_VHF_XMTR_A_CB.MaxAmps = 5.0;
	COMM_VHF_XMTR_A_CB.WireTo(&LMPs28VBus);
	COMM_VHF_RCVR_B_CB.MaxAmps = 2.0;
	COMM_VHF_RCVR_B_CB.WireTo(&LMPs28VBus);
	COMM_VHF_XMTR_B_CB.MaxAmps = 5.0;
	COMM_VHF_XMTR_B_CB.WireTo(&CDRs28VBus);
	COMM_PRIM_SBAND_PA_CB.MaxAmps = 7.5;
	COMM_PRIM_SBAND_PA_CB.WireTo(&LMPs28VBus);
	COMM_PRIM_SBAND_XCVR_CB.MaxAmps = 5.0;
	COMM_PRIM_SBAND_XCVR_CB.WireTo(&LMPs28VBus);
	COMM_SEC_SBAND_PA_CB.MaxAmps = 7.5;
	COMM_SEC_SBAND_PA_CB.WireTo(&CDRs28VBus);
	COMM_SEC_SBAND_XCVR_CB.MaxAmps = 5.0;
	COMM_SEC_SBAND_XCVR_CB.WireTo(&CDRs28VBus);
	COMM_CDR_AUDIO_CB.MaxAmps = 2.0;
	COMM_CDR_AUDIO_CB.WireTo(&CDRs28VBus);
	COMM_SE_AUDIO_CB.MaxAmps = 2.0;
	COMM_SE_AUDIO_CB.WireTo(&LMPs28VBus);
	INST_CWEA_CB.MaxAmps = 2.0;
	INST_CWEA_CB.WireTo(&LMPs28VBus);
	INST_SIG_SENSOR_CB.MaxAmps = 2.0;
	INST_SIG_SENSOR_CB.WireTo(&LMPs28VBus);
	INST_PCMTEA_CB.MaxAmps = 2.0;
	INST_PCMTEA_CB.WireTo(&LMPs28VBus);
	COMM_PMP_CB.MaxAmps = 2.0;
	COMM_PMP_CB.WireTo(&LMPs28VBus);
	COMM_SBAND_ANT_CB.MaxAmps = 2.0;
	COMM_SBAND_ANT_CB.WireTo(&LMPs28VBus);
	COMM_DISP_CB.MaxAmps = 2.0;
	COMM_DISP_CB.WireTo(&LMPs28VBus);

	// EXPLOSIVE DEVICES SUPPLY CBs
	EDS_CB_LG_FLAG.MaxAmps = 2.0;
	EDS_CB_LG_FLAG.WireTo(&CDRs28VBus);
	EDS_CB_LOGIC_A.MaxAmps = 2.0;
	EDS_CB_LOGIC_A.WireTo(&CDRs28VBus);
	EDS_CB_LOGIC_B.MaxAmps = 2.0;
	EDS_CB_LOGIC_B.WireTo(&LMPs28VBus);

	// EXPLOSIVE DEVICES SYSTEMS
	EDLGTB.WireTo(&EDS_CB_LG_FLAG);

	// ABORT GUIDANCE SYSTEM
	SCS_ASA_CB.MaxAmps = 20.0;
	SCS_ASA_CB.WireTo(&LMPs28VBus);
	SCS_AEA_CB.MaxAmps = 10.0;
	SCS_AEA_CB.WireTo(&LMPs28VBus);
	CDR_SCS_AEA_CB.MaxAmps = 10.0;
	CDR_SCS_AEA_CB.WireTo(&CDRs28VBus);
	SCS_ATCA_AGS_CB.MaxAmps = 3.0;
	SCS_ATCA_AGS_CB.WireTo(&LMPs28VBus);

	// ENVIRONMENTAL CONTROL SYSTEM
	LMSuitTempMeter.WireTo(&ECS_DISP_CB);
	LMCabinTempMeter.WireTo(&ECS_DISP_CB);
	LMSuitPressMeter.WireTo(&ECS_DISP_CB);
	LMCabinPressMeter.WireTo(&ECS_DISP_CB);
	LMCO2Meter.WireTo(&ECS_DISP_CB);
	LMGlycolTempMeter.WireTo(&ECS_DISP_CB);
	LMGlycolPressMeter.WireTo(&ECS_DISP_CB);
	LMOxygenQtyMeter.WireTo(&ECS_DISP_CB);
	LMWaterQtyMeter.WireTo(&ECS_DISP_CB);

	CrewInCabin = (h_crew *)Panelsdk.GetPointerByString("HYDRAULIC:CREW");
	CDRSuited = (h_crew *)Panelsdk.GetPointerByString("HYDRAULIC:CDRSUITED");
	LMPSuited = (h_crew *)Panelsdk.GetPointerByString("HYDRAULIC:LMPSUITED");

	//Initialize LM ECS
	DesO2Tank = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:DESO2TANK");
	AscO2Tank1 = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:ASCO2TANK1");
	AscO2Tank2 = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:ASCO2TANK2");
	DesO2Manifold = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:DESO2MANIFOLD");
	O2Manifold = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:O2MANIFOLD");
	PressRegA = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGA");
	PressRegB = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGB");
	DesH2OTank = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:DESH2OTANK");
	DesBatCooling = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:DESBATCOOLING");
	CabinFan1 = (Pump *)Panelsdk.GetPointerByString("ELECTRIC:CABINFAN");
	SuitFan1 = (Pump *)Panelsdk.GetPointerByString("ELECTRIC:SUITFAN1");
	SuitFan2 = (Pump *)Panelsdk.GetPointerByString("ELECTRIC:SUITFAN2");
	PrimGlyPump1 = (Pump *)Panelsdk.GetPointerByString("ELECTRIC:PRIMGLYCOLPUMP1");
	PrimGlyPump2 = (Pump *)Panelsdk.GetPointerByString("ELECTRIC:PRIMGLYCOLPUMP2");
	SecGlyPump = (Pump *)Panelsdk.GetPointerByString("ELECTRIC:SECGLYCOLPUMP");
	LCGPump = (Pump *)Panelsdk.GetPointerByString("ELECTRIC:LCGPUMP");

	PrimGlyPump1->WireTo(&ECS_GLYCOL_PUMP_1_CB);
	PrimGlyPump2->WireTo(&ECS_GLYCOL_PUMP_2_CB);
	CabinFan1->WireTo(&ECS_CABIN_FAN_1_CB);
	SuitFan1->WireTo(&ECS_SUIT_FAN_1_CB);
	SuitFan2->WireTo(&ECS_SUIT_FAN_2_CB);
	SecGlyPump->WireTo(&ECS_GLYCOL_PUMP_SEC_CB);
	LCGPump->WireTo(&ECS_LGC_PUMP_CB);

	//Initialize LM ECS
	DesO2Tank->BoilAllAndSetTemp(294.261);
	AscO2Tank1->BoilAllAndSetTemp(294.261);
	AscO2Tank2->BoilAllAndSetTemp(294.261);
	DesO2Manifold->BoilAllAndSetTemp(294.261);
	O2Manifold->BoilAllAndSetTemp(294.261);
	PressRegA->BoilAllAndSetTemp(285.928);
	PressRegB->BoilAllAndSetTemp(285.928);

	//Tunnel Connection Initialization   

	SetPipeMaxFlow("HYDRAULIC:LMTUNNELUNDOCKED", 1000.0 / LBH);

	//Oxygen Pipe Initialization   

	SetPipeMaxFlow("HYDRAULIC:DESO2PIPE1", 660.0 / LBH);
	SetPipeMaxFlow("HYDRAULIC:DESO2PIPE2", 660.0 / LBH);
	SetPipeMaxFlow("HYDRAULIC:PRESSREGAIN", 6.75 / LBH);
	SetPipeMaxFlow("HYDRAULIC:PRESSREGBIN", 6.75 / LBH);
	SetPipeMaxFlow("HYDRAULIC:PRESSREGAOUT", 6.75 / LBH);
	SetPipeMaxFlow("HYDRAULIC:PRESSREGBOUT", 6.75 / LBH);
	SetPipeMaxFlow("HYDRAULIC:SUITCIRCUITRELIEFVALVE", 7.8 / LBH);


	//****Need to go through these and remove any unnecessary ones****
/*
	SetPipeMaxFlow("HYDRAULIC:ASC1O2PIPE", 6.75 / LBH);
	SetPipeMaxFlow("HYDRAULIC:ASC2O2PIPE", 6.75 / LBH);
	SetPipeMaxFlow("HYDRAULIC:PLSSO2FILL", 6.75 / LBH);
	SetPipeMaxFlow("HYDRAULIC:SUITCIRCUITOUT", 6.75 / LBH);
	SetPipeMaxFlow("HYDRAULIC:SUITGASDIVERTERCABINOUT", 6.75 / LBH);
	SetPipeMaxFlow("HYDRAULIC:SUITGASDIVERTEREGRESSOUT", 6.75 / LBH);
	SetPipeMaxFlow("HYDRAULIC:CABINGASRETURN", 6.75 / LBH);
	SetPipeMaxFlow("HYDRAULIC:PRIMCO2INLET", 6.75 / LBH);
	SetPipeMaxFlow("HYDRAULIC:SECCO2INLET", 6.75 / LBH);
*/

	//Primary Glycol Pipe Initialization   
	SetPipeMaxFlow("HYDRAULIC:PRIMGLYPUMPMANIFOLDOUT1", 120.0 / LBH);
	SetPipeMaxFlow("HYDRAULIC:PRIMGLYPUMPMANIFOLDOUT2", 170.0 / LBH);
	SetPipeMaxFlow("HYDRAULIC:PRIMGLYCOLCOOLINGOUT", 120.0 / LBH);
	SetPipeMaxFlow("HYDRAULIC:HXFLOWCONTROL", 120.0 / LBH);
	SetPipeMaxFlow("HYDRAULIC:HXFLOWCONTROLBYPASS", 290.0 / LBH);
	SetPipeMaxFlow("HYDRAULIC:HXHOUTFLOW", 120.0 / LBH);
	SetPipeMaxFlow("HYDRAULIC:ASCBATINLET", 116.0 / LBH);
	SetPipeMaxFlow("HYDRAULIC:DESBATINLET", 174.0 / LBH); 

	//Secondary Glycol Pipe Initialization  
	SetPipeMaxFlow("HYDRAULIC:SECGLYFLOWREG2", 300.0 / LBH);

	//LCG Pipe Initialization   
	SetPipeMaxFlow("HYDRAULIC:LCGACCUMULATOROUT", 240.0 / LBH);
	SetPipeMaxFlow("HYDRAULIC:LCGFLOWCONTROLBYPASS", 240.0 / LBH);
	SetPipeMaxFlow("HYDRAULIC:LCGFLOWCONTROL", 240.0 / LBH);

	// Mission timer.
	MISSION_TIMER_CB.MaxAmps = 2.0;
	MISSION_TIMER_CB.WireTo(&CDRs28VBus);
	MissionTimerDisplay.Init(&MISSION_TIMER_CB, NULL, &LtgAnunNumKnob, &NUM_LTG_AC_CB);

	// Pyro Buses
	Panelsdk.AddElectrical(&ED28VBusA, false);
	Panelsdk.AddElectrical(&ED28VBusB, false);

	// Pyros
	LandingGearPyros.WireTo(&LandingGearPyrosFeeder);
	CableCuttingPyros.WireTo(&CableCuttingPyrosFeeder);
	DescentPropVentPyros.WireTo(&DescentPropVentPyrosFeeder);
	DescentEngineStartPyros.WireTo(&DescentEngineStartPyrosFeeder);
	DescentEngineOnPyros.WireTo(&DescentEngineOnPyrosFeeder);
	DescentPropIsolPyros.WireTo(&DescentPropIsolPyrosFeeder);
	AscentHeliumIsol1Pyros.WireTo(&AscentHeliumIsol1PyrosFeeder);
	AscentHeliumIsol2Pyros.WireTo(&AscentHeliumIsol2PyrosFeeder);
	AscentOxidCompValvePyros.WireTo(&AscentOxidCompValvePyrosFeeder);
	AscentFuelCompValvePyros.WireTo(&AscentFuelCompValvePyrosFeeder);
	RCSHeliumSupplyAPyros.WireTo(&RCSHeliumSupplyAPyrosFeeder);
	RCSHeliumSupplyBPyros.WireTo(&RCSHeliumSupplyBPyrosFeeder);

	// Update ECA ties
	Panelsdk.AddElectrical(&DES_CDRs28VBusA, false);
	Panelsdk.AddElectrical(&DES_CDRs28VBusB, false);
	Panelsdk.AddElectrical(&DES_LMPs28VBusA, false); 
	Panelsdk.AddElectrical(&DES_LMPs28VBusB, false); 

	// Arrange for updates of ECAs
	Panelsdk.AddElectrical(&ECA_1a, false);
	Panelsdk.AddElectrical(&ECA_1b, false);
	Panelsdk.AddElectrical(&ECA_2a, false);
	Panelsdk.AddElectrical(&ECA_2b, false);
	Panelsdk.AddElectrical(&ECA_3a, false);
	Panelsdk.AddElectrical(&ECA_3b, false);
	Panelsdk.AddElectrical(&ECA_4a, false);
	Panelsdk.AddElectrical(&ECA_4b, false);

	// Arrange for updates of tie points and bus balancer

	// Sum of ascent and descent feed lines
	Panelsdk.AddElectrical(&BTB_CDR_B, false);
	Panelsdk.AddElectrical(&BTB_CDR_C, false);
	Panelsdk.AddElectrical(&BTB_LMP_B, false);
	Panelsdk.AddElectrical(&BTB_LMP_C, false);

	// XLUNAR source
	Panelsdk.AddElectrical(&BTC_XLunar, false);
	// Sum of battery feed ties
	Panelsdk.AddElectrical(&BTB_CDR_D, false);
	Panelsdk.AddElectrical(&BTB_LMP_D, false);

	// Sum of BTB-D and XLUNAR power
	Panelsdk.AddElectrical(&BTB_CDR_A, false);
	Panelsdk.AddElectrical(&BTB_LMP_A, false);
	// The multiplexer will update the main 28V busses
	Panelsdk.AddElectrical(&BTC_MPX, false);

	// Sum of BTB-A and bus cross-tie-balancer
	Panelsdk.AddElectrical(&BTB_CDR_E, false);
	Panelsdk.AddElectrical(&BTB_LMP_E, false);

	// Arrange for updates of main busses, AC inverters, and the bus balancer
	Panelsdk.AddElectrical(&ACBusA, false);
	Panelsdk.AddElectrical(&ACBusB, false);
	Panelsdk.AddElectrical(&ACVoltsAttenuator, false);
	Panelsdk.AddElectrical(&INV_1, false);
	Panelsdk.AddElectrical(&INV_2, false);

	// ECS
	CabinRepressValve.Init((h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:CABINREPRESS"),
		&ECS_CABIN_REPRESS_CB, &CabinRepressValveSwitch, &PressRegAValve, &PressRegBValve);
	SuitCircuitPressureRegulatorA.Init((h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGAOUT"),
		(h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT"), &PressRegAValve);
	SuitCircuitPressureRegulatorB.Init((h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGBOUT"),
		(h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT"), &PressRegBValve);
	OverheadHatch.Init(this, &UpperHatchHandle);
	OVHDCabinReliefDumpValve.Init((h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:CABINOVHDHATCHVALVE"),
		&UpperHatchReliefValve, &OverheadHatch);
	ForwardHatch.Init(this, &ForwardHatchHandle);
	FWDCabinReliefDumpValve.Init((h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:CABINFWDHATCHVALVE"),
		&ForwardHatchReliefValve, &ForwardHatch);
	SuitCircuitReliefValve.Init((h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRELIEFVALVE"),
		&SuitCircuitReliefValveSwitch);
	SuitGasDiverter.Init((h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SUITGASDIVERTER"),
		(h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:CABIN"),
		&SuitGasDiverterSwitch, &ECS_DIVERT_VLV_CB, &PressRegAValve, &PressRegBValve);
	CabinGasReturnValve.Init((h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:CABINGASRETURN"),
		&CabinGasReturnValveSwitch);
	CO2CanisterSelect.Init((h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2CANISTER"),
		(h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SECCO2CANISTER"),
		&CO2CanisterSelectSwitch);
	PrimCO2CanisterVent.Init((h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2CANISTER"),
		&CO2CanisterPrimVent);
	SecCO2CanisterVent.Init((h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SECCO2CANISTER"),
		&CO2CanisterSecVent);
	WaterSeparationSelector.Init((h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERCOOLING"),
		&WaterSepSelectSwitch);
	CabinFan.Init(&ECS_CABIN_FAN_1_CB, &ECS_CABIN_FAN_CONT_CB, &PressRegAValve, &PressRegBValve, (Pump *)Panelsdk.GetPointerByString("ELECTRIC:CABINFAN"));
	WaterTankSelect.Init((h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:H2OTANKSELECT"),
		(h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:H2OSURGETANK"),
		&WaterTankSelectValve);
	PrimGlycolPumpController.Init((h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLACCUMULATOR"),
		(h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLPUMPMANIFOLD"),
		(Pump *)Panelsdk.GetPointerByString("ELECTRIC:PRIMGLYCOLPUMP1"),
		(Pump *)Panelsdk.GetPointerByString("ELECTRIC:PRIMGLYCOLPUMP2"),
		&GlycolRotary, &ECS_GLYCOL_PUMP_1_CB, &ECS_GLYCOL_PUMP_2_CB, &ECS_GLYCOL_PUMP_AUTO_XFER_CB);
	SuitFanDPSensor.Init((h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SUITFANMANIFOLD"),
		(h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERCOOLING"),
		&ECS_SUIT_FAN_DP_CB);
	ecs.Init(this);

	// EDS initialization
	eds.Init(this);
	EDDesFuelVent.WireTo(&PROP_DES_HE_REG_VENT_CB);
	EDDesOxidVent.WireTo(&PROP_DES_HE_REG_VENT_CB);
	EDDesFuelVentTB.WireTo(&PROP_DISP_ENG_OVRD_LOGIC_CB);
	EDDesOxidVentTB.WireTo(&PROP_DISP_ENG_OVRD_LOGIC_CB);

	// S&C Control Assemblies
	scca1.Init(this);
	scca2.Init(this);
	scca3.Init(this);

	// DPS and APS
	DPSPropellant.Init(&PROP_PQGS_CB);
	DPS.Init(this);
	DPS.pitchGimbalActuator.Init(this, &EngGimbalEnableSwitch, &DECA_GMBL_AC_CB);
	DPS.rollGimbalActuator.Init(this, &EngGimbalEnableSwitch, &DECA_GMBL_AC_CB);
	DESHeReg1Switch.WireTo(&PROP_DES_HE_REG_VENT_CB);
	DESHeReg2Switch.WireTo(&PROP_DES_HE_REG_VENT_CB);
	DESHeReg1TB.WireTo(&PROP_DISP_ENG_OVRD_LOGIC_CB);
	DESHeReg2TB.WireTo(&PROP_DISP_ENG_OVRD_LOGIC_CB);
	APS.Init(this);
	ASCHeReg1Switch.WireTo(&PROP_ASC_HE_REG_CB);
	ASCHeReg2Switch.WireTo(&PROP_ASC_HE_REG_CB);
	ASCHeReg1TB.WireTo(&PROP_DISP_ENG_OVRD_LOGIC_CB);
	ASCHeReg2TB.WireTo(&PROP_DISP_ENG_OVRD_LOGIC_CB);

	//RCS
	RCSA.Init(th_rcs, &RCSHeliumSupplyAPyros, &RCSB, 0, 6, 8, 14);
	RCSB.Init(th_rcs, &RCSHeliumSupplyBPyros, &RCSA, 2, 4, 10, 12);
	tca1A.Init(this, 7);
	tca2A.Init(this, 5);
	tca3A.Init(this, 3);
	tca4A.Init(this, 1);
	tca1B.Init(this, 8);
	tca2B.Init(this, 6);
	tca3B.Init(this, 4);
	tca4B.Init(this, 2);

	RCSHtr1Quad1 = (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:QUAD1HTRSYS1");
	RCSHtr1Quad2 = (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:QUAD2HTRSYS1");
	RCSHtr1Quad3 = (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:QUAD3HTRSYS1");
	RCSHtr1Quad4 = (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:QUAD4HTRSYS1");
	RCSHtr1Quad1->WireTo(&RCS_QUAD_1_CDR_HTR_CB);
	RCSHtr1Quad2->WireTo(&RCS_QUAD_2_CDR_HTR_CB);
	RCSHtr1Quad3->WireTo(&RCS_QUAD_3_CDR_HTR_CB);
	RCSHtr1Quad4->WireTo(&RCS_QUAD_4_CDR_HTR_CB);

	RCSHtr2Quad1 = (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:QUAD1HTRSYS2");
	RCSHtr2Quad2 = (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:QUAD2HTRSYS2");
	RCSHtr2Quad3 = (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:QUAD3HTRSYS2");
	RCSHtr2Quad4 = (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:QUAD4HTRSYS2");
	RCSHtr2Quad1->WireTo(&RCS_QUAD_1_LMP_HTR_CB);
	RCSHtr2Quad2->WireTo(&RCS_QUAD_2_LMP_HTR_CB);
	RCSHtr2Quad3->WireTo(&RCS_QUAD_3_LMP_HTR_CB);
	RCSHtr2Quad4->WireTo(&RCS_QUAD_4_LMP_HTR_CB);

	LMQuad1RCS = (h_Radiator *)Panelsdk.GetPointerByString("HYDRAULIC:LMRCSQUAD1");
	LMQuad2RCS = (h_Radiator *)Panelsdk.GetPointerByString("HYDRAULIC:LMRCSQUAD2");
	LMQuad3RCS = (h_Radiator *)Panelsdk.GetPointerByString("HYDRAULIC:LMRCSQUAD3");
	LMQuad4RCS = (h_Radiator *)Panelsdk.GetPointerByString("HYDRAULIC:LMRCSQUAD4");

	//ACA and TTCA
	CDR_ACA.Init(this, &ACAPropSwitch);
	CDR_TTCA.Init(this);

	//DECA
	deca.Init(this, &SCS_DECA_PWR_CB);

	//GASTA
	gasta.Init(this, &GASTA_DC_CB, &GASTA_AC_CB, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:GASTAHEAT"), &imu);

	//ORDEAL
	ordeal.Init(&ORDEALEarthSwitch, &ORDEAL_AC_CB, &ORDEAL_DC_CB, &ORDEALAltSetRotary, &ORDEALModeSwitch, &ORDEALSlewSwitch, &ORDEALFDAI1Switch, &ORDEALFDAI2Switch);

	//LM Mission Programer
	lmp.Init(this);

	//Mechanical Accelerometer
	mechanicalAccelerometer.Init(this);

	//Instrumentation
	scera1.Init(this, &INST_SIG_CONDR_1_CB, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:SCERAHEAT"), (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:SECSCERAHEAT"));
	scera2.Init(this, &INST_SIG_CONDR_2_CB, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:SCERAHEAT"), (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:SECSCERAHEAT"));

	// DS20060413 Initialize joystick
	js_enabled = 0;  // Disabled
	rhc_id = -1;     // Disabled
	rhc_rot_id = -1; // Disabled
	rhc_sld_id = -1; // Disabled
	rhc_rzx_id = -1; // Disabled
	thc_id = -1;     // Disabled
	thc_rot_id = -1; // Disabled
	thc_sld_id = -1; // Disabled
	thc_rzx_id = -1; // Disabled
	thc_tjt_id = 0; // Disabled
	thc_debug = -1;
	rhc_debug = -1;
	rhc_thctoggle = false;
	rhc_thctoggle_id = -1;
	rhc_thctoggle_pressed = false;

	ttca_throttle_pos = 0;
	ttca_throttle_vel = 0;
	ttca_throttle_pos_dig = 0;
	
	// Initialize other systems
	atca.Init(this, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:ATCAHEAT"), (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:SECATCAHEAT"));
}

void LEM::JoystickTimestep(double simdt)
{
	// Joystick read
	if (oapiGetFocusInterface() == this) {

		// Invert joystick configuration according to navmode in case of one joystick
		int tmp_id, tmp_rot_id, tmp_sld_id, tmp_rzx_id, tmp_pov_id, tmp_debug;
		if (rhc_thctoggle && ((rhc_id != -1 && thc_id == -1 && GetAttitudeMode() == RCS_LIN) ||
			(rhc_id == -1 && thc_id != -1 && GetAttitudeMode() == RCS_ROT))) {

			tmp_id = rhc_id;
			tmp_rot_id = rhc_rot_id;
			tmp_sld_id = rhc_sld_id;
			tmp_rzx_id = rhc_rzx_id;
			tmp_pov_id = rhc_pov_id;
			tmp_debug = rhc_debug;

			rhc_id = thc_id;
			rhc_rot_id = thc_rot_id;
			rhc_sld_id = thc_sld_id;
			rhc_rzx_id = thc_rzx_id;
			rhc_pov_id = thc_pov_id;
			rhc_debug = thc_debug;

			thc_id = tmp_id;
			thc_rot_id = tmp_rot_id;
			thc_sld_id = tmp_sld_id;
			thc_rzx_id = tmp_rzx_id;
			thc_pov_id = tmp_pov_id;
			thc_debug = tmp_debug;
		}

		if (thc_id != -1 && !(thc_id < js_enabled)) {
			sprintf(oapiDebugString(), "DX8JS: Joystick selected as THC does not exist.");
		}
		if (rhc_id != -1 && !(rhc_id < js_enabled)) {
			sprintf(oapiDebugString(), "DX8JS: Joystick selected as RHC does not exist.");
		}

		// Zero ACA and TTCA bits in channel 31
		ChannelValue val31;
		val31 = agc.GetInputChannel(031);
		val31 &= 030000; // Leaves AttitudeHold and AutomaticStab alone

		int ttca_pos[3];
		int thc_x_pos = 0;
		int thc_y_pos = 0;
		int thc_z_pos = 0;
		int thc_rot_pos = 0;
		int thc_tjt_pos = 32768; // Initialize to centered
		bool ttca_realistic_throttle = false;

		int rhc_pos[3];     // RHC x/y/z positions
		rhc_pos[0] = 32768; // Initialize
		rhc_pos[1] = 32768;
		rhc_pos[2] = 32768;


		/* ACA OPERATION:

		The LM ACA is a lot different from the CM RHC.
		The ACA works on a D/A converter.
		The OUT OF DETENT switch closes at .5 degrees of travel, and enables the proportional
		voltage circuit ("A" CIRCUIT) to operate.
		The hand controller must be moved 2 degrees to generate a count.
		8 degrees of usable travel, at .190 degrees per count.
		10 degrees total travel = 42 counts.

		*/
		// Axes have 32768 points of travel for the 13 degrees to hard stop
		// 2520 points per degree. It breaks out of detent at .5 degres, or 1260 pulses.
		// 480 points per count.

		// Read data
		HRESULT hr;
		// Handle RHC
		if (rhc_id != -1 && rhc_id < js_enabled) {
			// CHECK FOR POWER HERE
			hr = dx8_joystick[rhc_id]->Poll();
			if (FAILED(hr)) { // Did that work?
							  // Attempt to acquire the device
				hr = dx8_joystick[rhc_id]->Acquire();
				if (FAILED(hr)) {
					sprintf(oapiDebugString(), "DX8JS: Cannot aquire RHC");
				}
				else {
					hr = dx8_joystick[rhc_id]->Poll();
				}
			}
			dx8_joystick[rhc_id]->GetDeviceState(sizeof(dx8_jstate[rhc_id]), &dx8_jstate[rhc_id]);
			// Z-axis read.
			int rhc_rot_pos = 32768; // Initialize to centered
			if (rhc_rot_id != -1) { // If this is a rotator-type axis
				switch (rhc_rot_id) {
				case 0:
					rhc_rot_pos = dx8_jstate[rhc_id].lRx; break;
				case 1:
					rhc_rot_pos = dx8_jstate[rhc_id].lRy; break;
				case 2:
					rhc_rot_pos = dx8_jstate[rhc_id].lRz; break;
				}
			}
			if (rhc_sld_id != -1) { // If this is a slider
				rhc_rot_pos = dx8_jstate[rhc_id].rglSlider[rhc_sld_id];
			}
			if (rhc_rzx_id != -1 && rhc_rot_id == -1) { // If we use the native Z-axis
				rhc_rot_pos = dx8_jstate[rhc_id].lZ;
			}

			rhc_pos[0] = dx8_jstate[rhc_id].lX;
			rhc_pos[1] = dx8_jstate[rhc_id].lY;
			rhc_pos[2] = 65536 - rhc_rot_pos;

			//Let's cheat and give the ACA a throttle lever
			ttca_throttle_pos = dx8_jstate[rhc_id].rglSlider[0];
			ttca_throttle_pos_dig = (65536.0 - (double)ttca_throttle_pos) / 65536.0;

			// RCS mode toggle
			if (rhc_thctoggle && thc_id == -1 && rhc_thctoggle_id != -1) {
				if (dx8_jstate[rhc_id].rgbButtons[rhc_thctoggle_id]) {
					if (!rhc_thctoggle_pressed) {
						SetAttitudeMode(RCS_LIN);
					}
					rhc_thctoggle_pressed = true;
				}
				else {
					rhc_thctoggle_pressed = false;
				}
			}
		}
		else {

			// No JS

			// Roll
			if (GetManualControlLevel(THGROUP_ATT_BANKLEFT) > 0) {
				rhc_pos[0] = (int)(32768 - GetManualControlLevel(THGROUP_ATT_BANKLEFT) * 32768);
			}
			else if (GetManualControlLevel(THGROUP_ATT_BANKRIGHT) > 0) {
				rhc_pos[0] = (int)(32768 + GetManualControlLevel(THGROUP_ATT_BANKRIGHT) * 32768);
			}
			// Pitch
			if (GetManualControlLevel(THGROUP_ATT_PITCHDOWN) > 0) {
				rhc_pos[1] = (int)(32768 - GetManualControlLevel(THGROUP_ATT_PITCHDOWN) * 32768);
			}
			else if (GetManualControlLevel(THGROUP_ATT_PITCHUP) > 0) {
				rhc_pos[1] = (int)(32768 + GetManualControlLevel(THGROUP_ATT_PITCHUP) * 32768);
			}
			// Yaw
			if (GetManualControlLevel(THGROUP_ATT_YAWLEFT) > 0) {
				rhc_pos[2] = (int)(32768 + GetManualControlLevel(THGROUP_ATT_YAWLEFT) * 32768);
			}
			else if (GetManualControlLevel(THGROUP_ATT_YAWRIGHT) > 0) {
				rhc_pos[2] = (int)(32768 - GetManualControlLevel(THGROUP_ATT_YAWRIGHT) * 32768);
			}
		}

		CDR_ACA.Timestep(rhc_pos);

		if (CDR_ACA.GetOutOfDetent())
		{
			val31[ACAOutOfDetent] = 1;
		}

		if (YawSwitch.IsUp())
		{
			if (CDR_ACA.GetMinusYawBreakout())
			{
				val31[MinusYaw] = 1;
			}
			if (CDR_ACA.GetPlusYawBreakout())
			{
				val31[PlusYaw] = 1;
			}
		}

		if (PitchSwitch.IsUp())
		{
			if (CDR_ACA.GetMinusPitchBreakout())
			{
				val31[MinusElevation] = 1;
			}
			if (CDR_ACA.GetPlusPitchBreakout())
			{
				val31[PlusElevation] = 1;
			}
		}

		if (RollSwitch.IsUp())
		{
			if (CDR_ACA.GetMinusRollBreakout())
			{
				val31[MinusAzimuth] = 1;
			}
			if (CDR_ACA.GetPlusRollBreakout())
			{
				val31[PlusAzimuth] = 1;
			}
		}

		//
		// HARDOVER
		//

		if (LeftACA4JetSwitch.IsUp() && SCS_ATT_DIR_CONT_CB.Voltage() > SP_MIN_DCVOLTAGE)
		{
			if (CDR_ACA.GetMinusRollHardover()) {
				// MINUS ROLL
				SetRCSJet(0, 1);
				SetRCSJet(4, 1);
				SetRCSJet(11, 1);
				SetRCSJet(15, 1);

				SCS_ATT_DIR_CONT_CB.DrawPower(184); // Four thrusters worth
			}
			if (CDR_ACA.GetPlusRollHardover()) {
				// PLUS ROLL
				SetRCSJet(3, 1);
				SetRCSJet(7, 1);
				SetRCSJet(8, 1);
				SetRCSJet(12, 1);

				SCS_ATT_DIR_CONT_CB.DrawPower(184);
			}
			if (CDR_ACA.GetMinusPitchHardover()) {
				// MINUS PITCH
				SetRCSJet(0, 1);
				SetRCSJet(7, 1);
				SetRCSJet(11, 1);
				SetRCSJet(12, 1);

				SCS_ATT_DIR_CONT_CB.DrawPower(184);
			}
			if (CDR_ACA.GetPlusPitchHardover()) {
				// PLUS PITCH
				SetRCSJet(3, 1);
				SetRCSJet(4, 1);
				SetRCSJet(8, 1);
				SetRCSJet(15, 1);

				SCS_ATT_DIR_CONT_CB.DrawPower(184);
			}
			if (CDR_ACA.GetMinusYawHardover()) {
				// MINUS YAW
				SetRCSJet(2, 1);
				SetRCSJet(6, 1);
				SetRCSJet(9, 1);
				SetRCSJet(13, 1);

				SCS_ATT_DIR_CONT_CB.DrawPower(184);
			}
			if (CDR_ACA.GetPlusYawHardover()) {
				// PLUS YAW
				SetRCSJet(1, 1);
				SetRCSJet(5, 1);
				SetRCSJet(10, 1);
				SetRCSJet(14, 1);

				SCS_ATT_DIR_CONT_CB.DrawPower(184);
			}
		}

		//
		// DIRECT
		//

		if (SCS_ATT_DIR_CONT_CB.Voltage() > SP_MIN_DCVOLTAGE)
		{
			if (RollSwitch.IsDown())
			{
				if (CDR_ACA.GetMinusRollBreakout()) {
					// MINUS ROLL
					SetRCSJet(4, 1);
					SetRCSJet(11, 1);

					SCS_ATT_DIR_CONT_CB.DrawPower(92);
				}
				if (CDR_ACA.GetPlusRollBreakout()) {
					// PLUS ROLL
					SetRCSJet(3, 1);
					SetRCSJet(12, 1);

					SCS_ATT_DIR_CONT_CB.DrawPower(92);
				}
			}

			if (PitchSwitch.IsDown())
			{
				if (CDR_ACA.GetMinusPitchBreakout()) {
					// MINUS PITCH
					SetRCSJet(11, 1);
					SetRCSJet(12, 1);

					SCS_ATT_DIR_CONT_CB.DrawPower(92);
				}
				if (CDR_ACA.GetPlusPitchBreakout()) {
					// PLUS PITCH
					SetRCSJet(3, 1);
					SetRCSJet(4, 1);

					SCS_ATT_DIR_CONT_CB.DrawPower(92);
				}
			}

			if (YawSwitch.IsDown())
			{
				if (CDR_ACA.GetMinusYawBreakout()) {
					// MINUS YAW
					SetRCSJet(2, 1);
					SetRCSJet(9, 1);

					SCS_ATT_DIR_CONT_CB.DrawPower(92);
				}
				if (CDR_ACA.GetPlusYawBreakout()) {
					// PLUS YAW
					SetRCSJet(1, 1);
					SetRCSJet(10, 1);

					SCS_ATT_DIR_CONT_CB.DrawPower(92);
				}
			}
		}

		//
		// +X TRANSLATION
		//

		if (PlusXTranslationButton.GetState() == 1 && SCS_ATT_DIR_CONT_CB.IsPowered()) {
			SetRCSJet(7, true);
			SetRCSJet(15, true);
			SetRCSJet(11, true);
			SetRCSJet(3, true);
		}

		if (rhc_debug != -1)
		{
			sprintf(oapiDebugString(), "RHC: X/Y/Z = %d / %d / %d | rzx_id %d rot_id %d", rhc_pos[0], rhc_pos[1], rhc_pos[2], rhc_rzx_id, rhc_rot_id);
		}
		// And now the THC...
		if (thc_id != -1 && thc_id < js_enabled) {
			hr = dx8_joystick[thc_id]->Poll();
			if (FAILED(hr)) { // Did that work?
							  // Attempt to acquire the device
				hr = dx8_joystick[thc_id]->Acquire();
				if (FAILED(hr)) {
					sprintf(oapiDebugString(), "DX8JS: Cannot aquire THC");
				}
				else {
					hr = dx8_joystick[thc_id]->Poll();
				}
			}

			ttca_realistic_throttle = true;

			// Read data
			dx8_joystick[thc_id]->GetDeviceState(sizeof(dx8_jstate[thc_id]), &dx8_jstate[thc_id]);
			// The LM TTCA is even wierder than the CM THC...

			if (LeftTTCATranslSwitch.IsUp()) {
				if (thc_tjt_id != -1) {                    // If Throttle/Jets lever enabled
					thc_tjt_pos = dx8_jstate[thc_id].rglSlider[thc_tjt_id]; // Read
				}

				thc_y_pos = dx8_jstate[thc_id].lY - 32768;
				thc_x_pos = dx8_jstate[thc_id].lX - 32768;

				// Z-axis read.
				if (thc_rot_id != -1) { // If this is a rotator-type axis
					switch (thc_rot_id) {
					case 0:
						thc_rot_pos = dx8_jstate[thc_id].lRx; break;
					case 1:
						thc_rot_pos = dx8_jstate[thc_id].lRy; break;
					case 2:
						thc_rot_pos = dx8_jstate[thc_id].lRz; break;
					}
				}
				if (thc_sld_id != -1) { // If this is a slider
					thc_rot_pos = dx8_jstate[thc_id].rglSlider[thc_sld_id];
				}
				if (thc_rzx_id != -1 && thc_rot_id == -1) { // If we use the native Z-axis
					thc_rot_pos = dx8_jstate[thc_id].lZ;
				}

				thc_z_pos = thc_rot_pos - 32768;

				if (thc_debug != -1) {
					sprintf(oapiDebugString(), "THC: X/Y/Z = %d / %d / %d TJT = %d, Test: %d, thc_rot_id: %d, thc_rzx_id: %d", thc_x_pos, thc_y_pos,
						thc_z_pos, thc_tjt_pos, dx8_jstate[thc_id].rgbButtons[1], thc_rot_id, thc_rzx_id);
				}
			}

			// RCS mode toggle
			if (rhc_thctoggle && rhc_id == -1 && rhc_thctoggle_id != -1) {
				if (dx8_jstate[thc_id].rgbButtons[rhc_thctoggle_id]) {
					if (!rhc_thctoggle_pressed) {
						SetAttitudeMode(RCS_ROT);
					}
					rhc_thctoggle_pressed = true;
				}
				else {
					rhc_thctoggle_pressed = false;
				}
			}
		}
		else {
			// No JS

			// Up/down
			if (GetManualControlLevel(THGROUP_ATT_DOWN) > 0) {
				thc_y_pos = (int)((-GetManualControlLevel(THGROUP_ATT_DOWN)) * 32768.);
			}
			else if (GetManualControlLevel(THGROUP_ATT_UP) > 0) {
				thc_y_pos = (int)(GetManualControlLevel(THGROUP_ATT_UP) * 32768.);
			}
			// Left/right
			if (GetManualControlLevel(THGROUP_ATT_LEFT) > 0) {
				thc_x_pos = (int)((-GetManualControlLevel(THGROUP_ATT_LEFT)) * 32768.);
			}
			else if (GetManualControlLevel(THGROUP_ATT_RIGHT) > 0) {
				thc_x_pos = (int)(GetManualControlLevel(THGROUP_ATT_RIGHT) * 32768.);
			}
			// Forward/Back
			if (GetManualControlLevel(THGROUP_ATT_BACK) > 0) {
				thc_z_pos = (int)((-GetManualControlLevel(THGROUP_ATT_BACK)) * 32768.);
			}
			else if (GetManualControlLevel(THGROUP_ATT_FORWARD) > 0) {
				thc_z_pos = (int)(GetManualControlLevel(THGROUP_ATT_FORWARD) * 32768.);
			}

			//sprintf(oapiDebugString(), "%f %f", ttca_throttle_pos_dig, ttca_thrustcmd);
		}

		ttca_pos[0] = thc_y_pos;
		ttca_pos[1] = thc_x_pos;
		ttca_pos[2] = thc_z_pos;

		CDR_TTCA.Timestep(ttca_pos, thc_tjt_pos, ttca_realistic_throttle, ttca_throttle_pos_dig);

		if (LeftTTCATranslSwitch.IsUp())
		{
			if (CDR_TTCA.GetMinusXTrans()) {
				val31[MinusX] = 1;
			}
			if (CDR_TTCA.GetMinusYTrans()) {
				val31[MinusY] = 1;
			}
			if (CDR_TTCA.GetPlusXTrans()) {
				val31[PlusX] = 1;
			}
			if (CDR_TTCA.GetPlusYTrans()) {
				val31[PlusY] = 1;
			}
			if (CDR_TTCA.GetMinusZTrans()) {
				val31[MinusZ] = 1;
			}
			if (CDR_TTCA.GetPlusZTrans()) {
				val31[PlusZ] = 1;
			}
		}

		if (ttca_throttle_vel == 1)
		{
			ttca_throttle_pos_dig += 0.25*simdt;
		}
		else if (ttca_throttle_vel == -1)
		{
			ttca_throttle_pos_dig -= 0.25*simdt;
		}
		if (ttca_throttle_pos_dig > 1)
		{
			ttca_throttle_pos_dig = 1;
		}
		else if (ttca_throttle_pos_dig < 0)
		{
			ttca_throttle_pos_dig = 0;
		}

		//LM Programer
		if (HasProgramer)
		{
			if (lmp.GetPlusXTrans())
			{
				val31[PlusX] = 1;
			}
		}

		// Write back channel data
		agc.SetInputChannel(031, val31);
	}
}

void LEM::SystemsInternalTimestep(double simdt)
{
	double mintFactor = __max(simdt / 100.0, 0.5);
	double tFactor = __min(mintFactor, simdt);
	while (simdt > 0) {

		// Each timestep is passed to the SPSDK
		// to perform internal computations on the 
		// systems.

		Panelsdk.SimpleTimestep(tFactor);

		agc.SystemTimestep(tFactor);								// Draw power
		dsky.SystemTimestep(tFactor);								// This can draw power now.
		asa.SystemTimestep(tFactor);
		aea.SystemTimestep(tFactor);
		deda.SystemTimestep(tFactor);
		imu.SystemTimestep(tFactor);								// Draw power
		atca.SystemTimestep(tFactor);
		rga.SystemTimestep(tFactor);
		ordeal.SystemTimestep(tFactor);
		fdaiLeft.SystemTimestep(tFactor);							// Draw Power
		fdaiRight.SystemTimestep(tFactor);
		LR.SystemTimestep(tFactor);
		RR.SystemTimestep(tFactor);
		RadarTape.SystemTimeStep(tFactor);
		crossPointerLeft.SystemTimeStep(tFactor);
		crossPointerRight.SystemTimeStep(tFactor);
		SBandSteerable.SystemTimestep(tFactor);
		VHF.SystemTimestep(tFactor);
		SBand.SystemTimestep(tFactor);
		CabinRepressValve.SystemTimestep(tFactor);
		SuitCircuitPressureRegulatorA.SystemTimestep(tFactor);
		SuitCircuitPressureRegulatorB.SystemTimestep(tFactor);
		OVHDCabinReliefDumpValve.SystemTimestep(tFactor);
		FWDCabinReliefDumpValve.SystemTimestep(tFactor);
		SuitCircuitReliefValve.SystemTimestep(tFactor);
		SuitGasDiverter.SystemTimestep(tFactor);
		CabinGasReturnValve.SystemTimestep(tFactor);
		CO2CanisterSelect.SystemTimestep(tFactor);
		PrimCO2CanisterVent.SystemTimestep(tFactor);
		SecCO2CanisterVent.SystemTimestep(tFactor);
		WaterSeparationSelector.SystemTimestep(tFactor);
		WaterTankSelect.SystemTimestep(tFactor);
		CabinFan.SystemTimestep(tFactor);
		PrimGlycolPumpController.SystemTimestep(tFactor);
		SuitFanDPSensor.SystemTimestep(tFactor);
		DPSPropellant.SystemTimestep(tFactor);
		DPS.SystemTimestep(tFactor);
		deca.SystemTimestep(tFactor);
		gasta.SystemTimestep(tFactor);
		scera1.SystemTimestep(tFactor);
		scera2.SystemTimestep(tFactor);
		MissionTimerDisplay.SystemTimestep(tFactor);
		EventTimerDisplay.SystemTimestep(tFactor);

		simdt -= tFactor;
		tFactor = __min(mintFactor, simdt);
	}
}

void LEM::SystemsTimestep(double simt, double simdt) 

{
	// Clear debug line when timer runs out
	if(DebugLineClearTimer > 0){
		DebugLineClearTimer -= simdt;
		if(DebugLineClearTimer < 0){
			sprintf(oapiDebugString(),"");
			DebugLineClearTimer = 0;
		}
	}

	SystemsInternalTimestep(simdt);

	// After that come all other systems simesteps
	agc.Timestep(MissionTime, simdt);						// Do work
	dsky.Timestep(MissionTime);								// Do work
	asa.TimeStep(simdt);									// Do work
	aea.TimeStep(MissionTime, simdt);
	deda.TimeStep(simdt);
	imu.Timestep(simdt);								// Do work
	tcdu.Timestep(simdt);
	scdu.Timestep(simdt);
	// Manage IMU standby heater and temperature
	if(IMU_OPR_CB.Voltage() > 0){
		// IMU is operating.
		if(imublower->h_pump != 1){ imublower->SetPumpAuto(); }
	}else{
		// IMU is not operating.
		if(imublower->h_pump != 0){ imublower->SetPumpOff(); }
	}

	// FIXME: Draw power for lighting system.
	// I can't find the actual power draw anywhere.

	// Allow ATCA to operate between the FDAI and AGC/AEA so that any changes the FDAI makes
	// can be shown on the FDAI, but any changes the AGC/AEA make are visible to the ATCA.
	atca.Timestep(simt, simdt);
	rga.Timestep(simdt);
	ordeal.Timestep(simdt);
	mechanicalAccelerometer.TimeStep(simdt);
	fdaiLeft.Timestep(MissionTime, simdt);
	fdaiRight.Timestep(MissionTime, simdt);
	MissionTimerDisplay.Timestep(MissionTime, simdt, false);
	EventTimerDisplay.Timestep(MissionTime, simdt, false);
	JoystickTimestep(simdt);
	eds.TimeStep(simdt);
	optics.TimeStep(simdt);
	LR.TimeStep(simdt);
	RR.TimeStep(simdt);
	RadarTape.TimeStep(MissionTime);
	crossPointerLeft.TimeStep(simdt);
	crossPointerRight.TimeStep(simdt);
	SBandSteerable.TimeStep(simdt);
	omni_fwd.TimeStep();
	omni_aft.TimeStep();
	SBand.TimeStep(simt);
	ecs.TimeStep(simdt);
	scca1.Timestep(simdt);
	scca2.Timestep(simdt);
	scca3.Timestep(simdt);
	DPSPropellant.Timestep(simt, simdt);
	DPS.TimeStep(simt, simdt);
	APSPropellant.Timestep(simt, simdt);
	APS.TimeStep(simdt);
	RCSA.Timestep(simt, simdt);
	RCSB.Timestep(simt, simdt);
	tca1A.Timestep(simdt);
	tca2A.Timestep(simdt);
	tca3A.Timestep(simdt);
	tca4A.Timestep(simdt);
	tca1B.Timestep(simdt);
	tca2B.Timestep(simdt);
	tca3B.Timestep(simdt);
	tca4B.Timestep(simdt);
	deca.Timestep(simdt);
	gasta.Timestep(simt);
	// Do this toward the end so we can see current system state
	scera1.Timestep();
	scera2.Timestep();
	CWEA.TimeStep(simdt);

	//Treat LM O2 as gas every timestep
	DesO2Tank->BoilAllAndSetTemp(294.261);
	AscO2Tank1->BoilAllAndSetTemp(294.261);
	AscO2Tank2->BoilAllAndSetTemp(294.261);
	DesO2Manifold->BoilAllAndSetTemp(294.261);
	O2Manifold->BoilAllAndSetTemp(294.261);
	PressRegA->BoilAllAndSetTemp(285.928);
	PressRegB->BoilAllAndSetTemp(285.928);

	// Debug tests //

	//ECS Debug Lines//

/*
	double *O2ManifoldPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2MANIFOLD:PRESS");
	double *O2ManifoldMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2MANIFOLD:MASS");
	double *O2ManifoldTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2MANIFOLD:TEMP");
	double *DESO2ManifoldPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2MANIFOLD:PRESS");
	double *DESO2ManifoldMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2MANIFOLD:MASS");
	double *DESO2ManifoldTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2MANIFOLD:TEMP");

	int *deso2outvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2TANK:OUT:ISOPEN");
	int *deso2manifoldinvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2MANIFOLD:IN:ISOPEN");
	int *deso2manifoldoutvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2MANIFOLD:OUT:ISOPEN");
	int *asco2out1vlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:ASCO2TANK1:OUT:ISOPEN");
	int *asco2out2vlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:ASCO2TANK2:OUT:ISOPEN");

	double *DesO2pipeflow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2PIPE1:FLOW");
	double *DesO2pipeflowmax = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2PIPE1:FLOWMAX");
	float *DesO2vlvoutsize = (float*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2TANK:OUT:SIZE");
	float *DesO2Manifoldvlvinsize = (float*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2MANIFOLD:IN:SIZE");

	double *DESO2TankTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2TANK:TEMP");
	double *DESO2TankMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2TANK:MASS");
	double *DESO2VapMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2TANK:O2_VAPORMASS");
	double *DESO2TankEnergy = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2TANK:ENERGY");
	double *DESO2ManifoldEnergy = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2MANIFOLD:ENERGY");
	double *DESO2PP = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2TANK:O2_PPRESS");

	double *asco2tk1press = (double*)Panelsdk.GetPointerByString("HYDRAULIC:ASCO2TANK1:PRESS");
	double *asco2tk2press = (double*)Panelsdk.GetPointerByString("HYDRAULIC:ASCO2TANK2:PRESS");

	int *pressRegAvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGA:OUT:ISOPEN");
	int *pressRegBvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGB:OUT:ISOPEN");
	double *PressRegAPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGA:PRESS");
	double *PressRegBPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGB:PRESS");
	double *PressRegAMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGA:MASS");
	double *PressRegBMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGB:MASS");
	double *PressRegATemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGA:TEMP");
	double *PressRegBTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGB:TEMP");

	double *SuitCircuitPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT:PRESS");
	double *SuitCircuitMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT:MASS");
	double *SuitCircuitTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT:TEMP");

	int *suitReliefvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT:OUT2:ISOPEN");
	double *suitReliefflow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRELIEFVALVE:FLOW");
	double *suitReliefflowmax = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRELIEFVALVE:FLOWMAX");

	double *deso2flow2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2PIPE2:FLOW");
	double *deso2flowmax2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2PIPE2:FLOWMAX");

	double *repressFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINREPRESS:FLOW");
	double *repressFlowmax = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINREPRESS:FLOWMAX");
	double *pressRegAFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGAOUT:FLOW");
	double *pressRegAFlowmax = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGAOUT:FLOWMAX");
	double *pressRegBFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGBOUT:FLOW");

	int *fwdHatchvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:LEAK:ISOPEN");
	int *ovhdHatchvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:OUT2:ISOPEN");
	double *ovhdHatchFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINOVHDHATCHVALVE:FLOW");
	double *ovhdHatchFlowmax = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINOVHDHATCHVALVE:FLOWMAX");
	float *ovhdHatchSize = (float*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:OUT2:SIZE");
	double *fwdHatchFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINFWDHATCHVALVE:FLOW");
	double *fwdHatchFlowmax = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINFWDHATCHVALVE:FLOWMAX");

	double *cabinventpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINVENT:PRESS");

	double *CabinMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:MASS");
	double *CabinPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:PRESS");
	double *CabinTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:TEMP");

	int *suitGasDiverterCabinVLV = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SUITGASDIVERTER:OUT:ISOPEN");
	int *suitGasDiverterEgressVLV = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SUITGASDIVERTER:OUT2:ISOPEN");

	int *cabinGasReturnVLV = (int*)Panelsdk.GetPointerByString("HYDRAULIC:CO2CANISTERMANIFOLD:LEAK:ISOPEN");
	int *primCO2InVLV = (int*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2CANISTER:IN:ISOPEN");
	int *primCO2OutVLV = (int*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2CANISTER:OUT:ISOPEN");
	int *secCO2InVLV = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SECCO2CANISTER:IN:ISOPEN");
	int *secCO2OutVLV = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SECCO2CANISTER:OUT:ISOPEN");
	double *primCO2CanisterPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2CANISTER:PRESS");
	double *secCO2CanisterPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECCO2CANISTER:PRESS");
	double *primCO2CanisterMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2CANISTER:MASS");
	double *secCO2CanisterMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECCO2CANISTER:MASS");
	int *primCO2Vent = (int*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2CANISTER:OUT2:ISOPEN");
	int *secCO2Vent = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SECCO2CANISTER:OUT2:ISOPEN");
	double *primCO2VentFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2VENT:FLOW");
	double *secCO2VentFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECCO2VENT:FLOW");

	double *suitfanmanifoldPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITFANMANIFOLD:PRESS");
	double *hxheatingPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERHEATING:PRESS");
	double *hxheatingTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERHEATING:TEMP");
	double *hxheatingMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERHEATING:MASS");
	double *SGDPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITGASDIVERTER:PRESS");
	double *SGDMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITGASDIVERTER:MASS");
	double *hxcoolingMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERCOOLING:MASS");
	double *hxcoolingPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERCOOLING:PRESS");
	double *hxcoolingTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERCOOLING:TEMP");
	double *WSMMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:WATERSEPMANIFOLD:MASS");
	double *WSMPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:WATERSEPMANIFOLD:PRESS");
	double *WSMTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:WATERSEPMANIFOLD:TEMP");

	double *desO2burstflow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2BURSTDISK:FLOW");
	double *desO2reliefflow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2PRESSURERELIEFVALVE:FLOW");

	double *primglycoltemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLACCUMULATOR:TEMP");
	double *primglycolpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLACCUMULATOR:PRESS");
	double *primglycolmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLACCUMULATOR:MASS");

	int *primevapPump = (int*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOR:PUMP");
	int *primevapValve = (int*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOR:VALVE");
	double *primevapThrottle = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOR:THROTTLE");
	double *primevapSteam = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOR:STEAMPRESSURE");

	double *primCO2Flow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2ABSORBER:FLOW");
	double *primCO2FlowMax = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2ABSORBER:FLOWMAX");
	double *primCO2Removal = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2ABSORBER:CO2REMOVALRATE");
	double *CO2ManifoldPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CO2CANISTERMANIFOLD:PRESS");
	double *CO2ManifoldMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CO2CANISTERMANIFOLD:MASS");
	int *gasreturnvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:CO2CANISTERMANIFOLD:LEAK:ISOPEN");

	double *WS1Flow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:WATERSEP1:FLOW");
	double *WS1FlowMax = (double*)Panelsdk.GetPointerByString("HYDRAULIC:WATERSEP1:FLOWMAX");
	double *WS1H2ORemoval = (double*)Panelsdk.GetPointerByString("HYDRAULIC:WATERSEP1:H2OREMOVALRATE");
	int *WS1vlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERCOOLING:OUT:ISOPEN");
	int *WS2vlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERCOOLING:OUT2:ISOPEN");

	double *primCO2Mass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2CANISTER:MASS");
	double *suitfanmanifoldMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITFANMANIFOLD:MASS");

	double *DesH2OMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESH2OTANK:MASS");
	int *DesH2Ovlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:DESH2OTANK:OUT2:ISOPEN");

	double *WTSMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:H2OTANKSELECT:MASS");
	int *WTSdesinvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:H2OTANKSELECT:IN:ISOPEN");
	int *WTSdesoutvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:H2OTANKSELECT:OUT2:ISOPEN");
	double *STMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:H2OSURGETANK:MASS");
	double *STPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:H2OSURGETANK:PRESS");
	int *surgeprimvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:H2OSURGETANK:OUT:ISOPEN");
	int *surgesecvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:H2OSURGETANK:OUT2:ISOPEN");
	double *SToutflow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:WATERSEPPRIMOUT:FLOW");

	double *primregmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMREG:MASS");
	int *primevap1vlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMREG:OUT:ISOPEN");

	double *primwbmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMWATERBOILER:MASS");
	double *primwbpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMWATERBOILER:PRESS");
	int *primwbinvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMWATERBOILER:IN:ISOPEN");
	int *primwboutvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMWATERBOILER:OUT:ISOPEN");

	double *primloop1mass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLLOOP1:MASS");
	double *primloop1temp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLLOOP1:TEMP");
	double *primloop1press = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLLOOP1:PRESS");
	double *primloop2mass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLLOOP2:MASS");
	double *primloop2temp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLLOOP2:TEMP");
	double *primloop2press = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLLOOP2:PRESS");

	double *primevapinmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPINLET:MASS");
	double *primevaptempin = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPINLET:TEMP");
	double *primevapinpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPINLET:PRESS");
	double *primevapoutmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPOUTLET:MASS");
	double *primevaptempout = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPOUTLET:TEMP");
	double *primevapoutpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPOUTLET:PRESS");

	double *Pump1Flow = (double*)Panelsdk.GetPointerByString("ELECTRIC:PRIMGLYCOLPUMP1:FLOW");
	double *Pump2Flow = (double*)Panelsdk.GetPointerByString("ELECTRIC:PRIMGLYCOLPUMP2:FLOW");
	double *Pump1OutFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYPUMPMANIFOLDOUT1:FLOW");
	double *Pump2OutFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYPUMPMANIFOLDOUT2:FLOW");
	double *primGlyReg1Flow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYFLOWREG1:FLOW");
	double *waterGlyHXFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:WATERGLYCOLHXOUT:FLOW");
	double *suitHXGlyFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:HXFLOWCONTROL:FLOW");
	double *suitHXGlyFlowMax = (double*)Panelsdk.GetPointerByString("HYDRAULIC:HXFLOWCONTROL:FLOWMAX");
	double *suitHXGlyBypassFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:HXFLOWCONTROLBYPASS:FLOW");

	double *secglycolmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLACCUMULATOR:MASS");
	double *secglycolpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLACCUMULATOR:PRESS");
	double *secglycoltemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLACCUMULATOR:TEMP");
	double *secpumpmanifoldmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLPUMPFANMANIFOLD:MASS");
	double *secpumpmanifoldpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLPUMPFANMANIFOLD:PRESS");
	double *secpumpmanifoldtemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLPUMPFANMANIFOLD:TEMP");
	double *secloop1mass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLLOOP1:MASS");
	double *secloop1press = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLLOOP1:PRESS");
	double *secloop1temp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLLOOP1:TEMP");
	double *secloop2mass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLLOOP2:MASS");
	double *secloop2press = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLLOOP2:PRESS");
	double *secloop2temp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLLOOP2:TEMP");
	double *secevapinmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPINLET:MASS");
	double *secevapinpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPINLET:PRESS");
	double *secevaptempin = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPINLET:TEMP");
	double *secevapoutmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPOUTLET:MASS");
	double *secevapoutpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPOUTLET:PRESS");
	double *secevaptempout = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPOUTLET:TEMP");
	double *secascbatmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECASCBATCOOLING:MASS");
	double *secascbatpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECASCBATCOOLING:PRESS");
	double *secascbattemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECASCBATCOOLING:TEMP");
	double *secGlyReg1Flow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYFLOWREG1:FLOW");
	double *secGlyReg2Flow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYFLOWREG2:FLOW");

	int *secevapPump = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPORATOR:PUMP");
	int *secevapValve = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPORATOR:VALVE");
	double *secevapThrottle = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPORATOR:THROTTLE");
	double *secevapSteam = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPORATOR:STEAMPRESSURE");

	int *slevapPump = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SUITLOOPEVAPORATOR:PUMP");
	int *slevapValve = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SUITLOOPEVAPORATOR:VALVE");
	double *slevapThrottle = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITLOOPEVAPORATOR:THROTTLE");
	double *slevapSteam = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITLOOPEVAPORATOR:STEAMPRESSURE");

	double *secreg2mass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECREG2:MASS");
	int *secevapvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SECREG1MANIFOLD:OUT2:ISOPEN");
	double *secevapmassout = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPOUTLET:MASS");

	double *lcgaccumass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCGACCUMULATOR:MASS");
	double *lcgaccupress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCGACCUMULATOR:PRESS");
	double *lcgaccutemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCGACCUMULATOR:TEMP");
	double *lcghxmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCGHX:MASS");
	double *lcghxpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCGHX:PRESS");
	double *lcghxtemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCGHX:TEMP");
	double *lcgmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCG:MASS");
	double *lcgpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCG:PRESS");
	double *lcgtemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCG:TEMP"); 

	double *lcghxflow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCGFLOWCONTROL:FLOW");
	double *lcghxflowmax = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCGFLOWCONTROL:FLOWMAX");
	double *lcgaccflow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCGFLOWCONTROLBYPASS:FLOW");
	double *lcgaccflowmax = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCGFLOWCONTROLBYPASS:FLOWMAX");

	double *glycolsuitcooltemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLSUITHXCOOLING:TEMP");
	double *glycolsuitheattemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLSUITHXHEATING:TEMP");
	double *glycolpumpmanifoldtemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLPUMPMANIFOLD:TEMP");
	double *waterglycolhxtemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:WATERGLYCOLHX:TEMP");
	double *ascbatglycoltemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:ASCBATCOOLING:TEMP");
	double *desbatglycoltemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESBATCOOLING:TEMP");

	double *glycolsuitcoolmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLSUITHXCOOLING:MASS");
	double *glycolsuitcoolpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLSUITHXCOOLING:PRESS");
	double *glycolsuitheatmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLSUITHXHEATING:MASS");
	double *glycolsuitheatpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLSUITHXHEATING:PRESS");
	double *glycolpumpmanifoldmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLPUMPMANIFOLD:MASS");
	double *glycolpumpmanifoldpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLPUMPMANIFOLD:PRESS");

	double *waterglycolhxmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:WATERGLYCOLHX:MASS");
	double *waterglycolhxpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:WATERGLYCOLHX:PRESS");
	double *ascbatglycolmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:ASCBATCOOLING:MASS");
	double *ascbatglycolpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:ASCBATCOOLING:PRESS");
	double *desbatglycolmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESBATCOOLING:MASS");
	double *desbatglycolpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESBATCOOLING:PRESS");

	double *cdrsuitmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CDRSUIT:MASS");
	double *cdrsuitpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CDRSUIT:PRESS");
	double *cdrsuittemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CDRSUIT:TEMP");
	double *cdrsuitenergy = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CDRSUIT:ENERGY");
	double *lmpsuitmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMPSUIT:MASS");
	double *lmpsuitpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMPSUIT:PRESS");
	double *lmpsuittemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMPSUIT:TEMP");
	double *lmpsuitenergy = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMPSUIT:ENERGY");

	double *SuitHXHCO2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERHEATING:CO2_PPRESS");
	double *CDRSuitCO2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CDRSUIT:CO2_PPRESS");
	double *LMPSuitCO2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMPSUIT:CO2_PPRESS");
	double *SuitGasDiverterCO2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITGASDIVERTER:CO2_PPRESS");
	double *SuitCircuitCO2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT:CO2_PPRESS");
	double *CabinCO2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:CO2_PPRESS");
	double *CanisterMFCO2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CO2CANISTERMANIFOLD:CO2_PPRESS");
	double *PrimCO2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2CANISTER:CO2_PPRESS");
	double *SecCO2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECCO2CANISTER:CO2_PPRESS");
	double *SuitFanCO2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITFANMANIFOLD:CO2_PPRESS");
	double *SuitHXCCO2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERCOOLING:CO2_PPRESS");

	double *CabFan = (double*)Panelsdk.GetPointerByString("ELECTRIC:CABINFAN:ISON");


	//Prim Loop 1 Heat
	double *LGCHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LGCHEAT:HEAT");
	double *CDUHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CDUHEAT:HEAT");
	double *PSAHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PSAHEAT:HEAT");
	double *TLEHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:TLEHEAT:HEAT");
	double *GASTAHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:GASTAHEAT:HEAT");
	double *LCAHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCAHEAT:HEAT");
	double *DSEHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DSEHEAT:HEAT");
	double *ASAHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:ASAHEAT:HEAT");
	double *PTAHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PTAHEAT:HEAT");
	double *IMUHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:IMUHEAT:HEAT");
	double *RGAHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:RGAHEAT:HEAT");
	
	//Prim Loop 2 Heat
	double *SBPHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SBPHEAT:HEAT");
	double *AEAHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:AEAHEAT:HEAT");
	double *ATCAHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:ATCAHEAT:HEAT");
	double *SCERAHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SCERAHEAT:HEAT");
	double *CWEAHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CWEAHEAT:HEAT");
	double *RREHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:RREHEAT:HEAT");
	double *SBXHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SBXHEAT:HEAT");
	double *VHFHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:VHFHEAT:HEAT");
	double *INVHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:INVHEAT:HEAT");
	double *ECAHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:ECAHEAT:HEAT");
	double *PCMHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PCMHEAT:HEAT");

	//Sec Loop 2 Heat
	double *SBPHeatSec = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECSBPHEAT:HEAT");
	double *AEAHeatSec = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECAEAHEAT:HEAT");
	double *ATCAHeatSec = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECATCAHEAT:HEAT");
	double *SCERAHeatSec = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECSCERAHEAT:HEAT");
	double *CWEAHeatSec = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECCWEAHEAT:HEAT");
	double *RREHeatSec = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECRREHEAT:HEAT");
	double *SBXHeatSec = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECSBXHEAT:HEAT");
	double *VHFHeatSec = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECVHFHEAT:HEAT");
	double *INVHeatSec = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECINVHEAT:HEAT");
	double *ECAHeatSec = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECECAHEAT:HEAT");
	double *PCMHeatSec = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECPCMHEAT:HEAT");

	double *TLEHeatSec = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECTLEHEAT:HEAT");
	double *ASAHeatSec = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECASAHEAT:HEAT");
	double *RGAHeatSec = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECRGAHEAT:HEAT");

	double *ASATemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LEM-ASA-HSink:TEMP");
	double *SBDTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LEM-SBand-Steerable-Antenna:TEMP");
	double *RRTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LEM-RR-Antenna:TEMP");
	double *LRTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LEM-LR-Antenna:TEMP");
	double *IMUTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LM-IMU-Case:TEMP");
	double *SBDHtr = (double*)Panelsdk.GetPointerByString("ELECTRIC:LEM-SBand-Steerable-Antenna-Heater:ISON");
	double *RRStbyHtr = (double*)Panelsdk.GetPointerByString("ELECTRIC:LEM-RR-Antenna-StbyHeater:ISON");
	double *RRHtr = (double*)Panelsdk.GetPointerByString("ELECTRIC:LEM-RR-Antenna-Heater:ISON");
	double *LRHtr = (double*)Panelsdk.GetPointerByString("ELECTRIC:LEM-LR-Antenna-Heater:ISON");

	double *QD1Temp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMRCSQUAD1:TEMP");
	double *QD2Temp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMRCSQUAD2:TEMP");
	double *QD3Temp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMRCSQUAD3:TEMP");
	double *QD4Temp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMRCSQUAD4:TEMP");

	//CSM LM Connection
	double *lmcabinpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:PRESS");
	double *lmtunnelpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMTUNNEL:PRESS");
	double *lmtunneltemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMTUNNEL:TEMP");
	double *lmtunnelflow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMTUNNELUNDOCKED:FLOW");
*/

	//sprintf(oapiDebugString(), "Quad 1 %lf Quad 2 %lf Quad 3 %lf Quad 4 %lf", KelvinToFahrenheit(*QD1Temp), KelvinToFahrenheit(*QD2Temp), KelvinToFahrenheit(*QD3Temp), KelvinToFahrenheit(*QD4Temp));
	//sprintf(oapiDebugString(), "PrimGlycolQty %lf SecGlycolQty %lf", ecs.GetPrimaryGlycolQuantity(), ecs.GetSecondaryGlycolQuantity());

	//sprintf(oapiDebugString(), "GlyTmp %lf GlyCoolTmp %lf HXCTmp %lf GlyHeatTmp %lf HXHTmp %lf StTmp %lf CP %lf CT %lf LP %lf LT %lf", (*primglycoltemp)* 1.8 - 459.67, (*glycolsuitcooltemp)* 1.8 - 459.67, (*hxcoolingTemp)* 1.8 - 459.67, (*glycolsuitheattemp)* 1.8 - 459.67, (*hxheatingTemp)* 1.8 - 459.67, (*SuitCircuitTemp)* 1.8 - 459.67, (*cdrsuitpress)*PSI, (*cdrsuittemp)* 1.8 - 459.67, (*lmpsuitpress)*PSI, (*lmpsuittemp)* 1.8 - 459.67);
	//sprintf(oapiDebugString(), "CO2CP %lf SFMP %lf CO2F %lf CO2REM %lf WS1F %lf H2OREM %lf SC Mass: %lf", *primCO2CanisterPress*PSI, *suitfanmanifoldPress*PSI, *primCO2Flow, *primCO2Removal, *WS1Flow, *WS1H2ORemoval, (*hxheatingMass + *cdrsuitmass + *lmpsuitmass + *SuitCircuitMass + *SGDMass + *CO2ManifoldMass + *primCO2CanisterMass + *secCO2CanisterMass + *suitfanmanifoldMass + *hxcoolingMass));
	//sprintf(oapiDebugString(), "Total: %lf HXH %lf CDRS %lf LMPS %lf SC %lf SGD %lf CO2M %lf PCO2 %lf SFM %lf HXC %lf RV %d RVF %lf", (*hxheatingMass + *cdrsuitmass + *lmpsuitmass + *SuitCircuitMass + *SGDMass + *CO2ManifoldMass + *primCO2CanisterMass + *secCO2CanisterMass + *suitfanmanifoldMass + *hxcoolingMass), *hxheatingMass, *cdrsuitmass, *lmpsuitmass, *SuitCircuitMass, *SGDMass, *CO2ManifoldMass, *primCO2CanisterMass, *suitfanmanifoldMass, *hxcoolingMass, *suitReliefvlv, *suitReliefflow*LBH);
	//sprintf(oapiDebugString(), "HXH %lf CS %lf LS %lf SC %lf SGD %lf CO2M %lf PCO2 %lf SFM %lf HXC %lf WSM %lf CO2F %lf CO2REM %lf WS1F %lf H2OREM %lf", *hxheatingPress*PSI, *cdrsuitpress*PSI, *lmpsuitpress*PSI, *SuitCircuitPress*PSI, *SGDPress*PSI, *CO2ManifoldPress*PSI, *primCO2CanisterPress*PSI, *suitfanmanifoldPress*PSI, *hxcoolingPress*PSI, *WSMPress*PSI, *primCO2Flow, *primCO2Removal, *WS1Flow, *WS1H2ORemoval);
	//sprintf(oapiDebugString(), "CAB %lf SUIT %lf OVHDFLOW %lf OVHDFLOWMAX %lf OVHDSIZE %f TUNNELPRESS %lf TUNNELFLOW %lf", ecs.GetCabinPressurePSI(), (*SuitCircuitPress)*PSI, *ovhdHatchFlow*LBH, *ovhdHatchFlowmax*LBH, *ovhdHatchSize, *lmtunnelpress*PSI, *lmtunnelflow*LBH);
	
	//sprintf(oapiDebugString(), "LM Cabin: %lf LM Tunnel: %lf", *lmcabinpress*PSI, *lmtunnelpress*PSI);
	//sprintf(oapiDebugString(), "HXH %lf CDRS %lf LMPS %lf SC %lf SGD %lf CO2M %lf PCO2 %lf SFM %lf HXC %lf CO2F %lf CO2REM %lf GRV %d", *hxheatingPress*PSI, *cdrsuitpress*PSI, *lmpsuitpress*PSI, *SuitCircuitPress*PSI, *SGDPress*PSI, *CO2ManifoldPress*PSI, *primCO2CanisterPress*PSI, *suitfanmanifoldPress*PSI, *hxcoolingPress*PSI, *primCO2Flow, *primCO2Removal, *gasreturnvlv);
	//sprintf(oapiDebugString(), "CAB %lf RVF %lf RVFM %lf HXH %lf CDRS %lf LMPS %lf SC %lf SGD %lf CO2M %lf PCO2 %lf SFM %lf HXC %lf", *CabinMass, *suitReliefflow, *suitReliefflowmax, *hxheatingMass, *cdrsuitmass, *lmpsuitmass, *SuitCircuitMass, *SGDMass, *CO2ManifoldMass, *primCO2Mass, *suitfanmanifoldMass, *hxcoolingMass);
	
	//sprintf(oapiDebugString(), "CAB %lf RVF %lf HXH %lf CDRS %lf LMPS %lf SC %lf SGD %lf CO2M %lf PCO2 %lf SFM %lf HXC %lf", *CabinMass, *suitReliefflow, *hxheatingPress*PSI, *cdrsuitpress*PSI, *lmpsuitpress*PSI, *SuitCircuitPress*PSI, *SGDPress*PSI, *CO2ManifoldPress*PSI, *primCO2CanisterPress*PSI, *suitfanmanifoldPress*PSI, *hxcoolingPress*PSI);
	
	//sprintf(oapiDebugString(), "SBD: T %lf H %lf RR: T %lf SH %lf H %lf LR: T %lf H %lf", KelvinToFahrenheit(*SBDTemp), *SBDHtr, KelvinToFahrenheit(*RRTemp), *RRStbyHtr, *RRHtr, KelvinToFahrenheit(*LRTemp), *LRHtr);

	//sprintf(oapiDebugString(), "ASA %lf GL1 %lf Prim Loop 1 Heat: %lf Prim Loop 2 Heat: %lf", KelvinToFahrenheit(*ASATemp), KelvinToFahrenheit(*primglycoltemp), (*LGCHeat + *CDUHeat + *PSAHeat + *TLEHeat + *GASTAHeat + *LCAHeat + *DSEHeat + *ASAHeat + *PTAHeat + *IMUHeat + *RGAHeat), (*SBPHeat + *AEAHeat + *ATCAHeat + *SCERAHeat + *CWEAHeat + *RREHeat + *SBXHeat + *VHFHeat + *INVHeat + *ECAHeat + *PCMHeat));
	//sprintf(oapiDebugString(), "ASA %lf PL1 %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", *ASATemp, *primglycoltemp, *SBPHeat, *AEAHeat, *ATCAHeat, *SCERAHeat, *CWEAHeat, *RREHeat, *SBXHeat, *VHFHeat, *INVHeat, *ECAHeat, *PCMHeat);
	//sprintf(oapiDebugString(), "Sec Loop 1 Heat: %lf, Sec Loop 2 Heat: %lf", (*SBPHeatSec + *AEAHeatSec + *ATCAHeatSec + *SCERAHeatSec + *CWEAHeatSec + *RREHeatSec + *SBXHeatSec + *VHFHeatSec + *INVHeatSec + *ECAHeatSec + *PCMHeatSec), (*TLEHeatSec + *ASAHeatSec + *RGAHeatSec));

	//sprintf(oapiDebugString(), "Sen %lf RM %lf HXH %lf CDR %lf LMP %lf SGD %lf SC %lf CAB %lf CAN %lf PRIM %lf SEC %lf SF %lf HXC %lf", ecs.GetSensorCO2MMHg(), *primCO2Removal, *SuitHXHCO2*MMHG, *CDRSuitCO2*MMHG, *LMPSuitCO2*MMHG, *SuitGasDiverterCO2*MMHG, *SuitCircuitCO2*MMHG, *CabinCO2*MMHG, *CanisterMFCO2*MMHG, *PrimCO2*MMHG, *SecCO2*MMHG, *SuitFanCO2*MMHG, *SuitHXCCO2*MMHG);
	//sprintf(oapiDebugString(), "GRV %d CO2MP %lf PCO2P %1f SFMP %lf SHXCP %lf SHXHP %lf CO2F %lf CO2REM %lf WS1F %lf H2OTM %lf H2OTOF %lf", *gasreturnvlv, (*CO2ManifoldPress)*PSI, (*primCO2CanisterPress)*PSI, (*suitfanmanifoldPress)*PSI, (*hxcoolingPress)*PSI, (*hxheatingPress)*PSI, *primCO2Flow, *primCO2Removal, *WS1Flow, (*STMass)*LBS, (*STPress)*PSI, *SToutflow);

	//sprintf(oapiDebugString(), "AcM %lf FMM %lf L1M %lf ABCM %lf L2M %lf EIM %lf EOM %lf Flow1 %lf Flow2 %lf", *secglycolmass, *secpumpmanifoldmass, *secloop1mass, *secascbatmass, *secloop2mass, *secevapinmass, *secevapoutmass, *secGlyReg1Flow*LBH, *secGlyReg2Flow*LBH);
	//sprintf(oapiDebugString(), "AcP %lf FMP %lf L1P %lf ABCP %lf L2P %lf EIP %lf EOP %lf Flow1 %lf Flow2 %lf", *secglycolpress*PSI, *secpumpmanifoldpress*PSI, *secloop1press*PSI, *secascbatpress*PSI, *secloop2press*PSI, *secevapinpress*PSI, *secevapoutpress*PSI, *secGlyReg1Flow*LBH, *secGlyReg2Flow*LBH);
	//sprintf(oapiDebugString(), "AcT %lf FMT %lf L1T %lf ABCT %lf L2T %lf ETI %lf ETO %lf SCT %lf SETh %lf SCTh %lf", *secglycoltemp* 1.8 - 459.67, *secpumpmanifoldtemp* 1.8 - 459.67, *secloop1temp* 1.8 - 459.67, *secascbattemp* 1.8 - 459.67, *secloop2temp* 1.8 - 459.67, *secevaptempin* 1.8 - 459.67, *secevaptempout* 1.8 - 459.67, *hxcoolingTemp* 1.8 - 459.67, *secevapThrottle, *slevapThrottle);
	
	//sprintf(oapiDebugString(), "AP %lf PMP %lf HXCP %lf L1P %lf HXLP %lf L2P %lf HXHP %lf EIP %lf EOP %lf ACP %lf DBP %lf Flow1 %lf HXFlow %lf", *primglycolpress*PSI, *glycolpumpmanifoldpress*PSI, *glycolsuitcoolpress*PSI, *primloop1press*PSI, *waterglycolhxpress*PSI, *primloop2press*PSI, *glycolsuitheatpress*PSI, *primevapinpress*PSI, *primevapoutpress*PSI, *ascbatglycolpress*PSI, *desbatglycolpress*PSI, *Pump1Flow*LBH, *suitHXGlyFlow*LBH);
	//sprintf(oapiDebugString(), "AM %lf HXCM %lf L1M %lf HXLM %lf L2M %lf HXHM %lf EIM %lf EOM %lf ACM %lf DBM %lf", *primglycolmass, *glycolsuitcoolmass, *primloop1mass, *waterglycolhxmass, *primloop2mass, *glycolsuitheatmass, *primevapinmass, *primevapoutmass, *ascbatglycolmass, *desbatglycolmass);
	//sprintf(oapiDebugString(), "P1 %lf P2 %lf Reg1 %lf WGHX %lf SHX %lf SHXBP %lf", *Pump1OutFlow*LBH, *Pump2OutFlow*LBH, *primGlyReg1Flow*LBH, *waterGlyHXFlow*LBH, *suitHXGlyFlow*LBH, *suitHXGlyBypassFlow*LBH);
	//sprintf(oapiDebugString(), "AcT %lf PMT %lf GCT %lf SCT %lf L1T %lf HXT %lf L2T %lf GHT %lf SHT %lf ETI %lf ETO %lf ABC %lf DBC %lf IMUT %lf", *primglycoltemp* 1.8 - 459.67, *glycolpumpmanifoldtemp* 1.8 - 459.67, *glycolsuitcooltemp* 1.8 - 459.67, *hxcoolingTemp* 1.8 - 459.67, *primloop1temp* 1.8 - 459.67, *waterglycolhxtemp* 1.8 - 459.67, *primloop2temp* 1.8 - 459.67, *glycolsuitheattemp* 1.8 - 459.67, *hxheatingTemp* 1.8 - 459.67, *primevaptempin* 1.8 - 459.67, *primevaptempout* 1.8 - 459.67, *ascbatglycoltemp* 1.8 - 459.67, *desbatglycoltemp* 1.8 - 459.67, *IMUTemp* 1.8 - 459.67);
	
	//sprintf(oapiDebugString(), "LCG %lf SEC %lf", LCGPump->Voltage(), SecGlyPump->Voltage());
	//sprintf(oapiDebugString(), "CM %lf CP %lf CT %lf CE %lf LM %lf LP %lf LT %lf LE %lf", *cdrsuitmass, (*cdrsuitpress)*PSI, (*cdrsuittemp)* 1.8 - 459.67, *cdrsuitenergy, *lmpsuitmass, (*lmpsuitpress)*PSI, (*lmpsuittemp)* 1.8 - 459.67, *lmpsuitenergy);
	//sprintf(oapiDebugString(), "PRAQ %lf PRAP %lf PRAT %lf PRBQ %lf PRBP %lf PRBT %lf", *PressRegAMass, (*PressRegAPress)*PSI, (*PressRegATemp)* 1.8 - 459.67, *PressRegBMass, (*PressRegBPress)*PSI, (*PressRegBTemp)* 1.8 - 459.67);
	//sprintf(oapiDebugString(), "GlyTmp %lf GlySuitCoolTmp %lf HXCTmp %lf HXHTmp %lf StTmp %lf CP %lf CT %lf CE %lf LP %lf LT %lf LE %lf", (*primglycoltemp)* 1.8 - 459.67, (*glycolsuitcooltemp)* 1.8 - 459.67, (*hxcoolingTemp)* 1.8 - 459.67, (*hxheatingTemp)* 1.8 - 459.67, (*SuitCircuitTemp)* 1.8 - 459.67, (*cdrsuitpress)*PSI, (*cdrsuittemp)* 1.8 - 459.67, *cdrsuitenergy, (*lmpsuitpress)*PSI, (*lmpsuittemp)* 1.8 - 459.67, *lmpsuitenergy);
	//sprintf(oapiDebugString(), "LCGHXT %lf LCGT %lf CDRT %lf LMPT %lf WGHXT %lf HXFlow %lf BPFlow %lf HXFM %lf BPFM %lf", *lcghxtemp* 1.8 - 459.67, *lcgtemp* 1.8 - 459.67, *cdrsuittemp* 1.8 - 459.67, *lmpsuittemp* 1.8 - 459.67, *waterglycolhxtemp* 1.8 - 459.67, *lcghxflow, *lcgaccflow, *lcghxflowmax, *lcgaccflowmax);
	//sprintf(oapiDebugString(), "LCGAM %lf LCGAP %lf LCGAT %lf LCGHXM %lf LCGHXP %lf LCGHXT %lf LCGM %lf LCGP %lf LCGT %lf WGHXT %lf HXF %lf BPF %lf", *lcgaccumass, *lcgaccupress*PSI, *lcgaccutemp* 1.8 - 459.67, *lcghxmass, *lcghxpress*PSI, *lcghxtemp* 1.8 - 459.67, *lcgmass, *lcgpress*PSI, *lcgtemp* 1.8 - 459.67, *waterglycolhxtemp* 1.8 - 459.67, *lcghxflow, *lcgaccflow);
	//sprintf(oapiDebugString(), "Press %lf Loop1Mass %lf Evap Mass %lf Loop1Temp %lf Loop2Temp %lf EvapInTemp %lf EvapOutTemp %lf, EPump %d EValve %d EThrot %lf ESteam %lf", ecs.GetSecondaryGlycolPressure(), *secloop1mass, *secevapmassout, *secloop1temp, *secloop2temp, *secevaptempin, *secevaptempout, *secevapPump, *secevapValve, *secevapThrottle, (*secevapSteam)*PSI);
	//sprintf(oapiDebugString(), "WB Press %lf Loop1Temp %lf Loop2Temp %lf EvapInTemp %lf EvapOutTemp %lf, EPump %d EValve %d EThrot %lf ESteam %lf", (*primwbpress)*PSI, *primloop1temp, *primloop2temp, *primevaptempin, *primevaptempout, *primevapPump, *primevapValve, *primevapThrottle, (*primevapSteam)*PSI);
	//sprintf(oapiDebugString(), "DH2O:M %lf WTS:M %lf ST:M %lf PR:M %lf vlv %d WB:M %lf Loop1 %lf EVAPin %lf EVAPout %lf CaseGly %lf EThrot %lf ESteam %lf", *DesH2OMass, *WTSMass, *STMass, *primregmass, *primevap1vlv, *primwbmass, *primevaptempin, *primevaptempout, *primloop1temp, *primevapThrottle, *primevapSteam*PSI);
	//sprintf(oapiDebugString(), "PCO2F %lf SFMP %lf HXCP %lf HXHP %lf WSF %lf WSFM %lf", *primCO2Flow, *suitfanmanifoldPress*PSI, *hxcoolingPress*PSI, *hxheatingPress*PSI, *WS1Flow, *WS1FlowMax);
	//sprintf(oapiDebugString(), "GTemp %lf GPress %lf GMass %lf EPump %d EValve %lf EThrot %lf ESteam %lf", ecs.GetPrimaryGlycolTemperature(), ecs.GetPrimaryGlycolPressure(), *primglycolmass, *primevapPump, *primevapValve, *primevapThrottle, *primevapSteam*PSI);
	
	//sprintf(oapiDebugString(), "SCT %lf SCM %1f SCP %lf HXHM %1f HXHP %lf SGDM %1f SGDP %lf HXCM %1f HXCP %lf", ecs.GetSuitTemperature(), *SuitCircuitMass, ecs.GetSuitPressurePSI(), *hxheatingMass, (*hxheatingPress)*PSI, *SGDMass, (*SGDPress)*PSI, *hxcoolingMass, (*hxcoolingPress)*PSI);
	//sprintf(oapiDebugString(), "BDF %lf RVF %lf CabinP %lf CabinT %lf SuitP %lf SuitT %lf", *desO2burstflow, *desO2reliefflow, ecs.GetCabinPressurePSI(), ecs.GetCabinTemperature(), ecs.GetSuitPressurePSI(), ecs.GetSuitTemperature());
	//sprintf(oapiDebugString(), "PRAQ %lf PRAP %lf PRAT %lf PRBQ %lf PRBP %lf PRBT %lf", *PressRegAMass, (*PressRegAPress)*PSI, *PressRegATemp, *PressRegBMass, (*PressRegBPress)*PSI, *PressRegBTemp);
	//sprintf(oapiDebugString(), "DO2Q %lf DO2P %lf DO2TT %lf DO2MQ %lf DO2MP %lf DO2MT %lf O2MQ %lf O2MP %lf O2MT %lf", ecs.DescentOxyTankQuantity(), ecs.DescentOxyTankPressurePSI(), *DESO2TankTemp, *DESO2ManifoldMass, (*DESO2ManifoldPress)*PSI, *O2ManifoldTemp, *O2ManifoldMass, (*O2ManifoldPress)*PSI, *O2ManifoldTemp);

	//sprintf(oapiDebugString(), "CO2 MP %lf PRIM CO2 %lf SEC CO2 %lf CAB %lf SUIT %lf PV %d PF %lf SV %d SF %lf", (*CO2ManifoldPress)*PSI, (*primCO2CanisterPress)*PSI, (*secCO2CanisterPress)*PSI, (*cabinPress)*PSI, (*suitPress)*PSI, *primCO2Vent, *primCO2Flow, *secCO2Vent, *secCO2Flow);
	//sprintf(oapiDebugString(), "CAB %lf SUIT %lf VLV %d SRFLOW %lf SRFLOWMAX %lf FWDFLOW %lf FWDFLOWMAX %lf OVHDFLOW %lf OVHDFLOWMAX %lf TUNNELPRESS %lf TUNNELFLOW %lf", ecs.GetCabinPressurePSI(), (*SuitCircuitPress)*PSI, *suitReliefvlv, *suitReliefflow*LBH, *suitReliefflowmax*LBH, *fwdHatchFlow*LBH, *fwdHatchFlowmax*LBH, *ovhdHatchFlow*LBH, *ovhdHatchFlowmax*LBH, *lmtunnelpress*PSI, *lmtunnelflow*LBH);
	//sprintf(oapiDebugString(), "CabinP %lf CabinT %lf SuitP %lf SuitT %lf", ecs.GetCabinPressurePSI(), ecs.GetCabinTemperature(), ecs.GetSuitPressurePSI(), ecs.GetSuitTemperature());
	
	//sprintf(oapiDebugString(), "DO2Q %lf DO2P %lf DO2T %lf DO2VM %lf DO2E %lf DO2PP %lf", ecs.DescentOxyTankQuantity(), ecs.DescentOxyTankPressurePSI(), *DESO2TankTemp, *DESO2VapMass, *DESO2Energy, (*DESO2PP*PSI));
	//sprintf(oapiDebugString(), "DO2TP %lf DO2MP %lf O2MP %lf PREGA %lf SUITP %lf", ecs.DescentOxyTankPressurePSI(), (*DESO2ManifoldPress*PSI), (*O2ManifoldPress*PSI), (*PressRegAPress*PSI), ecs.GetSuitPressurePSI());
	//sprintf(oapiDebugString(), "DO2TP %lf DO2TM %lf DO2MP %lf DO2MM %lf O2MP %lf O2MM %lf DESO2 %d ASCO21 %d ASCO22 %d", ecs.DescentOxyTankPressurePSI(), ecs.DescentOxyTankQuantity(), (*DESO2ManifoldPress*PSI), *DESO2ManifoldMass, (*O2ManifoldPress*PSI), *O2ManifoldMass, *deso2manifoldoutvlv, *asco2out1vlv, *asco2out2vlv);


	/*
	double CDRAmps=0,LMPAmps=0;
	double CDRVolts = CDRs28VBus.Voltage(),LMPVolts = LMPs28VBus.Voltage();
	if(LMPVolts > 0){ LMPAmps = LMPs28VBus.PowerLoad()/LMPVolts; }
	if(CDRVolts > 0){ CDRAmps = CDRs28VBus.PowerLoad()/CDRVolts; }	
	sprintf(oapiDebugString(),"LM: LMP %f V/%f A CDR %f V/%f A | AC-A %f V AC-B %f V",LMPVolts,LMPAmps,
		CDRVolts,CDRAmps,ACBusA.Voltage(), ACBusB.Voltage());
	*/	
}

void LEM::SetPipeMaxFlow(char *pipe, double flow) {

	h_Pipe *p = (h_Pipe *)Panelsdk.GetPointerByString(pipe);
	p->flowMax = flow;
}

h_Pipe* LEM::GetLMTunnelPipe()
{
	return (h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:LMTUNNELUNDOCKED");
}

void LEM::ConnectTunnelToCabinVent()
{
	h_Pipe *pipe = (h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:LMTUNNELUNDOCKED");
	h_Tank *tank = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:LMTUNNEL");

	pipe->in = &tank->OUT_valve;
}

double LEM::GetRCSQuadTempF(int index)
{
	if (index >= LM_RCS_QUAD_1 && index <= LM_RCS_QUAD_4)
	{
		h_Radiator *quad = 0;
		switch (index)
		{
		case LM_RCS_QUAD_1:
			quad = LMQuad1RCS;
			break;

		case LM_RCS_QUAD_2:
			quad = LMQuad2RCS;
			break;

		case LM_RCS_QUAD_3:
			quad = LMQuad3RCS;
			break;

		case LM_RCS_QUAD_4:
			quad = LMQuad4RCS;
			break;
		}
		return (KelvinToFahrenheit(quad->GetTemp()));
	}
		return 0;
}



//
// ECS state for the ProjectApolloMFD.
//

void LEM::GetECSStatus(LEMECSStatus &ecs)

{
	// Crew

	if (CDRSuited->number == 1)
	{
		ecs.cdrInSuit = true;
	}
	else
	{
		ecs.cdrInSuit = false;
	}

	if (LMPSuited->number == 1)
	{
		ecs.lmpInSuit = true;
	}
	else
	{
		ecs.lmpInSuit = false;
	}

	ecs.crewNumber = CrewInCabin->number + CDRSuited->number + LMPSuited->number;
	ecs.crewStatus = 0;


}

void LEM::SetCrewNumber(int number)
{
	int crewsuited = CDRSuited->number + LMPSuited->number;

	if (number + crewsuited <= 3)
	{
		CrewInCabin->number = number;
	}
}

void LEM::SetCDRInSuit()
{
	if (CrewInCabin->number >= 1 && CDRSuited->number == 0)
	{
		CrewInCabin->number--;
		CDRSuited->number = 1;
	}
	else if (CDRSuited->number == 1)
	{
		CrewInCabin->number++;
		CDRSuited->number = 0;
	}
}

void LEM::SetLMPInSuit()
{
	if (CrewInCabin->number >= 1 && LMPSuited->number == 0)
	{
		CrewInCabin->number--;
		LMPSuited->number = 1;
	}
	else if (LMPSuited->number == 1)
	{
		CrewInCabin->number++;
		LMPSuited->number = 0;
	}
}

void LEM::CheckDescentStageSystems()
{
	if (stage < 2) {
		// Descent Stage Deadface Bus Stubs wire to the ECAs

		DES_LMPs28VBusA.WireTo(&ECA_1a);
		DES_LMPs28VBusB.WireTo(&ECA_1b);
		DES_CDRs28VBusA.WireTo(&ECA_2a);
		DES_CDRs28VBusB.WireTo(&ECA_2b);
		DSCBattFeedTB.SetState(1);
	}
	else {
		DES_LMPs28VBusA.Disconnect();
		DES_LMPs28VBusB.Disconnect();
		DES_CDRs28VBusA.Disconnect();
		DES_CDRs28VBusB.Disconnect();
		DSCBattFeedTB.SetState(0);

		//ECS

		DesO2Tank->space.Void();
		DesO2Manifold->space.Void();
		DesO2Manifold->IN_valve.Close();

		DesH2OTank->space.Void();
		DesH2OTank->IN_valve.Close();
		DesH2OTank->OUT_valve.Close();
		DesH2OTank->OUT2_valve.Close();

		SetPipeMaxFlow("HYDRAULIC:ASCBATINLET", 290.0 / LBH);
		DesBatCooling->space.Void();
		DesBatCooling->IN_valve.Close();
		DesBatCooling->OUT_valve.Close();

		//LR

		Boiler *lrheater = (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:LEM-LR-Antenna-Heater");
		lrheater->SetPumpOff();
		lrheater->WireTo(NULL);
	}
}

// SYSTEMS COMPONENTS

// ELECTRICAL CONTROL ASSEMBLY SUBCHANNEL
LEM_ECAch::LEM_ECAch(){
	lem = NULL;
	dc_source_tb = NULL;
	input = -1; // Flag uninit
}

void LEM_ECAch::Init(LEM *s,e_object *src,int inp){
	lem = s;
	if(input == -1){ input = inp; }
	dc_source = src;
	Volts = 24;
}

void LEM_ECAch::SaveState(FILEHANDLE scn, char *start_str, char *end_str)

{
	oapiWriteLine(scn, start_str);
	oapiWriteScenario_int(scn, "INPUT", input);
	oapiWriteLine(scn, end_str);
}

void LEM_ECAch::LoadState(FILEHANDLE scn, char *end_str)

{
	char *line;
	int dec = 0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, end_str, end_len))
			return;
		if (!strnicmp (line, "INPUT", 5)) {
			sscanf(line + 6, "%d", &dec);
			input = dec;
		}
	}
}

void LEM_ECAch::DrawPower(double watts){ 
	power_load += watts;
};

void LEM_ECAch::UpdateFlow(double dt){
	// ECA INPUTS CAN BE PARALLELED, BUT NOT IN THE SAME CHANNEL
	// That is, Battery 1 and 2 can be on at the same time.
	// Draw power from the source, and retake voltage, etc.

	// Take power
	switch(input){
		case 1: // HI tap
			if(dc_source != NULL){
				dc_source->DrawPower(power_load); // Draw 1:1
			}
			break;
		case 2: // LO tap
			if(dc_source != NULL){
				dc_source->DrawPower(power_load*1.06); // Draw 6% more
			}
			break;
	}
	
	// Resupply from source
	switch(input){
		case 0: // NULL
			Volts = 0;
			Amperes = 0;
			break;
		case 1: // HV
			if(dc_source != NULL){
				Volts =   dc_source->Voltage();
				Amperes = dc_source->Current();
			}
			break;
		case 2: // LV
			if(dc_source != NULL){
				Volts =   (dc_source->Voltage()*0.93);
				Amperes = dc_source->Current();
			}
			break;
	}

	// Reset for next pass.
	e_object::UpdateFlow(dt);	
}

// BUS TIE BLOCK

LEM_BusFeed::LEM_BusFeed(){
	lem = NULL;
	dc_source_a = NULL;
	dc_source_b = NULL;
}

void LEM_BusFeed::Init(LEM *s,e_object *sra,e_object *srb){
	lem = s;
	dc_source_a = sra;
	dc_source_b = srb;
	Volts = 0;
}

void LEM_BusFeed::DrawPower(double watts)
{ 
	power_load += watts;
};

void LEM_BusFeed::UpdateFlow(double dt){
	//sprintf(oapiDebugString(),"BTO Input = %d Voltage %f Load %f",input,Volts,power_load);
	// Draw power from the source, and retake voltage, etc.

	int csrc=0;                             // Current Sources Operational
	double PowerDrawPerSource;              // Current to draw, per source
	double A_Volts = 0;
	double A_Amperes = 0;
	double B_Volts = 0;
	double B_Amperes = 0;

	if (dc_source_a != NULL)
	{
		A_Volts = dc_source_a->Voltage();
	}
	if (dc_source_b != NULL)
	{
		B_Volts = dc_source_b->Voltage();
	}
	
	// Find active sources
	if(A_Volts > 0){
		csrc++;
	}
	if(B_Volts > 0){
		csrc++;
	}
	// Compute draw
	if(csrc > 1){
		PowerDrawPerSource = power_load/2;
	}else{
		PowerDrawPerSource = power_load;
	}

	// Now take power
	if(dc_source_a != NULL && A_Volts > 0){
		dc_source_a->DrawPower(PowerDrawPerSource); 
	}
	if(dc_source_b != NULL && B_Volts > 0){
		dc_source_b->DrawPower(PowerDrawPerSource); 
	}

	// Resupply from source
	if(dc_source_a != NULL){
		A_Volts =   dc_source_a->Voltage();
		A_Amperes = dc_source_a->Current();
	}
	if(dc_source_b != NULL){
		B_Volts = dc_source_b->Voltage();
		B_Amperes = dc_source_b->Current();
	}
	// Final output
	switch(csrc){
		case 2: // DUAL
			Volts = (A_Volts + B_Volts) / 2;
			Amperes = A_Amperes+B_Amperes;
			break;
		case 1: // SINGLE
			if(A_Volts > 0){ // Only one (or no) input
				Volts = A_Volts;
				Amperes = A_Amperes;
			}else{
				Volts = B_Volts;
				Amperes = B_Amperes;
			}
			break;
		default: // OFF OR OTHER
			Volts = 0;
			Amperes = 0;
			break;
	}

	// if(this == &lem->BTB_CDR_D){ sprintf(oapiDebugString(),"LM_BTO: = Voltages %f %f | Load %f PS %f Output %f V",A_Volts,B_Volts,power_load,PowerDrawPerSource,Volts); }

	// Reset for next pass.
	power_load = 0;
}

// XLUNAR BUS MANAGER OUTPUT SOURCE
LEM_XLBSource::LEM_XLBSource(){
	Volts = 0;
	enabled = true;
}

void LEM_XLBSource::SetVoltage(double v){
	Volts = v;
}

void LEM_XLBSource::DrawPower(double watts)
{ 
	power_load += watts;
}

// XLUNAR BUS MANAGER
LEM_XLBControl::LEM_XLBControl(){
	lem = NULL;
}

void LEM_XLBControl::Init(LEM *s){
	lem = s;
	dc_output.SetVoltage(0);
}

// Depreciated - Don't tie directly
void LEM_XLBControl::DrawPower(double watts)
{ 
	power_load += watts;
};

void LEM_XLBControl::UpdateFlow(double dt){
	// If we have no LEM, punt
	if(lem == NULL){ return; }
	// Do we have power from the other side?
	double sVoltage = lem->CSMToLEMPowerSource.Voltage();	
	// Is the CSM Power relay latched?
	if(lem->CSMToLEMPowerConnector.csm_power_latch == 1){
		// Yes, we can put voltage on the CDR bus
		dc_output.SetVoltage(sVoltage);
	}else{
			// No, we have no return path, so we have no voltage.
			dc_output.SetVoltage(0);
		}
	// Handle switchery
	switch(lem->CSMToLEMPowerConnector.csm_power_latch){
		case 1:
			// If the CSM latch is set, keep the descent ECAs off
			lem->ECA_1a.input = 0; lem->ECA_1b.input = 0;
			lem->ECA_2a.input = 0; lem->ECA_2b.input = 0;
			break;
		case -1:
			// If the CSM latch is reset, turn on the LV taps on batteries 1 and 4.
			// And reset the latch to zero
			lem->ECA_1a.input = 2; lem->ECA_1b.input = 0;
			lem->ECA_2a.input = 0; lem->ECA_2b.input = 2;
			lem->CSMToLEMPowerConnector.csm_power_latch = 0;
			break;
	}	
	// So then, do we have xlunar voltage?
	if(dc_output.Voltage() > 0){
		// Process load at our feed point
		lem->CSMToLEMPowerSource.DrawPower(dc_output.PowerLoad());
		// sprintf(oapiDebugString(),"Drawing %f watts from CSM",dc_output.PowerLoad());
		dc_output.UpdateFlow(dt); // Shouldn't touch voltage since it has no SRC
	}
	
};

void LEM_XLBControl::SaveState(FILEHANDLE scn, char *start_str, char *end_str)

{
	oapiWriteLine(scn, start_str);
	oapiWriteScenario_int(scn, "CSMPOWERLATCH", lem->CSMToLEMPowerConnector.csm_power_latch);
	oapiWriteLine(scn, end_str);
}

void LEM_XLBControl::LoadState(FILEHANDLE scn, char *end_str)

{
	char *line;
	int dec = 0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len))
			return;
		if (!strnicmp(line, "CSMPOWERLATCH", 13)) {
			sscanf(line + 14, "%d", &dec);
			lem->CSMToLEMPowerConnector.csm_power_latch = dec;
		}
	}
}

// CROSS-TIE BALANCER OUTPUT SOURCE
LEM_BCTSource::LEM_BCTSource(){
	Volts = 0;
}

void LEM_BCTSource::SetVoltage(double v){
	Volts = v;
}

// BUS CROSS-TIE BALANCER
LEM_BusCrossTie::LEM_BusCrossTie(){
	lem = NULL;
	dc_bus_lmp = NULL;
	dc_bus_cdr = NULL;
	lmp_bal_cb = NULL;	lmp_bus_cb = NULL;
	cdr_bal_cb = NULL;	cdr_bus_cb = NULL;
	last_cdr_ld = 0;
	last_lmp_ld = 0;
}

void LEM_BusCrossTie::Init(LEM *s,DCbus *sra,DCbus *srb,CircuitBrakerSwitch *cb1,CircuitBrakerSwitch *cb2,CircuitBrakerSwitch *cb3,CircuitBrakerSwitch *cb4){
	lem = s;
	dc_bus_lmp = sra;
	dc_bus_cdr = srb;
	lmp_bal_cb = cb1;	lmp_bus_cb = cb2;
	cdr_bal_cb = cb3;	cdr_bus_cb = cb4;
	dc_output_lmp.SetVoltage(0);
	dc_output_cdr.SetVoltage(0);	
	last_cdr_ld = 0;
	last_lmp_ld = 0;
}

// Depreciated - Don't tie directly
void LEM_BusCrossTie::DrawPower(double watts)
{ 
	power_load += watts;
};

void LEM_BusCrossTie::UpdateFlow(double dt){
	// Voltage, load, load-share-difference
	double cdr_v,cdr_l,cdr_ld;
	double lmp_v,lmp_l,lmp_ld;
	double loadshare;

	lmp_v = lem->BTB_LMP_A.Voltage(); // Measure bus voltages at their A tie point, so we don't get our own output 
	cdr_v = lem->BTB_CDR_A.Voltage(); 
	lmp_l = dc_bus_lmp->PowerLoad();
	cdr_l = dc_bus_cdr->PowerLoad();

	// If both busses are dead or both CBs on either side are out, the output is dead.
	if((cdr_v == 0 && lmp_v == 0) ||
		(lmp_bus_cb->GetState() == 0 && lmp_bal_cb->GetState() == 0) || 
		(cdr_bus_cb->GetState() == 0 && cdr_bal_cb->GetState() == 0)){ 
		dc_output_lmp.SetVoltage(0);
		dc_output_cdr.SetVoltage(0);
		lem->CDRs28VBus.UpdateFlow(dt);
		lem->LMPs28VBus.UpdateFlow(dt);
		return;
	}

	// Compute load-share and differences.
	if(lmp_v == 0 || cdr_v == 0){
		// We lost power on one or both busses. Reset the stored load split.
		last_cdr_ld = 0;
		last_lmp_ld = 0;
		// If one bus is powered, but the other is not,
		// we feed the dead bus from the live one.
		lem->CDRs28VBus.UpdateFlow(dt);
		lem->LMPs28VBus.UpdateFlow(dt);
		if(cdr_v == 0){
			// Draw CDR load from LMP side and equalize voltage
			dc_output_cdr.SetVoltage(lmp_v);
			dc_output_lmp.SetVoltage(0);
			dc_bus_lmp->DrawPower(cdr_l);
			double Draw = cdr_l / lmp_v;
			if(lmp_bus_cb->GetState() > 0){			
				if(Draw > 100){
					lmp_bus_cb->SetState(0);
				}
				if(lmp_bal_cb->GetState() > 0 && Draw > 60){
					lmp_bal_cb->SetState(0);
				}
			}else{
				if(lmp_bal_cb->GetState() > 0 && Draw > 30){
					lmp_bal_cb->SetState(0);
				}
			}
		}else{
			// Draw LMP load from CDR side and equalize voltage
			dc_output_lmp.SetVoltage(cdr_v);
			dc_output_cdr.SetVoltage(0);
			dc_bus_cdr->DrawPower(lmp_l);
			double Draw = lmp_l / cdr_v;
			if(cdr_bus_cb->GetState() > 0){			
				if(Draw > 100){
					cdr_bus_cb->SetState(0);
				}
				if(cdr_bal_cb->GetState() > 0 && Draw > 60){
					cdr_bal_cb->SetState(0);
				}
			}else{
				if(cdr_bal_cb->GetState() > 0 && Draw > 30){
					cdr_bal_cb->SetState(0);
				}
			}
		}
		return;
	}else{
		// If both sides are powered, then one side is going to have a higher load
		// than the other. We draw power from the low-load side to feed the high-load side.
		// The higher-load side will probably have the lower voltage.
		loadshare = (lmp_l+cdr_l)/2;
		cdr_ld = loadshare - cdr_l;
		lmp_ld = loadshare - lmp_l;			
	}

	// Are we within tolerance already?
	if((cdr_ld < 0.000001 && cdr_ld > -0.000001) && (lmp_ld < 0.000001 && lmp_ld > -0.000001)){
		// In this case, the busses are already balanced.
		// Use whatever numbers we used last time.
		cdr_ld = last_cdr_ld;
		lmp_ld = last_lmp_ld;
		// sprintf(oapiDebugString(),"BCT L: LMP/CDR V %f %f L %f %f | LS %f | DF %f %f",lmp_v,cdr_v,lmp_l,cdr_l,loadshare,lmp_ld,cdr_ld);
	}else{
		// Include what we did before
		cdr_ld += last_cdr_ld;
		lmp_ld += last_lmp_ld;
		// Save this for later abuse
		last_cdr_ld = cdr_ld;
		last_lmp_ld = lmp_ld;
		// sprintf(oapiDebugString(),"BCT N: LMP/CDR V %f %f L %f %f | LS %f | DF %f %f",lmp_v,cdr_v,lmp_l,cdr_l,loadshare,lmp_ld,cdr_ld);
	}

	// If this works the load on both sides should be equal, with each bus having half the total load.
	// sprintf(oapiDebugString(),"BCT: LMP/CDR V %f %f L %f %f | LS %f | D %f %f",lmp_v,cdr_v,lmp_l,cdr_l,loadshare,lmp_ld,cdr_ld);

	lem->CDRs28VBus.UpdateFlow(dt);
	lem->LMPs28VBus.UpdateFlow(dt);

	// Transfer power from the higher-voltage side

	// Balance voltage
	// dc_output_cdr.SetVoltage((cdr_v+lmp_v)/2);	
	// dc_output_lmp.SetVoltage((cdr_v+lmp_v)/2);		

	// Transfer load (works both ways)
	dc_bus_cdr->DrawPower(cdr_ld); 
	dc_bus_lmp->DrawPower(lmp_ld);
	// Last thing we do is blow CBs on overcurrent.
	// BUS TIE blows at 100 amps, BUS BAL blows at 30 amps, or 60 amps if the TIE breaker is also closed.
	if(cdr_ld > 0){
		double Draw = cdr_ld / cdr_v;
		if(cdr_bus_cb->GetState() > 0){			
			if(Draw > 100){
				cdr_bus_cb->SetState(0);
			}
			if(cdr_bal_cb->GetState() > 0 && Draw > 60){
				cdr_bal_cb->SetState(0);
			}
		}else{
			if(cdr_bal_cb->GetState() > 0 && Draw > 30){
				cdr_bal_cb->SetState(0);
			}		
		}
	}
	if(lmp_ld > 0){
		double Draw = lmp_ld / lmp_v;
		if(lmp_bus_cb->GetState() > 0){			
			if(Draw > 100){
				lmp_bus_cb->SetState(0);
			}
			if(lmp_bal_cb->GetState() > 0 && Draw > 60){
				lmp_bal_cb->SetState(0);
			}
		}else{
			if(lmp_bal_cb->GetState() > 0 && Draw > 30){
				lmp_bal_cb->SetState(0);
			}
		}
	}
}


// AC INVERTER

LEM_INV::LEM_INV(){
	lem = NULL;
	active = 0;
	dc_input = NULL;
}

void LEM_INV::Init(LEM *s){
	lem = s;
}

void LEM_INV::DrawPower(double watts)

{ 
	power_load += watts;
};

void LEM_INV::UpdateFlow(double dt){

	// Reset these before pass
	Volts = 0;
	Amperes = 0;

	// If not active, die.
	if(!active){ return; }

	if(dc_input != NULL){
		// First take power from source
		dc_input->DrawPower(power_load*2.5);  // Add inefficiency
		// Then supply the bus
		if(dc_input->Voltage() > 24){		  // Above 24V input
			Volts = 115.0;                    // Regulator supplies 115V
		}else{                                // Otherwise
			Volts = dc_input->Voltage()*4.8;  // Falls out of regulation
		}                                     // until the load trips the CB
		Amperes = power_load/Volts;           // AC load amps
	}

	// Debug
	/*
	if(dc_input->Voltage() > 0){
		sprintf(oapiDebugString(),"INV: DC V = %f A = %f | AC LOAD = %f V = %f A = %f",
			dc_input->Voltage(),(power_load/dc_input->Voltage()*2.5),power_load,Volts,Amperes);
	}else{
		sprintf(oapiDebugString(),"INV: INPUT V = %f LOAD = %f",dc_input->Voltage(),power_load);
	}
	*/
	// Reset for next pass
	e_object::UpdateFlow(dt);
}

// Landing Radar
LEM_LR::LEM_LR()
{
	lem = NULL;
	lrheat = 0;
	antennaAngle = 24; // Position 1
}

void LEM_LR::Init(LEM *s,e_object *dc_src, h_Radiator *ant, Boiler *anheat, h_HeatLoad *hl){
	lem = s;
	// Set up antenna.
	// LR antenna is designed to operate between 0F and 185F
	// The heater switches on if the temperature gets below +55F and turns it off again when the temperature reaches +70F
	// Values in the constructor are name, pos, vol, isol
	antenna = ant;
	antheater = anheat;
	lrheat = hl;
	antenna->isolation = 0.00001;
	antenna->Area = 1250; // 1250 cm
	if(lem != NULL){
		antheater->WireTo(&lem->HTR_LR_CB);
	}
	// Attach power source
	dc_source = dc_src;
	// Clear flags
	range = 0;
	rate[0] = rate[1] = rate[2] = 0;
	rangeGood = 0;
	velocityGood = 0;
}

// Are we on?
bool LEM_LR::IsPowered()

{
	if (dc_source->Voltage() < SP_MIN_DCVOLTAGE || lem->stage > 1) { 
		return false;
	}
	return true;
}


void LEM_LR::TimeStep(double simdt){
	if(lem == NULL){ return; }
	// char debugmsg[256];
	ChannelValue val12;
	ChannelValue val13;
	ChannelValue val33;
	val12 = lem->agc.GetInputChannel(012);
	val13 = lem->agc.GetInputChannel(013);
	val33 = lem->agc.GetInputChannel(033);

	if (IsPowered())
	{
		antheater->SetPumpOff();
	}

	else
	{
		antheater->SetPumpAuto();
	}

	if (!IsPowered() ) { 
		// Clobber data.
		bool clobber = FALSE;
		if(val33[LRDataGood]){ clobber = TRUE; val33[LRDataGood] = 0; }
		if(val33[LRVelocityDataGood]){ clobber = TRUE; val33[LRVelocityDataGood] = 0; }
		if(val33[LRPos1]){ clobber = TRUE; val33[LRPos1] = 0; }
		if(val33[LRPos2]){ clobber = TRUE; val33[LRPos2] = 0; }
		if(val33[LRRangeLowScale]){ clobber = TRUE; val33[LRRangeLowScale] = 0; }
		if(clobber == TRUE){ lem->agc.SetInputChannel(033, val33); }
		return;
	}	

	// The altimeter works up to 40,000 feet.
	// The velocity data should be working by 24,000 feet.
	// Velocity Z is forward, Velocity X is lateral, meaning Velocity Y must be vertical.
	// Below 500 feet, the radar may lose lock due to zero doppler.
	// Below 50 feet, the LGC starts ignoring the radar.

	// Follow drive commands and use power
	// The antenna takes 10 seconds to move, and draws 15 watts while doing so.
	// The computer can command position 2, but it can't command position 1 from position 2.
	if(val12[LRPositionCommand] == 1 || lem->LandingAntSwitch.GetState() == THREEPOSSWITCH_DOWN){
		if(antennaAngle != 0){
			// Drive to Position 2
			antennaAngle -= (2.4*simdt);
			if(antennaAngle < 0){ antennaAngle = 0; }
			dc_source->DrawPower(140);
			// sprintf(oapiDebugString(),"LR CPos %d Pos %0.1f",val12.Bits.LRPositionCommand,antennaAngle);
		}else{
			// At position 2
			dc_source->DrawPower(125);
		}
	}else{
		if(lem->LandingAntSwitch.GetState() == THREEPOSSWITCH_CENTER && antennaAngle != 24){
			// Drive to Position 1
			antennaAngle += (2.4*simdt);
			if(antennaAngle > 24){ antennaAngle = 24; }
			dc_source->DrawPower(140);
			// sprintf(oapiDebugString(),"LR CPos %d Pos %0.1f",val12.Bits.LRPositionCommand,antennaAngle);
		}else{
			// At position 1
			dc_source->DrawPower(125);
		}
	}
	// Maintain antenna angle discretes
	// If at Pos 1
	if(antennaAngle == 24){
		// Light Pos 1
		if(val33[LRPos1] == 0){
			val33[LRPos1] = 1;
			lem->agc.SetInputChannel(033, val33);
		}
	}else{
		// Otherwise
		// Clobber Pos 1 flag
		if(val33[LRPos1] == 1){
			val33[LRPos1] = 0;
			lem->agc.SetInputChannel(033, val33);
		}
		// If at Pos 2
		if(antennaAngle == 0){
			// Light Pos 2
			if(val33[LRPos2] == 0){
				val33[LRPos2] = 1;
				lem->agc.SetInputChannel(033, val33);
			}
		}else{
			// Otherwise clobber Pos 2 flag
			if(val33[LRPos2] == 1){
				val33[LRPos2] = 0;
				lem->agc.SetInputChannel(033, val33);
			}
		}
	}

	// Data Determination
	if(lem->RadarTestSwitch.GetState() == THREEPOSSWITCH_DOWN){
		// Test Mode
		// Drive to:
		// Alt 8287 ft
		// Vel -494,1861,1331 ft/sec
		// on the LGC
		// For some reason this should show up as 8000 ft and -480 fps on the alt/alt-rate monitor?
		range = 8287;
		rate[0] = -494;
		rate[1] = 1861;
		rate[2] = 1331;
		rangeGood = 1;
		velocityGood = 1;
	}else{
		// Operate Mode
		rangeGood = 0;
		velocityGood = 0;

		MATRIX3 Rot;
		VECTOR3 pos, lrvec_glob, U_XAB, U_YAB, U_ZAB, U_RBA, U_RBB, U_RBB_lh;
		OBJHANDLE gravref;
		double alt, ang, alpha, beta, dh;

		//landing radar under CoG of LM
		dh = 3.0;

		//Gravity reference
		gravref = lem->GetGravityRef();

		//Altitude
		alt = lem->GetAltitude(ALTMODE_GROUND) - dh;

		//Rotation matrix
		lem->GetRotationMatrix(Rot);

		//state vector
		lem->GetRelativePos(gravref, pos);

		pos = pos*(length(pos) - dh) / length(pos);

		//Radar Beams Orientation Subroutine
		alpha = 6.0*RAD;
		beta = antennaAngle*RAD;

		U_XAB = _V(cos(beta), sin(alpha)*sin(beta), -sin(beta)*cos(alpha));
		U_YAB = _V(0, cos(alpha), sin(alpha));
		U_ZAB = _V(sin(beta), -sin(alpha)*cos(beta), cos(beta)*cos(alpha));

		U_RBA = _V(-cos(20.38*RAD), 0, -sin(20.38*RAD));

		U_RBB = tmul(_M(U_XAB.x, U_YAB.x, U_ZAB.x, U_XAB.y, U_YAB.y, U_ZAB.y, U_XAB.z, U_YAB.z, U_ZAB.z), U_RBA);

		//Now Left handed. But also needs to change coordinate system differences
		U_RBB_lh = _V(U_RBB.y, U_RBB.x, U_RBB.z);

		//convert local LR vector to global frame.
		lrvec_glob = mul(Rot, U_RBB_lh);

		//Angle between local vertical and LR vector
		ang = acos(dotp(unit(-pos), unit(lrvec_glob)));

		//Assumption: Moon is flat
		range = alt / cos(ang)/0.3048;

		//Doesn't point at the moon
		if (range < 0)
		{
			range = 1000000.0;
		}

		if (range > 10.0 && range < 50000.0) //Arbitrary, goal is lock on at 40,000 feet altitude
		{
			rangeGood = 1;
		}

		//Now velocity data
		VECTOR3 vel, vel_lh, vel_LR;

		lem->GetGroundspeedVector(FRAME_LOCAL, vel_lh);

		//In LM navigation base coordinates
		vel = _V(vel_lh.y, vel_lh.x, vel_lh.z);

		//Rotate to LR position
		vel_LR = mul(_M(U_XAB.x, U_YAB.x, U_ZAB.x, U_XAB.y, U_YAB.y, U_ZAB.y, U_XAB.z, U_YAB.z, U_ZAB.z), vel);

		rate[0] = vel_LR.x / 0.3048;
		rate[1] = vel_LR.y / 0.3048;
		rate[2] = vel_LR.z / 0.3048;

		if (range > 10.0 && range < 50000.0)
		{
			velocityGood = 1;
		}

		//sprintf(oapiDebugString(), "Alt: %f, Range: %f, Velocity: %f %f %f", alt/0.3048, range, rate[0], rate[1], rate[2]);
	}

	// Computer interface
	/*
	sprintf(debugmsg,"LR STATUS: ");
	if(val12.Bits.LRPositionCommand != 0){ sprintf(debugmsg,"%s LRPos2",debugmsg); }
	if(val13.Bits.RadarA != 0){ sprintf(debugmsg,"%s RadarA",debugmsg); }
	if(val13.Bits.RadarB != 0){ sprintf(debugmsg,"%s RadarB",debugmsg); }
	if(val13.Bits.RadarC != 0){ sprintf(debugmsg,"%s RadarC",debugmsg); }
	if(val13.Bits.RadarActivity != 0){ sprintf(debugmsg,"%s RdrActy",debugmsg); }			
	sprintf(oapiDebugString(),debugmsg);
	*/

	// Maintain discretes
	// Range data good
	if(rangeGood == 1 && val33[LRDataGood] == 0){ val33[LRDataGood] = 1; lem->agc.SetInputChannel(033, val33);	}
	if(rangeGood == 0 && val33[LRDataGood] == 1){ val33[LRDataGood] = 0; lem->agc.SetInputChannel(033, val33);	}
	// RANGE SCALE:
	// C++ VALUE OF 1 = HIGH RANGE
	// C++ VALUE OF 0 = LOW RANGE
	// We switch from high range to low range at 2500 feet
	// Range scale affects only the altimeter, velocity is not affected.
	if((rangeGood == 1 && range < 2500) && val33[LRRangeLowScale] == 0){ val33[LRRangeLowScale] = 1; lem->agc.SetInputChannel(033, val33);	}
	if((rangeGood == 0 || range > 2500) && val33[LRRangeLowScale] == 1){ val33[LRRangeLowScale] = 0; lem->agc.SetInputChannel(033, val33);	}
	// Velocity data good
	if(velocityGood == 1 && val33[LRVelocityDataGood] == 0){ val33[LRVelocityDataGood] = 1; lem->agc.SetInputChannel(033, val33);	}
	if(velocityGood == 0 && val33[LRVelocityDataGood] == 1){ val33[LRVelocityDataGood] = 0; lem->agc.SetInputChannel(033, val33);	}

	// The computer wants something from the radar.
	if(val13[RadarActivity] == 1){
		int radarBits = 0;
		if(val13[RadarA] == 1){ radarBits |= 1; }
		if(val13[RadarB] == 1){ radarBits |= 2; }
		if(val13[RadarC] == 1){ radarBits |= 4; }
		switch(radarBits){
		case 1: 
			// LR (LR VEL X)
			// 12288 COUNTS = -000000 F/S
			// SIGN REVERSED				
			// 0.643966 F/S PER COUNT
			lem->agc.vagc.Erasable[0][RegRNRAD] = (int16_t)(12288.0 - (rate[0] / 0.643966));
			lem->agc.SetInputChannelBit(013, RadarActivity, 0);
			lem->agc.GenerateRadarupt();
			ruptSent = 1;

			break;
		case 2:
			// RR RANGE RATE
			// Not our problem
			break;
		case 3:
			// LR (LR VEL Z)
			// 12288 COUNTS = +00000 F/S
			// 0.866807 F/S PER COUNT
			lem->agc.vagc.Erasable[0][RegRNRAD] = (int16_t)(12288.0 + (rate[2] / 0.866807));
			lem->agc.SetInputChannelBit(013, RadarActivity, 0);
			lem->agc.GenerateRadarupt();
			ruptSent = 3;

			break;
		case 4:
			// RR RANGE
			// Not our problem
			break;
		case 5:
			// LR (LR VEL Y)
			// 12288 COUNTS = +000000 F/S
			// 1.211975 F/S PER COUNT
			lem->agc.vagc.Erasable[0][RegRNRAD] = (int16_t)(12288.0 + (rate[1] / 1.211975));
			lem->agc.SetInputChannelBit(013, RadarActivity, 0);
			lem->agc.GenerateRadarupt();
			ruptSent = 5;

			break;
		case 7: 
			// LR (LR RANGE)
			// High range is 5.395 feet per count
			// Low range is 1.079 feet per count
			if (val33[LRRangeLowScale] == 0) {
				// Hi Range
				lem->agc.vagc.Erasable[0][RegRNRAD] = (int16_t)(range / 5.395);
			}
			else {
				// Lo Range
				lem->agc.vagc.Erasable[0][RegRNRAD] = (int16_t)(range / 1.079);
			}
			lem->agc.SetInputChannelBit(013, RadarActivity, 0);
			lem->agc.GenerateRadarupt();
			ruptSent = 7;

			break;
			/*
		default:
			sprintf(oapiDebugString(),"%s BADBITS",debugmsg);
			*/
		}

	}else{
		ruptSent = 0;
	}


	//sprintf(oapiDebugString(), "rangeGood: %d velocityGood: %d ruptSent: %d  RadarActivity: %d Position %f� Range: %f", rangeGood, velocityGood, ruptSent, val13[RadarActivity] == 1, antennaAngle, range);
}

void LEM_LR::SystemTimestep(double simdt)
{
	if (IsPowered())
	{
		lrheat->GenerateHeat(118);
	}
}

void LEM_LR::SaveState(FILEHANDLE scn,char *start_str,char *end_str){
	oapiWriteLine(scn, start_str);
	papiWriteScenario_double(scn, "RANGE", range);
	papiWriteScenario_double(scn, "ANTENNAANGLE", antennaAngle);
	oapiWriteScenario_int(scn, "RUPTSEND", ruptSent);
	oapiWriteScenario_int(scn, "RANGEGOOD", rangeGood);
	oapiWriteScenario_int(scn, "VELOCITYGOOD", velocityGood);
	papiWriteScenario_vec(scn, "RATE", _V(rate[0], rate[1], rate[2]));
	oapiWriteLine(scn, end_str);
}

void LEM_LR::LoadState(FILEHANDLE scn,char *end_str){
	char *line;
	double dec = 0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len))
			return;
		if (!strnicmp(line, "RANGE", 5)) {
			sscanf(line + 5, "%lf", &dec);
			range = dec;
		}
		if (!strnicmp(line, "ANTENNAANGLE", 12)) {
			sscanf(line + 12, "%lf", &dec);
			antennaAngle = dec;
		}
		if (!strnicmp(line, "RUPTSEND", 8)) {
			sscanf(line + 8, "%d", &ruptSent);
		}
		if (!strnicmp(line, "RANGEGOOD", 9)) {
			sscanf(line + 9, "%d", &rangeGood);
		}
		if (!strnicmp(line, "VELOCITYGOOD", 12)) {
			sscanf(line + 12, "%d", &velocityGood);
		}
		if (!strnicmp(line, "RATE", 4)) {
			sscanf(line + 4, "%lf %lf %lf", &rate[0], &rate[1], &rate[2]);
		}
	}
}

double LEM_LR::GetAntennaTempF(){
	if (lem->stage < 2) {
		return KelvinToFahrenheit(antenna->GetTemp());
	}
	else {
		return KelvinToFahrenheit(0);
	}
}

// Rendezvous Radar
// Position and draw numbers are just guesses!
LEM_RR::LEM_RR()
{
	lem = NULL;	
	RREHeat = 0;
	RRESECHeat = 0;
}

void LEM_RR::Init(LEM *s,e_object *dc_src,e_object *ac_src, h_Radiator *ant, Boiler *anheat, Boiler *stbyanheat,  h_HeatLoad *rreh, h_HeatLoad *secrreh) {
	lem = s;
	// Set up antenna.
	// RR antenna is designed to operate between 10F and 75F
	// The standby heater switches on below -40F and turns it off again at 0F
	// The oprational heater switches on below 0F and turns it off again at 20F
	//The RR assembly has multiple heater systems within, we will only concern ourselves with the antenna itself
	// The CWEA complains if the temperature is outside of -54F to +148F
	// Values in the constructor are name, pos, vol, isol
	// The DC side of the RR is most of it, the AC provides the transmit source.
	antenna = ant;
	stbyantheater = stbyanheat;
	antheater = anheat;
	RREHeat = rreh;
	RRESECHeat = secrreh;
	antenna->isolation = 0.000001; 
	antenna->Area = 9187.8912; // Area of reflecting dish, probably good enough
	trunnionAngle = -180.0 * RAD;
	shaftAngle = 0.0 * RAD; // Stow
	dc_source = dc_src;
	ac_source = ac_src;
	mode = 2;
	if (lem != NULL) {
		stbyantheater->WireTo(&lem->HTR_RR_STBY_CB);
		antheater->WireTo(&lem->HTR_RR_OPR_CB);
	}

	hpbw_factor = acos(sqrt(sqrt(0.5))) / (3.5*RAD / 4.0);	//3.5� beamwidth
	SignalStrength = 0.0;
	AutoTrackEnabled = false;
	ShaftErrorSignal = 0.0;
	TrunnionErrorSignal = 0.0;
	GyroRates = _V(0.0, 0.0, 0.0);

	for (int i = 0;i < 4;i++)
	{
		SignalStrengthQuadrant[i] = 0.0;
		U_RRL[i] = _V(0.0, 0.0, 0.0);
	}
}

bool LEM_RR::IsDCPowered()

{
	if (dc_source->Voltage() < SP_MIN_DCVOLTAGE) { 
		return false;
	}
	return true;
}

bool LEM_RR::IsACPowered()

{
	if (ac_source->Voltage() > 100) {
		return true;
	}
	return false;
}

bool LEM_RR::IsPowered()

{
	if (IsDCPowered() && IsACPowered()) {
		return true;
	}
	return false;
}

double LEM_RR::GetShaftErrorSignal()
{
	if (!IsPowered() || !AutoTrackEnabled)
	{
		return 0;
	}

	return 2.5 + ShaftErrorSignal*40.0;
}

double LEM_RR::GetTrunnionErrorSignal()
{
	if (!IsPowered() || !AutoTrackEnabled)
	{
		return 0;
	}

	return 2.5 + TrunnionErrorSignal*40.0;
}


void LEM_RR::TimeStep(double simdt){

	ChannelValue val12;
	ChannelValue val13;
	ChannelValue val14;
	ChannelValue val30;
	ChannelValue val33;
	val12 = lem->agc.GetInputChannel(012);
	val13 = lem->agc.GetInputChannel(013);
	val14 = lem->agc.GetInputChannel(014);
	val30 = lem->agc.GetInputChannel(030);
	val33 = lem->agc.GetInputChannel(033);

	double ShaftRate = 0;
	double TrunRate = 0;
	trunnionVel = 0;
	shaftVel = 0;

	/*
	This is backwards?
	if(val30.Bits.RRCDUFailure != 1){
		val30.Bits.RRCDUFailure = 1; // No failure
		lem->agc.SetInputChannel(030,val30.Value);
		sprintf(oapiDebugString(),"RR CDU Failure Flag Cleared");
	}
	*/

	if (!IsPowered() ) { 
		val33[RRPowerOnAuto] = 0;
		val33[RRDataGood] = 0;
		lem->agc.SetInputChannel(033, val33);
		SignalStrength = 0.0;
		return;
	}

	// Determine slew rate
	switch(lem->SlewRateSwitch.GetState()) {
		case TOGGLESWITCH_UP:       // HI
			ShaftRate = 7.0*RAD;
			TrunRate = 7.0*RAD;
			break;
		case TOGGLESWITCH_DOWN:     // LOW
			ShaftRate = 1.33*RAD;
			TrunRate = 1.33*RAD;
			break;
	}

	//Gyro rates
	lem->GetAngularVel(GyroRates);

	// If we are in test mode...
	if(lem->RadarTestSwitch.GetState() == THREEPOSSWITCH_UP){
		double trunnionTarget = 0,shaftTarget = 0;
		// TEST MODE:
		// NO TRACK light on
		// Range Rate to -500 FPS,
		// Shaft/Trunnion varies between +/- 5 degrees (at 0.015 d/s)
		// After 12 seconds, Range to 195.5nm and NO TRACK light out
		rate = -152.4;
		// CONTROL MOVEMENT
		// If we will run into a stop, bias the "start" angle.
		if(tstate[0] == 0){
			tsangle[0] = trunnionAngle;
			if(tsangle[0] > (RAD*85)){ tsangle[0] -= (RAD*5); }
			if(tsangle[0] < (RAD*-85)){ tsangle[0] += (RAD*5); }
			tstate[0] = 2;
		}
		if(tstate[1] == 0){
			tsangle[1] = shaftAngle;
			if(tsangle[1] < -(RAD*175)){ tsangle[1] += (RAD*5); }
			if(tsangle[1] > (RAD*85)){ tsangle[1] -= (RAD*5); }
			tstate[1] = 2;
		}
		if(tstate[0] == 1){
			trunnionTarget = tsangle[0]+(RAD*5);
		}
		if(tstate[1] == 1){
			shaftTarget += tsangle[1]+(RAD*5);
		}
		if(tstate[0] == 2){
			trunnionTarget = tsangle[0]-(RAD*5);;
		}
		if(tstate[1] == 2){
			shaftTarget = tsangle[1]-(RAD*5);;
		}
		if(trunnionAngle > trunnionTarget){
			trunnionAngle -= ((RAD*0.015)*simdt);
			if(trunnionAngle < trunnionTarget){ trunnionAngle = trunnionTarget; } // Don't overshoot
		}else{
			if(trunnionAngle < trunnionTarget){
				trunnionAngle += ((RAD*0.015)*simdt);
				if(trunnionAngle > trunnionTarget){ trunnionAngle = trunnionTarget; } // Don't overshoot
			}
		}
		if(shaftAngle > shaftTarget){
			shaftAngle -= ((RAD*0.015)*simdt);
			if(shaftAngle < shaftTarget){ shaftAngle = shaftTarget; } // Don't overshoot
		}else{
			if(shaftAngle < shaftTarget){
				shaftAngle += ((RAD*0.015)*simdt);
				if(shaftAngle > shaftTarget){ shaftAngle = shaftTarget; } // Don't overshoot
			}
		}
		// Position state advancement
		if(trunnionAngle == trunnionTarget && shaftAngle == shaftTarget){
			tstate[0]++; tstate[1]++;
			if(tstate[0] > 2){ tstate[0] = 1; }
			if(tstate[1] > 2){ tstate[1] = 1; }
		}
		// Range state advancement
		if(tstime < 12){
			tstime += simdt;
			radarDataGood = 0;
		}else{
			radarDataGood = 1;
			range = 362066; // 195.5 nautical miles in meters
		}
		//sprintf(oapiDebugString(),"RR TEST MODE TIMER %0.2f STATE T/S %d %d POS %0.2f %0.2f TPOS %0.2f %0.2f",tstime,tstate[0],tstate[1],shaftAngle*DEG,trunnionAngle*DEG,shaftTarget*DEG,trunnionTarget*DEG);
	}
	else {
		// Clobber test data if not already zero
		if (tstime > 0) { tstime = 0; tstate[0] = 0; tstate[1] = 0; }
		// We must be in normal operation.
		radarDataGood = 0;
		range = 0;
		rate = 0;

		VECTOR3 CSMPos, CSMVel, LMPos, LMVel, U_R, U_RR, R;
		MATRIX3 Rot;
		double relang;

		double anginc = 0.1*RAD;

		VESSEL *csm = lem->agc.GetCSM();

		if (csm)
		{

			//Global position of Earth, Moon and spacecraft, spacecraft rotation matrix from local to global
			lem->GetGlobalPos(LMPos);
			csm->GetGlobalPos(CSMPos);
			//oapiGetGlobalPos(hEarth, &R_E);
			//oapiGetGlobalPos(hMoon, &R_M);
			lem->GetRotationMatrix(Rot);

			//Vector pointing from LM to CSM
			R = CSMPos - LMPos;

			//Unit vector of it
			U_R = unit(R);

			//Unit vector of antenna in navigation base vessel's local frame, right handed
			U_RRL[0] = unit(_V(sin(shaftAngle + anginc)*cos(trunnionAngle), -sin(trunnionAngle), cos(shaftAngle + anginc)*cos(trunnionAngle)));
			U_RRL[1] = unit(_V(sin(shaftAngle - anginc)*cos(trunnionAngle), -sin(trunnionAngle), cos(shaftAngle - anginc)*cos(trunnionAngle)));
			U_RRL[2] = unit(_V(sin(shaftAngle)*cos(trunnionAngle + anginc), -sin(trunnionAngle + anginc), cos(shaftAngle)*cos(trunnionAngle + anginc)));
			U_RRL[3] = unit(_V(sin(shaftAngle)*cos(trunnionAngle - anginc), -sin(trunnionAngle - anginc), cos(shaftAngle)*cos(trunnionAngle - anginc)));

			//In LM navigation base coordinates, left handed
			for (int i = 0;i < 4;i++)
			{
				U_RRL[i] = _V(U_RRL[i].y, U_RRL[i].x, U_RRL[i].z);

				//Calculate antenna pointing vector in global frame
				U_RR = mul(Rot, U_RRL[i]);

				//relative angle between antenna pointing vector and direction of CSM
				relang = acos(dotp(U_RR, U_R));

				SignalStrengthQuadrant[i] = (pow(cos(hpbw_factor*relang), 2.0) + 1.0) / 2.0*exp(-25.0*relang*relang);
			}

			SignalStrength = (SignalStrengthQuadrant[0] + SignalStrengthQuadrant[1] + SignalStrengthQuadrant[2] + SignalStrengthQuadrant[3]) / 4.0;

			if (relang < 1.75*RAD && length(R) > 80.0*0.3048 && length(R) < 400.0*1852.0)
			{
				if (AutoTrackEnabled)
				{
					radarDataGood = 1;
					range = length(R);

					lem->GetGlobalVel(LMVel);
					csm->GetGlobalVel(CSMVel);

					rate = dotp(CSMVel - LMVel, U_R);
				}
			}

			//sprintf(oapiDebugString(), "Shaft: %f, Trunnion: %f, Relative Angle: %f�, SignalStrength %f %f %f %f", shaftAngle*DEG, trunnionAngle*DEG, relang*DEG, SignalStrengthQuadrant[0], SignalStrengthQuadrant[1], SignalStrengthQuadrant[2], SignalStrengthQuadrant[3]);

		}
	}

	// Let's test.
	// First, manage the status bit.
	if(lem->RendezvousRadarRotary.GetState() == 2){
		if(val33[RRPowerOnAuto] != 1){
			val33[RRPowerOnAuto] = 1;
			lem->agc.SetInputChannel(033, val33);
			//sprintf(oapiDebugString(),"RR Power On Discrete Enabled");
		}
	}else{
		if(val33[RRPowerOnAuto] != 0){
			val33[RRPowerOnAuto] = 0;
			lem->agc.SetInputChannel(033, val33);
			//sprintf(oapiDebugString(),"RR Power On Discrete Disabled");
		}
	}

	// Handle mode switch
	switch (lem->RendezvousRadarRotary.GetState()) {
	case 0:	// AUTO TRACK
		break;

	case 1: // SLEW
		// Watch the SLEW switch. 
		if (lem->RadarSlewSwitch.GetState() == 4) {	// Can we move up?
			trunnionAngle -= TrunRate*simdt;						// Move the trunnion
			trunnionVel = -TrunRate;
		}
		if (lem->RadarSlewSwitch.GetState() == 3) {	// Can we move down?
			trunnionAngle += TrunRate*simdt;						// Move the trunnion
			trunnionVel = TrunRate;
		}
		if (lem->RadarSlewSwitch.GetState() == 2) {
			shaftAngle += ShaftRate*simdt;
			shaftVel = ShaftRate;
		}
		if (lem->RadarSlewSwitch.GetState() == 0) {
			shaftAngle -= ShaftRate*simdt;
			shaftVel = -ShaftRate;
		}

		//sprintf(oapiDebugString(), "Ang %f Vel %f", shaftAngle*DEG, shaftVel);

		//if(lem->RadarTestSwitch.GetState() != THREEPOSSWITCH_UP){ sprintf(oapiDebugString(),"RR SLEW: SHAFT %f TRUNNION %f",shaftAngle*DEG,trunnionAngle*DEG); }
		break;
	case 2: // AGC

		if (val12[RRAutoTrackOrEnable] == 0)
		{
			int pulses;

			pulses = lem->scdu.GetErrorCounter();

			shaftVel = (RR_SHAFT_STEP*pulses);
			shaftAngle += (RR_SHAFT_STEP*pulses)*simdt;

			pulses = lem->tcdu.GetErrorCounter();

			trunnionVel = (RR_SHAFT_STEP*pulses);
			trunnionAngle += (RR_SHAFT_STEP*pulses)*simdt;
		}
		break;
	}

	//Auto Tracking Logic
	if (lem->RendezvousRadarRotary.GetState() == 0)
	{
		AutoTrackEnabled = true;
	}
	else if (lem->RendezvousRadarRotary.GetState() == 2 && val12[RRAutoTrackOrEnable] == 1)
	{
		AutoTrackEnabled = true;
	}
	else
	{
		AutoTrackEnabled = false;
	}

	//AUTO TRACKING
	if (AutoTrackEnabled && lem->RadarTestSwitch.GetState() != THREEPOSSWITCH_UP)
	{
		ShaftErrorSignal = (SignalStrengthQuadrant[0] - SignalStrengthQuadrant[1])*0.25;
		TrunnionErrorSignal = (SignalStrengthQuadrant[2] - SignalStrengthQuadrant[3])*0.25;

		shaftAngle += (ShaftErrorSignal - GyroRates.x)*simdt;
		shaftVel = ShaftErrorSignal;

		trunnionAngle += (TrunnionErrorSignal - GyroRates.y)*simdt;
		trunnionVel = TrunnionErrorSignal;

		//sprintf(oapiDebugString(), "Shaft: %f, Trunnion: %f, ShaftErrorSignal %f TrunnionErrorSignal %f", shaftAngle*DEG, trunnionAngle*DEG, ShaftErrorSignal, TrunnionErrorSignal);
	}

	if (lem->RendezvousRadarRotary.GetState() == 2)
	{

		//sprintf(oapiDebugString(),"RR MOVEMENT: SHAFT %f TRUNNION %f RANGE %f RANGE-RATE %f",shaftAngle*DEG,trunnionAngle*DEG,range,rate);

		// Maintain RADAR GOOD state
		if (radarDataGood == 1 && val33[RRDataGood] == 0) { val33[RRDataGood] = 1; lem->agc.SetInputChannel(033, val33); }
		if (radarDataGood == 0 && val33[RRDataGood] == 1) { val33[RRDataGood] = 0; lem->agc.SetInputChannel(033, val33); }
		// Maintain radar scale indicator
		// We use high scale above 50.6nm, and low scale below that.
		if (range > 93700 && val33[RRRangeLowScale] == 1) {
			// HI SCALE
			val33[RRRangeLowScale] = 0; lem->agc.SetInputChannel(033, val33);
		}
		if (range < 93701 && val33[RRRangeLowScale] == 0) {
			// LO SCALE
			val33[RRRangeLowScale] = 1; lem->agc.SetInputChannel(033, val33);
		}

		// Print status
		/*
		char debugmsg[256];
		sprintf(debugmsg,"RADAR STATUS: ");
		if(val12.Bits.ZeroRRCDU != 0){ sprintf(debugmsg,"%s ZeroRRCDU",debugmsg); }
		if(val12.Bits.EnableRRCDUErrorCounter != 0){ sprintf(debugmsg,"%s EnableEC",debugmsg); }
		if(val12.Bits.LRPositionCommand != 0){ sprintf(debugmsg,"%s LRPos2",debugmsg); }
		if(val12.Bits.RRAutoTrackOrEnable != 0){ sprintf(debugmsg,"%s RRAutoTrk",debugmsg); }
		if(val13.Bits.RadarA != 0){ sprintf(debugmsg,"%s RadarA",debugmsg); }
		if(val13.Bits.RadarB != 0){ sprintf(debugmsg,"%s RadarB",debugmsg); }
		if(val13.Bits.RadarC != 0){ sprintf(debugmsg,"%s RadarC",debugmsg); }
		if(val13.Bits.RadarActivity != 0){ sprintf(debugmsg,"%s RdrActy",debugmsg); }

		if(val14.Bits.ShaftAngleCDUDrive != 0){ sprintf(debugmsg,"%s DriveS(%f)",debugmsg,shaftAngle*DEG); }
		if(val14.Bits.TrunnionAngleCDUDrive != 0){ sprintf(debugmsg,"%s DriveT(%f)",debugmsg,trunnionAngle*DEG); }
		sprintf(oapiDebugString(),debugmsg);
		*/

		// The computer wants something from the radar.
		if (val13[RadarActivity] == 1) {
			int radarBits = 0;
			if (val13[RadarA] == 1) { radarBits |= 1; }
			if (val13[RadarB] == 1) { radarBits |= 2; }
			if (val13[RadarC] == 1) { radarBits |= 4; }
			switch (radarBits) {
			case 1:
				// LR (LR VEL X)
				// Not our problem
				break;
			case 2:
				// RR RANGE RATE
				// Our center point is at 17000 counts.
				// Counts are 0.627826 F/COUNT, negative = positive rate, positive = negative rate
				lem->agc.vagc.Erasable[0][RegRNRAD] = (int16_t)(17000.0 - (rate / 0.191361));
				lem->agc.SetInputChannelBit(013, RadarActivity, 0);
				lem->agc.GenerateRadarupt();
				ruptSent = 2;

				break;
			case 3:
				// LR (LR VEL Z)
				// Not our problem
				break;
			case 4:
				// RR RANGE
				// We use high scale above 50.6nm, and low scale below that.
				if (range > 93700) {
					// HI SCALE
					// Docs says this should be 75.04 feet/bit, or 22.8722 meters/bit
					lem->agc.vagc.Erasable[0][RegRNRAD] = (int16_t)(range / 22.8722);
				}
				else {
					// LO SCALE
					// Should be 9.38 feet/bit
					lem->agc.vagc.Erasable[0][RegRNRAD] = (int16_t)(range / 2.85902);
				}
				lem->agc.SetInputChannelBit(013, RadarActivity, 0);
				lem->agc.GenerateRadarupt();
				ruptSent = 4;

				break;
			case 5:
				// LR (LR VEL Y)
				// Not our problem
				break;
			case 7:
				// LR (LR RANGE)
				// Not our problem
				break;
				/*
			default:
				sprintf(oapiDebugString(),"%s BADBITS",debugmsg);
				*/
			}

		}
		else {
			ruptSent = 0;
		}
	}

	//Limits

	if (trunnionAngle > 70.0*RAD)
	{
		trunnionVel = 0.0;
		trunnionAngle = 70.0*RAD;
	}
	else if (trunnionAngle < -250.0*RAD)
	{
		trunnionVel = 0.0;
		trunnionAngle = -250.0*RAD;
	}

	if (shaftAngle > 68.0*RAD)
	{
		shaftAngle = 68.0*RAD;
		shaftVel = 0.0;
	}
	else if (shaftAngle < -148.0*RAD)
	{
		shaftAngle = -148.0*RAD;
		shaftVel = 0.0;
	}

	//Mode I or II determination
	if (cos(trunnionAngle) > 0.0 && mode == 2)
	{
		mode = 1;
	}
	else if (cos(trunnionAngle) < 0.0 && mode == 1)
	{
		mode = 2;
	}

	lem->tcdu.SetReadCounter(trunnionAngle);
	lem->scdu.SetReadCounter(shaftAngle);

	//sprintf(oapiDebugString(), "Shaft %f, Trunnion %f Mode %d", shaftAngle*DEG, trunnionAngle*DEG, mode);
	//sprintf(oapiDebugString(), "RRDataGood: %d ruptSent: %d  RadarActivity: %d Range: %f", val33[RRDataGood] == 0, ruptSent, val13[RadarActivity] == 1, range);
}

void LEM_RR::SystemTimestep(double simdt) {
	if (IsDCPowered())
	{
		dc_source->DrawPower(117);
		RREHeat->GenerateHeat(58.5);
		RRESECHeat->GenerateHeat(58.5);
	}

	if (IsACPowered())
	{
		ac_source->DrawPower(13.8);
		RREHeat->GenerateHeat(6.9);
		RRESECHeat->GenerateHeat(6.9);
	}

	if (abs(shaftVel) > 0.01*RAD)
	{
			dc_source->DrawPower(16.5);
	}

	if (abs(trunnionVel) > 0.01*RAD)
	{
		dc_source->DrawPower(16.5);
	}

}

void LEM_RR::SaveState(FILEHANDLE scn,char *start_str,char *end_str){
	oapiWriteLine(scn, start_str);
	papiWriteScenario_double(scn, "RR_TRUN", trunnionAngle);
	papiWriteScenario_double(scn, "RR_SHAFT", shaftAngle);
	papiWriteScenario_double(scn, "RR_ANTTEMP", GetAntennaTempF());
	oapiWriteScenario_int(scn, "RR_MODE", mode);
	oapiWriteLine(scn, end_str);
}

void LEM_RR::LoadState(FILEHANDLE scn,char *end_str){
		char *line;
	double dec = 0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, end_str, end_len))
			return;

		papiReadScenario_double(line, "RR_TRUN", trunnionAngle);
		papiReadScenario_double(line, "RR_SHAFT", shaftAngle);
		papiReadScenario_int(line, "RR_MODE", mode);
	}
}

LEM_RadarTape::LEM_RadarTape()
{
	lem = NULL;
	ac_source = NULL;
	dc_source = NULL;
	reqRange = 0;
	reqRate = 0;
	dispRange = 0;
	dispRate = 0;
	lgc_alt = 0;
	lgc_altrate = 0;
}

void LEM_RadarTape::Init(LEM *s, e_object * dc_src, e_object *ac_src){
	lem = s;
	dc_source = dc_src;
	ac_source = ac_src;
}

void LEM_RadarTape::TimeStep(double simdt) {
	
	if (!IsPowered())
	{
		return;
	}
	
	if( lem->AltRngMonSwitch.GetState()==TOGGLESWITCH_UP ) {
		if( lem->RR.IsRadarDataGood() ){
			setRange(lem->RR.GetRadarRange());
			setRate(lem->RR.GetRadarRate());
		} else {
			setRange(0);
			setRate(0);
		}
	} else {
		if (lem->ModeSelSwitch.IsUp()) // LR
		{
			if (lem->LR.IsRangeDataGood())
			{
				setRange(lem->LR.GetAltitude());
			}
			else
			{
				setRange(0);
			}
			if (lem->LR.IsVelocityDataGood())
			{
				setRate(lem->LR.GetAltitudeRate());
			}
			else
			{
				setRate(0);
			}
		}
		else if (lem->ModeSelSwitch.IsCenter()) //PGNS
		{
			setRange(lgc_alt);
			setRate(lgc_altrate);
		}
		else //AGS
		{
			setRange(lem->aea.GetAltitude());
			setRate(lem->aea.GetAltitudeRate());
		}

	}
	//
	//  Missing code to smooth out tape scrolling
	if( reqRange < (120000.0 * 0.3048) ) {
		dispRange = 6443 - 82 - (int)((reqRange * 3.2808399) * 40.0 / 1000.0);
	} else {
		dispRange = 81 + 1642 - 82 - (int)((reqRange * 0.000539956803*100.0)  * 40.0 / 1000.0);
	}
	dispRate  = 2881 - 82 -  (int)(reqRate * 3.2808399 * 40.0 * 100.0 / 1000.0);
}

void LEM_RadarTape::SystemTimeStep(double simdt) {
	if (!IsPowered())
		return;

	if (ac_source)
		ac_source->DrawPower(2.0);

	if (dc_source)
		dc_source->DrawPower(2.1);
}

bool LEM_RadarTape::IsPowered()
{
	if (dc_source->Voltage() < SP_MIN_DCVOLTAGE || ac_source->Voltage() < SP_MIN_ACVOLTAGE) {
		return false;
	}
	return true;
}

void LEM_RadarTape::SetLGCAltitude(int val) {

	int pulses;

	if (!IsPowered()) { return; }

	//if (val & 040000) { // Negative
	//	pulses = -((~val) & 077777);
	//}
	//else {
		pulses = val & 077777;
	//}

	lgc_alt = (2.345*0.3048*pulses);
}

void LEM_RadarTape::SetLGCAltitudeRate(int val) {

	int pulses;

	if (!IsPowered()) { return; }

	if (val & 040000) { // Negative
		pulses = val & 077777;
		pulses = 040000 - pulses;
	}
	else {
		pulses = val & 077777;
	}

	lgc_altrate = -(0.5*0.3048*pulses);
}

void LEM_RadarTape::SaveState(FILEHANDLE scn,char *start_str,char *end_str){
	oapiWriteLine(scn, start_str);
	oapiWriteScenario_int(scn, "RDRTAPE_RANGE", dispRange);
	oapiWriteScenario_float(scn, "RDRTAPE_RATE", dispRate);
	oapiWriteLine(scn, end_str);
}

void LEM_RadarTape::LoadState(FILEHANDLE scn,char *end_str){
	char *line;
	int value = 0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, end_str, end_len))
			return;
		if (!strnicmp (line, "RDRTAPE_RANGE", 13)) {
			sscanf(line + 13, "%d", &value);
			dispRange = value;
		}
		if (!strnicmp (line, "RDRTAPE_RATE", 12)) {
			sscanf(line + 12, "%d", &value);
			dispRate = value;
		}
	}
}

void LEM_RadarTape::RenderRange(SURFHANDLE surf, SURFHANDLE tape)
{
    oapiBlt(surf,tape,0,0,0, dispRange ,43,163, SURF_PREDEF_CK); 
}

void LEM_RadarTape::RenderRate(SURFHANDLE surf, SURFHANDLE tape)
{
    oapiBlt(surf,tape,0,0,42, dispRate ,35,163, SURF_PREDEF_CK); 
}

double LEM_RR::GetAntennaTempF(){

	return KelvinToFahrenheit(antenna->GetTemp());
}

//Cross Pointer

CrossPointer::CrossPointer()
{
	lem = NULL;
	rateErrMonSw = NULL;
	scaleSwitch = NULL;
	dc_source = NULL;
	vel_x = 0;
	vel_y = 0;
	lgc_forward = 0;
	lgc_lateral = 0;
}

void CrossPointer::Init(LEM *s, e_object *dc_src, ToggleSwitch *scaleSw, ToggleSwitch *rateErrMon)
{
	lem = s;
	dc_source = dc_src;
	scaleSwitch = scaleSw;
	rateErrMonSw = rateErrMon;
}

bool CrossPointer::IsPowered()
{
	if (dc_source->Voltage() < SP_MIN_DCVOLTAGE) {
		return false;
	}
	return true;
}

void CrossPointer::SystemTimeStep(double simdt)
{
	if (IsPowered() && dc_source)
		dc_source->DrawPower(8.0);  // take DC power
}

void CrossPointer::TimeStep(double simdt)
{
	if (!IsPowered())
	{
		vel_x = 0;
		vel_y = 0;
		return;
	}

	if (rateErrMonSw->IsUp()) //Rendezvous Radar
	{
		if (lem->RR.IsPowered())
		{
			vel_x = lem->RR.GetRadarShaftVel()*1000.0;
			vel_y = lem->RR.GetRadarTrunnionVel()*1000.0;
		}
		else
		{
			vel_x = 0;
			vel_y = 0;
		}
	}
	else //Landing Radar, Computer
	{
		double vx = 0, vy = 0;

		if (lem->ModeSelSwitch.IsUp()) //Landing Radar
		{
			if (lem->LR.IsVelocityDataGood())
			{
				vx = lem->LR.rate[2] * 0.3048;
				vy = lem->LR.rate[1] * 0.3048;
			}
			else
			{
				vx = 0;
				vy = 0;
			}
		}
		else if (lem->ModeSelSwitch.IsCenter())	//PGNS
		{
			lgc_forward = 0.5571*(double)lem->scdu.GetAltOutput();
			lgc_lateral = 0.5571*(double)lem->tcdu.GetAltOutput();

			vx = lgc_forward*0.3048;
			vy = lgc_lateral*0.3048;
		}
		else //AGS
		{
			vx = 0;
			vy = lem->aea.GetLateralVelocity()*0.3048;
		}
		vel_x = vx / 0.3048 * 20.0 / 200.0;
		vel_y = vy / 0.3048 * 20.0 / 200.0;
	}

	//10 times finer scale
	if (scaleSwitch->IsDown())
	{
		vel_x *= 10.0;
		vel_y *= 10.0;
	}

	//The output scaling is 20 for full deflection.
}

void CrossPointer::GetVelocities(double &vx, double &vy)
{
	vx = vel_x;
	vy = vel_y;
}

void CrossPointer::SaveState(FILEHANDLE scn) {

	oapiWriteLine(scn, CROSSPOINTER_END_STRING);
}

void CrossPointer::LoadState(FILEHANDLE scn) {
	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, CROSSPOINTER_END_STRING, sizeof(CROSSPOINTER_END_STRING))) {
			return;
		}

	}
}

// CWEA 

LEM_CWEA::LEM_CWEA(){
	lem = NULL;	
	CabinLowPressLt = 0;
	WaterWarningDisabled = 0;
	GlycolWarningDisabled = 0;
}

void LEM_CWEA::Init(LEM *s){
	int row=0,col=0;
	while(col < 8){
		while(row < 5){
			LightStatus[row][col] = 0;
			row++;
		}
		row = 0; col++;
	}
	lem = s;
}

void LEM_CWEA::TimeStep(double simdt){
	ChannelValue val11;
	ChannelValue val13;
	ChannelValue val30;
	ChannelValue val33;	
	ChannelValue val163;

	if(lem == NULL){ return; }
	val11 = lem->agc.GetOutputChannel(011);
	val13 = lem->agc.GetOutputChannel(013);
	val30 = lem->agc.GetInputChannel(030);
	val33 = lem->agc.GetInputChannel(033);
	val163 = lem->agc.GetOutputChannel(0163);

	// 6DS2 ASC PROP LOW
	// Pressure of either ascent helium tanks below 2773 psia prior to staging, - This reason goes out when stage deadface opens.
	// Blanket pressure in fuel or oxi lines at the bi-propellant valves of the ascent stage below 120 psia
	// FIXME: Finish this!
	LightStatus[1][0] = 1;

	// 6DS3 HI/LO HELIUM REG OUTLET PRESS
	// Enabled by DES ENG "ON" command. Disabled by stage deadface open.
	// Pressure in descent helium lines downstream of the regulators is above 260 psia or below 220 psia.
	LightStatus[2][0] = 0; // Default
	if(lem->stage < 2 && lem->deca.GetK10() && lem->deca.GetK23()){
		if(lem->DPSPropellant.GetHeliumRegulatorManifoldPressurePSI() > 260 || lem->DPSPropellant.GetHeliumRegulatorManifoldPressurePSI() < 220){
			LightStatus[2][0] = 1;
		}
	}

	// 6DS4 DESCENT PROPELLANT LOW
	// On if fuel/oxi in descent stage below 2 minutes endurance @ 25% power prior to staging.
	// (This turns out to be 5.6%)
	// Master Alarm and Tone are disabled if this is active.
	if(lem->stage < 2 && lem->DPS.thrustOn && lem->DPSPropellant.PropellantLevelLow()){
		LightStatus[3][0] = 1;
	}else{
		LightStatus[3][0] = 0;
	}

	// 6DS6 CES AC VOLTAGE FAILURE
	// Either CES AC voltage (26V or 28V) out of tolerance.
	// This power is provided by the ATCA main power supply and spins the RGAs and operate the AEA reference.
	// Disabled by Gyro Test Control in POS RT or NEG RT position.
	if(lem->SCS_ATCA_CB.Voltage() > 24 || lem->GyroTestRightSwitch.GetState() != THREEPOSSWITCH_CENTER){
		LightStatus[0][1] = 0;
	}else{ 
		LightStatus[0][1] = 1;
	}

	// 6DS7 CES DC VOLTAGE FAILURE
	// Any CES DC voltage out of tolerance.
	// All of these are provided by the ATCA main power supply.
	// Disabled by Gyro Test Control in POS RT or NEG RT position.
	if(lem->SCS_ATCA_CB.Voltage() > 24 || lem->GyroTestRightSwitch.GetState() != THREEPOSSWITCH_CENTER){
		LightStatus[1][1] = 0;
	}else{
		LightStatus[1][1] = 1;
	}

	// 6DS8 AGS FAILURE
	// On when any AGS power supply signals a failure, when AGS raises failure signal, or ASA heater fails.
	// Disabled when AGS status switch is OFF.
	// FIXME: Finish this!
	if(lem->AGSOperateSwitch.GetState() == THREEPOSSWITCH_DOWN){
		LightStatus[2][1] = 0;
	}else{
		LightStatus[2][1] = 1;
	}

	// 6DS9 LGC FAILURE
	// On when any LGC power supply signals a failure, scaler fails, LGC restarts, counter fails, or LGC raises failure signal.
	// Disabled by Guidance Control switch in AGS position.
	if((val163[Ch163DSKYWarn]) && lem->GuidContSwitch.GetState() == TOGGLESWITCH_UP){
		LightStatus[3][1] = 1;
	}else{
		LightStatus[3][1] = 0;
	}

	// 6DS10 ISS FAILURE
	// On when ISS power supply fails, PIPA fails while main engine thrusting, gimbal servo fails, CDU fails.
	// Disabled by Guidance Control switch in AGS position.
	if ((val11[ISSWarning] || val33[PIPAFailed] || val30[IMUCDUFailure] || val30[IMUFailure]) && lem->GuidContSwitch.GetState() == TOGGLESWITCH_UP){
		LightStatus[4][1] = 1;
	}else{
		LightStatus[4][1] = 0;
	}

	// 6DS11 RCS TCA WARNING
	// RCS fire command exists with no resulting chamber pressure,
	// chamber pressure present when no fire command exists,
	// opposing colinear jets on simultaneously
	// Disabled when failing TCA isol valve closes.
	// FIXME: Implement this test.
	LightStatus[0][2] = 0;

	// 6DS12 RCS A REGULATOR FAILURE
	// 6DS13 RCS B REGULATOR FAILURE
	// RCS helium line pressure above 205 pisa or below 165 psia. Disabled when main shutoff solenoid valves close.
	LightStatus[1][2] = 1;
	LightStatus[2][2] = 1;

	// 6DS14 DC BUS VOLTAGE FAILURE
	// On when CDR or SE DC bus below 26.5 V.
	if(lem->CDRs28VBus.Voltage() < 26.5 || lem->LMPs28VBus.Voltage() < 26.5){
		LightStatus[3][2] = 1;
	}else{
		LightStatus[3][2] = 0;
	}

	// 6DS16 CABIN LOW PRESSURE WARNING
	// On when cabin pressure below 4.15 psia (+/- 0.3 psia)
	// Off when cabin pressure above 4.65 psia (+/- 0.25 psia)
	// Disabled when both Atmosphere Revitalization Section Pressure Regulator Valves in EGRESS or CLOSE position.
	if(lem->ecs.GetCabinPressurePSI() < 4.15){
		CabinLowPressLt = 1;
	}
	if(lem->ecs.GetCabinPressurePSI() > 4.65 && CabinLowPressLt){
		CabinLowPressLt = 0;
	}
	// FIXME: Need to check valve when enabled
	if(CabinLowPressLt){
		LightStatus[0][3] = 1;
	}else{
		LightStatus[0][3] = 0;
	}

	// 6DS17 SUIT/FAN LOW PRESSURE WARNING
	// On when suit pressure below 3.12 psia or #2 suit circulation fan fails.
	// Suit fan failure alarm disabled when Suit Fan DP Control CB is open.
	// FIXME: IMPLEMENT #2 SUIT CIRC FAN TEST
	if(lem->ECS_SUIT_FAN_DP_CB.GetState() == 0 && lem->ecs.GetSuitPressurePSI() < 3.12){
		LightStatus[1][3] = 1;
	}

	// 6DS21 HIGH HELIUM REGULATOR OUTLET PRESSURE CAUTION
	// On when helium pressure downstream of regulators in ascent helium lines above 220 psia.
	if(lem->APSPropellant.GetHeliumRegulator1OutletPressurePSI() > 220.0){
		LightStatus[0][4] = 1;
	}else{
		LightStatus[0][4] = 0;
	}

	// 6DS22 ASCENT PROPELLANT LOW QUANTITY CAUTION
	// On when less than 10 seconds of ascent propellant/oxidizer remains.
	// Disabled when ascent engine is not firing.
	// FIXME: This test probably used a fixed setpoint instead of division. Investigate.
	if(lem->ph_Asc && lem->APS.thrustOn && lem->GetPropellantFlowrate(lem->ph_Asc) > 0 && (lem->GetPropellantMass(lem->ph_Asc)/lem->GetPropellantFlowrate(lem->ph_Asc) < 10)){
		LightStatus[1][4] = 1;
	}else{
		LightStatus[1][4] = 0;
	}

	// 6DS23 AUTOMATIC GIMBAL FAILURE CAUTION
	// On when difference in commanded and actual descent engine trim position is detected.
	// Enabled when descent engine armed and engine gimbal switch is enabled.
	// Disabled by stage deadface open.
	if (lem->stage < 2 && (abs(lem->DPS.pitchGimbalActuator.GetPosition()) >= 6.0 || abs(lem->DPS.rollGimbalActuator.GetPosition()) >= 6.0))
	{
		LightStatus[2][4] = 1;
	}
	else
	{
		LightStatus[2][4] = 0;
	}

	// 6DS26 INVERTER FAILURE CAUTION
	// On when AC bus voltage below 112V or frequency below 398hz or above 402hz.
	// Disabled when AC Power switch is off.
	if(lem->EPSInverterSwitch.GetState() != THREEPOSSWITCH_DOWN){
		if(lem->ACBusA.Voltage() < 112 || lem->ACBusB.Voltage() < 112){
			LightStatus[0][5] = 1;
		}else{
			LightStatus[0][5] = 0;
		}
	}else{
		LightStatus[0][5] = 0;
	}

	// 6DS27 BATTERY FAILURE CAUTION
	// On when over-current, reverse-current, or over-temperature condition occurs in any ascent or descent battery.
	// Disabled if affected battery is turned off.
	// FIXME: We'll ignore this for now.
	LightStatus[1][5] = 0;

	// 6DS28 RENDEZVOUS RADAR DATA FAILURE CAUTION
	// On when RR indicates Data-Not-Good.
	// Disabled when RR mode switch is not set to AUTO TRACK.
	if(lem->RendezvousRadarRotary.GetState() != 0){
		LightStatus[2][5] = 0;
	}else{
		LightStatus[2][5] = 1;
	}

	// 6DS29 LANDING RADAR was not present on LM-7 thru LM-9!
	LightStatus[3][5] = 2;

	// 6DS30 PRE-AMPLIFIER POWER FAILURE CAUTION
	// On when either ATCA solenoid driver power supply fails.
	// Disabled by stage deadface open or Abort PB press.
	LightStatus[4][5] = 0;
	if(lem->GuidContSwitch.GetState() == TOGGLESWITCH_UP && lem->CDR_SCS_ATCA_CB.Voltage() < 24){ LightStatus[4][5] = 1; }
	if(lem->GuidContSwitch.GetState() == TOGGLESWITCH_DOWN && lem->SCS_ATCA_AGS_CB.Voltage() < 24){ LightStatus[4][5] = 1; }
	// FIXME: Handle stage DF and abort PB disables

	// 6DS31 EDS RELAY FAILURE
	// On when any EDS relay fails.
	// Failures of stage relays disabled when stage relay switch in RESET position.
	// Disabled when MASTER ARM is ON or if ABORT STAGE commanded.
	if ((lem->eds.RelayBoxA.GetStageRelayMonitor() || lem->eds.RelayBoxA.GetStageRelayMonitor()) && !(lem->EDMasterArm.IsUp() || lem->AbortStageSwitch.GetState() == 0))
	{
		LightStatus[0][6] = 1;
	}
	else
	{
		LightStatus[0][6] = 0;
	}

	// 6DS32 RCS FAILURE CAUTION
	// On when helium pressure in either RCS system below 1700 psia.
	// Disabled when RCS TEMP/PRESS MONITOR switch in HELIUM position.
	LightStatus[1][6] = 0;
	if(lem->TempPressMonRotary.GetState() != 0){
		LightStatus[1][6] = 1;
	}

	// 6DS33 HEATER FAILURE CAUTION
	// On when:
	// S-Band Antenna Electronic Drive Assembly < -64.08F or > 152.63F
	// RR Assembly < -54.07F or > 147.69F
	// LR Assembly < -15.6F or > 148.9F
	// Disabled when Temperature Monitor switch selects affected assembly.
	LightStatus[2][6] = 0;
	if(lem->TempMonitorRotary.GetState() != 0 && (lem->RR.GetAntennaTempF() < -54.07 || lem->RR.GetAntennaTempF() > 147.69)){
		LightStatus[2][6] = 1;
	}
	if(lem->stage < 2 && lem->TempMonitorRotary.GetState() != 1 && (lem->LR.GetAntennaTempF() < -15.6 || lem->LR.GetAntennaTempF() > 148.9)){
		LightStatus[2][6] = 1; //Needs to not be looked at after staging as the LR is no longer attached.
	}
	if (lem->TempMonitorRotary.GetState() != 6 && (lem->SBandSteerable.GetAntennaTempF() < -64.08 || lem->SBandSteerable.GetAntennaTempF() > 152.63)) {
		LightStatus[2][6] = 1;
	}

	// 6DS34 CWEA POWER FAILURE CAUTION
	// On when any CWEA power supply indicates failure.
	// Not dimmable. Master Alarm associated with this failure cannot be silenced.
	// FIXME: We'll ignore this for now.
	LightStatus[3][6] = 0;

	// 6DS36 ECS FAILURE CAUTION
	// On when:
	// Glycol Pump Failure
	// CO2 Partial Pressure > 7.6mm
	// Water Separator Failure
	// Suit Fan #1 Failure
	// Off when (in order of failure):
	// Glycol pump pressure restored by selection of pump 2, or selecting INST(SEC) if #2 has failed
	// Restoration of normal CO2 pressure
	// Restoration of normal water separator speed
	// Selection of #2 suit fan
	// FIXME: Turned off for now.
	LightStatus[0][7] = 0;

	// 6DS37 OXYGEN QUANTITY CAUTION
	// On when:
	// < 135 psia in descent oxygen tank, or Less than full (<682.4 / 681.6 psia) ascent oxygen tanks, WHEN NOT STAGED
	// Less than 99.6 psia in ascent oxygen tank #1
	// Off by positioning O2/H20 QTY MON switch to CWEA RESET position.
	LightStatus[1][7] = 0;
	if(lem->stage < 2 && (lem->ecs.AscentOxyTank1PressurePSI() < 681.6 || lem->ecs.AscentOxyTank2PressurePSI() < 682.4)){ LightStatus[1][7] = 1; }
	if(lem->stage < 2 && (lem->ecs.DescentOxyTankPressurePSI() < 135)){ LightStatus[1][7] = 1; }
	if(lem->ecs.AscentOxyTank1PressurePSI() < 99.6){ LightStatus[1][7] = 1; }

	// 6DS38 GLYCOL FAILURE CAUTION
	// On when glycol qty low in primary coolant loop or primary loop glycol temp @ accumulator > 50F
	// Disabled by Glycol Pump to INST(SEC) position
	LightStatus[2][7] = 0;
	if (GlycolWarningDisabled == 0) {
		if (lem->ecs.GetPrimaryGlycolTempF() > 50.0) { LightStatus[2][7] = 1; }
		if (lem->ecs.GetPrimaryGlycolQuantity() < 2.5 || lem->ecs.GetSecondaryGlycolQuantity() < 0.5) { LightStatus[2][7] = 1; }
	}
	if (lem->GlycolRotary.GetState() == 0 && LightStatus[2][7] != 0){
		GlycolWarningDisabled = 1;
	}

	// 6DS39 WATER QUANTITY CAUTION
	// On when:
	// NOT STAGED: Descent water tank < 10% or less than full in either ascent tank
	// Unequal levels in either ascent tank
	// Off by positioning O2/H20 QTY MON switch to CWEA RESET position.
	LightStatus[3][7] = 0;
	if(WaterWarningDisabled == 0){
		if(lem->stage < 2 && (lem->ecs.DescentWaterTankQuantity() < 0.1)){ LightStatus[3][7] = 1; }
		if(lem->stage < 2 && (lem->ecs.AscentWaterTank1Quantity()  < 0.99 || lem->ecs.AscentWaterTank2Quantity() < 0.99)){ LightStatus[3][7] = 1; }
		if(abs(lem->ecs.AscentWaterTank1Quantity() - lem->ecs.AscentWaterTank2Quantity()) > 0.01) { LightStatus[3][7] = 1; }
	}
	if(lem->QtyMonRotary.GetState() == 0 && LightStatus[3][7] != 0){
		WaterWarningDisabled = 1;
	}

	// 6DS40 S-BAND RECEIVER FAILURE CAUTION
	// On when AGC signal lost.
	// Off when Range/TV function switch to OFF/RESET
	// Disabled when Range/TV switch is not in TV/CWEA ENABLE position
	LightStatus[4][7] = 0;
	if(lem->SBandRangeSwitch.GetState() == THREEPOSSWITCH_DOWN){
		LightStatus[4][7] = 1;
	}

	// Rendezvous Radar Caution

	LightStatus[2][5]=0;
	if(lem->RendezvousRadarRotary.GetState()==0 && lem->RR.IsRadarDataGood() == 0 ) {
		LightStatus[2][5]=1;
	}
	// CWEA TEST SWITCH FUNCTIONALITY
	if(lem->LTG_MASTER_ALARM_CB.Voltage() > 0){
		switch(lem->LampToneTestRotary.GetState()){
			case 0: // OFF
			case 7: // OFF
			default:
				break;
			case 1: // ALARM/TONE
				// Light MASTER ALARM and sound tone
				// FIXME: IMPLEMENT THIS
				break;
			case 2: // C/W 1
				// Light Panel 1 first bank warning lamps
				LightStatus[0][0] = 1; LightStatus[1][0] = 1; LightStatus[2][0] = 1; LightStatus[3][0] = 1; LightStatus[4][0] = 1;
				LightStatus[0][1] = 1; LightStatus[1][1] = 1; LightStatus[2][1] = 1; LightStatus[3][1] = 1; LightStatus[4][1] = 1;
				break;
			case 3: // ENG PB & C/W 2
				// Light engine START/STOP lights and Panel 1 second bank warning lamps
				LightStatus[0][2] = 1; LightStatus[1][2] = 1; LightStatus[2][2] = 1; LightStatus[3][2] = 1; LightStatus[4][2] = 1;
				LightStatus[0][3] = 1; LightStatus[1][3] = 1; LightStatus[2][3] = 1; LightStatus[3][3] = 1; LightStatus[4][3] = 1;
				break;
			case 4: // C/W 3
				// Light Panel 2 first bank warning lamps;
				LightStatus[0][4] = 1; LightStatus[1][4] = 1; LightStatus[2][4] = 1; LightStatus[3][4] = 1; LightStatus[4][4] = 1;
				LightStatus[0][5] = 1; LightStatus[1][5] = 1; LightStatus[2][5] = 1; /* LightStatus[3][5] = 1; */ LightStatus[4][5] = 1; // LDG RDR lamp only for LM10+
				break;
			case 5: // C/W 4
				// Light Panel 2 second bank warning lamps;
				LightStatus[0][6] = 1; LightStatus[1][6] = 1; LightStatus[2][6] = 1; LightStatus[3][6] = 1; LightStatus[4][6] = 1;
				LightStatus[0][7] = 1; LightStatus[1][7] = 1; LightStatus[2][7] = 1; LightStatus[3][7] = 1; LightStatus[4][7] = 1;
				break;
			case 6: // COMPNT
				// Light component caution and Lunar Contact lights
				// FIXME: IMPLEMENT THIS
				// Lunar Contact lights are lit in clbkPanelRedrawEvent code
				break;
		}
	}
}	

void LEM_CWEA::SaveState(FILEHANDLE scn,char *start_str,char *end_str){

}

void LEM_CWEA::LoadState(FILEHANDLE scn,char *end_str){

}

void LEM_CWEA::RedrawLeft(SURFHANDLE sf, SURFHANDLE ssf){
	int row=0,col=0,dx=0,dy=0;
	while(col < 4){
		switch(col){
			case 0:
				dx = 0; break;
			case 1:
				dx = 71; break;
			case 2:
				dx = 167; break;
			case 3:
				dx = 238; break;
		}
		while(row < 5){
			if(LightStatus[row][col] == 1 && lem->INST_CWEA_CB.Voltage() > 24){
				dy=134;
			}else{
				dy=7;
			}
			if(LightStatus[row][col] == 2){
				// Special Hack: This Lamp Doesn't Exist
				oapiBlt(sf, ssf, 8+dx, 7+(row*23), 8, 7, 67, 19);
			}else{
				oapiBlt(sf, ssf, 8+dx, 7+(row*23), 8+dx, dy+(row*23), 67, 19);
			}
			row++;
		}
		row = 0; col++;
	}
}

void LEM_CWEA::RedrawRight(SURFHANDLE sf, SURFHANDLE ssf){
	int row=0,col=0,dx=0,dy=0;
	while(col < 4){
		switch(col){
			case 0:
				dx = 0; break;
			case 1:
				dx = 71; break;
			case 2:
				dx = 146; break;
			case 3:
				dx = 217; break;
		}
		while(row < 5){
			if(LightStatus[row][col+4] == 1 && lem->INST_CWEA_CB.Voltage() > 24){
				dy = 134;
			}else{
				dy = 7;
			}
			if(LightStatus[row][col+4] == 2){
				// Special Hack: This Lamp Doesn't Exist
				oapiBlt(sf, ssf, 8+dx, 7+(row*23), 8, 7, 67, 19);
			}else{
				oapiBlt(sf, ssf, 8+dx, 7+(row*23), 330+dx, dy+(row*23), 67, 19);
			}
			row++;
		}
		row = 0; col++;
	}
}