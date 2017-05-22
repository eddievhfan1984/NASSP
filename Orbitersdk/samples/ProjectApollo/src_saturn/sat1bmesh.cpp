/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: generic Saturn base class
  Saturn 1b mesh code

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
#include "resource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "toggleswitch.h"

#include "apolloguidance.h"
#include "csmcomputer.h"
#include "dsky.h"
#include "IMU.h"

#include "saturn.h"

#include "saturn1b.h"

#include "sivb.h"
#include "s1b.h"
#include "sm.h"

//
// Meshes are loaded globally, once, so we use these global
// variables for them. Nothing vessel-specific should use a
// global variable.
//

static MESHHANDLE hSat1stg1;
static MESHHANDLE hSat1stg1low;
static MESHHANDLE hSat1intstg;
static MESHHANDLE hSat1intstglow;
static MESHHANDLE hSat1stg2;
static MESHHANDLE hSat1stg2low;
static MESHHANDLE hSat1stg21;
static MESHHANDLE hSat1stg22;
static MESHHANDLE hSat1stg23;
static MESHHANDLE hSat1stg24;
static MESHHANDLE hNosecap;
static MESHHANDLE hastp;
static MESHHANDLE hastp2;
static MESHHANDLE hCOAStarget;
static MESHHANDLE hlm_1;

static SURFHANDLE exhaust_tex;

//
// Same for particle streams.
//

/*

--- UNUSED STREAMS ---

PARTICLESTREAMSPEC srb_contrail = {
	0, 12.0, 1, 50.0, 0.3, 4.0, 4, 3.0, PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_PLOG, 1e-6, 0.1
};

PARTICLESTREAMSPEC contrail_condensation = {
	0,		// flag
	12.0,	// size
	1,		// rate
	50.0,	// velocity
	0.3,	// velocity distribution
	4.0,	// lifetime
	4,		// growthrate
	3.0,	// atmslowdown
	PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_PLOG, 1e-6, 0.1
};
*/

PARTICLESTREAMSPEC s1b_exhaust = {
	0,		// flag
	3.2,	// size
	7000,	// rate
	180.0,	// velocity
	0.15,	// velocity distribution
	0.33,	// lifetime
	4.0,	// growthrate
	0.0,	// atmslowdown 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

PARTICLESTREAMSPEC srb_exhaust = {
	0,		// flag
	2.75,	// size
	2000,	// rate
	60.0,	// velocity
	0.1,	// velocity distribution
	0.4,	// lifetime
	2.0,	// growthrate
	0.0,	// atmslowdown 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

PARTICLESTREAMSPEC solid_exhaust = {
	0, 0.5, 250, 35.0, 0.1, 0.15, 0.5, 1.0, 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

// "staging vent" particle streams
static PARTICLESTREAMSPEC stagingvent_spec = {
	0,		// flag
	2.5,	// size
	100,	// rate
	10,	    // velocity
	2,		// velocity distribution
	2,		// lifetime
	2.0,	// growthrate
	0.5,    // atmslowdown 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_FLAT, 0.1, 0.1,
	PARTICLESTREAMSPEC::ATM_FLAT, 0.1, 0.1
};

// "fuel venting" particle streams
static PARTICLESTREAMSPEC fuel_venting_spec = {
	0,		// flag
	0.8,	// size
	30,		// rate
	2,	    // velocity
	0.5,    // velocity distribution
	20,		// lifetime
	0.15,	// growthrate
	0.5,    // atmslowdown 
	PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_FLAT, 0.6, 0.6,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

// "prelaunch tank venting" particle streams
static PARTICLESTREAMSPEC prelaunchvent_spec = {
	0,		// flag
	0.4,	// size
	200,	// rate
	2,	    // velocity
	0.6,    // velocity distribution
	0.5,	// lifetime
	0.2,	// growthrate
	0.9,    // atmslowdown
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_FLAT, 0.1, 0.1,
	PARTICLESTREAMSPEC::ATM_FLAT, 0.1, 0.1
};


void Saturn1b::SetFirstStage ()
{
	SetSize (45);
	SetEmptyMass (Stage1Mass);
	SetPMI (_V(140,145,28));
	SetCrossSections (_V(395, 380, 40));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);
	ClearMeshes();

	SetFirstStageMeshes(-14.0);
	SetSecondStageMeshes(13.95);

	SetView(34.95, false);

	Offset1st = -28.5;
	SetCameraOffset (_V(-1,1.0,35.15));

	EnableTransponder (true);

	// **************************** NAV radios *************************************

	InitNavRadios (4);
}

void Saturn1b::SetFirstStageMeshes(double offset)

{
	double TCP=-54.485-TCPO;//STG0O;

	double Mass = Stage1Mass + SI_FuelMass;;
	double ro = 30;
	TOUCHDOWNVTX td[4];
	double x_target = -0.5;
	double stiffness = (-1)*(Mass*9.80655) / (3 * x_target);
	double damping = 0.9*(2 * sqrt(Mass*stiffness));
	for (int i = 0; i<4; i++) {
		td[i].damping = damping;
		td[i].mu = 3;
		td[i].mu_lng = 3;
		td[i].stiffness = stiffness;
	}
	td[0].pos.x = -cos(30 * RAD)*ro;
	td[0].pos.y = -sin(30 * RAD)*ro;
	td[0].pos.z = TCP;
	td[1].pos.x = 0;
	td[1].pos.y = 1 * ro;
	td[1].pos.z = TCP;
	td[2].pos.x = cos(30 * RAD)*ro;
	td[2].pos.y = -sin(30 * RAD)*ro;
	td[2].pos.z = TCP;
	td[3].pos.x = 0;
	td[3].pos.y = 0;
	td[3].pos.z = 15 * ro;
	
	SetTouchdownPoints(td, 4);

	//SetTouchdownPoints (_V(0,-1.0,TCP), _V(-.5,.5,TCP), _V(.5,.5,TCP));
	
	VECTOR3 mesh_dir=_V(0,0,offset);

	AddMesh (hStage1Mesh, &mesh_dir);
	mesh_dir=_V(0,0,16.2 + offset);
	AddMesh (hInterstageMesh, &mesh_dir);
}

void Saturn1b::SetFirstStageEngines()

{
	ClearThrusters();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	// ************************* propellant specs **********************************
	if (!ph_1st)
		ph_1st  = CreatePropellantResource(SI_FuelMass); //1st stage Propellant
	SetDefaultPropellantResource (ph_1st); // display 1st stage propellant level in generic HUD

	// *********************** thruster definitions ********************************

	int i;

	Offset1st = -80.1;//+STG0O;
	VECTOR3 m_exhaust_ref = {0,0,-1};

	VECTOR3 m_exhaust_pos5= {0,1.414,Offset1st+55};
    VECTOR3 m_exhaust_pos6= {1.414,0,Offset1st+55};
	VECTOR3 m_exhaust_pos7= {0,-1.414,Offset1st+55};
	VECTOR3 m_exhaust_pos8= {-1.414,0,Offset1st+55};
	VECTOR3 m_exhaust_pos1= {2.12,2.12,Offset1st+55};
    VECTOR3 m_exhaust_pos2= {2.12,-2.12,Offset1st+55};
	VECTOR3 m_exhaust_pos3= {-2.12,-2.12,Offset1st+55};
	VECTOR3 m_exhaust_pos4= {-2.12,2.12,Offset1st+55};

	// orbiter main thrusters
	th_main[0] = CreateThruster (m_exhaust_pos1, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[1] = CreateThruster (m_exhaust_pos2, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[2] = CreateThruster (m_exhaust_pos3, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[3] = CreateThruster (m_exhaust_pos4, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[4] = CreateThruster (m_exhaust_pos5, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[5] = CreateThruster (m_exhaust_pos6, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[6] = CreateThruster (m_exhaust_pos7, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[7] = CreateThruster (m_exhaust_pos8, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);

	SURFHANDLE tex = oapiRegisterExhaustTexture ("ProjectApollo/Exhaust2");
	thg_main = CreateThrusterGroup (th_main, 8, THGROUP_MAIN);
	for (i = 0; i < 8; i++)
		AddExhaust(th_main[i], 30.0, 0.80, tex);

	srb_exhaust.tex = oapiRegisterParticleTexture ("ProjectApollo/Contrail_Saturn2");
	s1b_exhaust.tex = oapiRegisterParticleTexture ("ProjectApollo/Contrail_Saturn");

	double exhpos = -4;
	double exhpos2 = -5;
	//AddExhaustStream (th_main[0], m_exhaust_pos1+_V(0,0,-15), &srb_contrail);
	//AddExhaustStream (th_main[1], m_exhaust_pos2+_V(0,0,-15), &srb_contrail);
	//AddExhaustStream (th_main[2], m_exhaust_pos3+_V(0,0,-15), &srb_contrail);
	//AddExhaustStream (th_main[3], m_exhaust_pos4+_V(0,0,-15), &srb_contrail);
	//AddExhaustStream (th_main[4], m_exhaust_pos5+_V(0,0,-15), &srb_contrail);
	//AddExhaustStream (th_main[5], m_exhaust_pos6+_V(0,0,-15), &srb_contrail);
	//AddExhaustStream (th_main[6], m_exhaust_pos7+_V(0,0,-15), &srb_contrail);
	//AddExhaustStream (th_main[7], m_exhaust_pos8+_V(0,0,-15), &srb_contrail);

	AddExhaustStream (th_main[0], m_exhaust_pos1+_V(0,0,exhpos2), &s1b_exhaust);
	AddExhaustStream (th_main[1], m_exhaust_pos2+_V(0,0,exhpos2), &s1b_exhaust);
	AddExhaustStream (th_main[2], m_exhaust_pos3+_V(0,0,exhpos2), &s1b_exhaust);
	AddExhaustStream (th_main[3], m_exhaust_pos4+_V(0,0,exhpos2), &s1b_exhaust);
	AddExhaustStream (th_main[4], m_exhaust_pos5+_V(0,0,exhpos2), &s1b_exhaust);
	AddExhaustStream (th_main[5], m_exhaust_pos6+_V(0,0,exhpos2), &s1b_exhaust);
	AddExhaustStream (th_main[6], m_exhaust_pos7+_V(0,0,exhpos2), &s1b_exhaust);
	AddExhaustStream (th_main[7], m_exhaust_pos8+_V(0,0,exhpos2), &s1b_exhaust);
	
	AddExhaustStream (th_main[0], m_exhaust_pos1+_V(0,0,exhpos), &srb_exhaust);
	AddExhaustStream (th_main[1], m_exhaust_pos2+_V(0,0,exhpos), &srb_exhaust);
	AddExhaustStream (th_main[2], m_exhaust_pos3+_V(0,0,exhpos), &srb_exhaust);
	AddExhaustStream (th_main[3], m_exhaust_pos4+_V(0,0,exhpos), &srb_exhaust);
	AddExhaustStream (th_main[4], m_exhaust_pos5+_V(0,0,exhpos), &srb_exhaust);
	AddExhaustStream (th_main[5], m_exhaust_pos6+_V(0,0,exhpos), &srb_exhaust);
	AddExhaustStream (th_main[6], m_exhaust_pos7+_V(0,0,exhpos), &srb_exhaust);
	AddExhaustStream (th_main[7], m_exhaust_pos8+_V(0,0,exhpos), &srb_exhaust);
/**/

	// Contrail
	/*for (i = 0; i < 8; i++) {
		if (contrail[i]) {
			DelExhaustStream(contrail[i]);
			contrail[i] = NULL;
		}
	}

	double conpos = -10;
	contrail[0] = AddParticleStream(&srb_contrail, m_exhaust_pos1+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[1] = AddParticleStream(&srb_contrail, m_exhaust_pos2+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[2] = AddParticleStream(&srb_contrail, m_exhaust_pos3+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[3] = AddParticleStream(&srb_contrail, m_exhaust_pos4+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[4] = AddParticleStream(&srb_contrail, m_exhaust_pos5+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[5] = AddParticleStream(&srb_contrail, m_exhaust_pos6+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[6] = AddParticleStream(&srb_contrail, m_exhaust_pos7+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[7] = AddParticleStream(&srb_contrail, m_exhaust_pos8+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	*/
}

void Saturn1b::SetSecondStage ()
{
	SetSize (22);
	SetCOG_elev (15.225);

	double EmptyMass = Stage2Mass + (LESAttached ? Abort_Mass : 0.0);

	SetEmptyMass (EmptyMass);
	SetPMI (_V(94,94,20));
	SetCrossSections (_V(267,267,97));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);

    ClearMeshes();
	SetSecondStageMeshes(13.95-12.25);
}

void Saturn1b::SetSecondStageMeshes(double offset)
{
	UINT meshidx;

	VECTOR3 mesh_dir=_V(0,0,offset);
    AddMesh (hStage2Mesh, &mesh_dir);

	mesh_dir=_V(2.45, 0, 10.55 + offset);
    AddMesh (hStageSLA1Mesh, &mesh_dir);
	mesh_dir=_V(0, 2.45, 10.55 + offset);
    AddMesh (hStageSLA2Mesh, &mesh_dir);
	mesh_dir=_V(0, -2.45, 10.55 + offset);
    AddMesh (hStageSLA3Mesh, &mesh_dir);
	mesh_dir=_V(-2.45, 0, 10.55 + offset);
    AddMesh (hStageSLA4Mesh, &mesh_dir);

	probeidx = -1;
	probeextidx = -1;
	crewidx = -1;
	cmpidx = -1;
	sidehatchidx = -1;
	sidehatchopenidx = -1;
	opticscoveridx = -1;
	nosecapidx = -1;
	meshLM_1 = -1;

	if (SaturnHasCSM()) {

		//
		// Add CSM.
		//

		AddSM(17.05 + offset, false);

		WORD CMMode = MESHVIS_VCEXTERNAL;

		if (LESAttached)
		{
			TowerOffset = 26.15 + offset;
			mesh_dir=_V(0, 0, TowerOffset);
			meshidx = AddMesh (hsat5tower, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

			//
			// If the LES is attached, no point drawing things in the external view which can't
			// actually be seen...
			//
			CMMode = MESHVIS_VC;
		}
		else if (HasProbe)
		{
			mesh_dir=_V(0,0,21.2 + offset);
			probeidx = AddMesh(hprobe, &mesh_dir);
			probeextidx = AddMesh(hprobeext, &mesh_dir);
			SetDockingProbeMesh();
		}

		//
		// ... but at least draw the CM in the external view, 
		// otherwise the BPC is floating above the SM.
		//
		mesh_dir=_V(0,0,21.2 + offset);
		meshidx = AddMesh (hCM, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		//
		// And the Crew.
		//
		if (Crewed) {
			cmpidx = AddMesh (hCMP, &mesh_dir);
			crewidx = AddMesh (hCREW, &mesh_dir);
			SetCrewMesh();
		}

		meshidx = AddMesh (hCMInt, &mesh_dir);
		SetMeshVisibilityMode (meshidx, CMMode);

		//
		// Don't Forget the Hatch
		//
		sidehatchidx = AddMesh (hFHC, &mesh_dir);
		sidehatchopenidx = AddMesh (hFHO, &mesh_dir);
		SetSideHatchMesh();

		// Optics Cover
		opticscoveridx = AddMesh (hopticscover, &mesh_dir);
		SetOpticsCoverMesh();

	} else if (NosecapAttached) {
		//
		// Add nosecap.
		//
		mesh_dir=_V(0,0,15.8 + offset);
		nosecapidx = AddMesh (hNosecap, &mesh_dir);
		SetNosecapMesh();

		//TODO: Only Apollo 5 with nosecape, but this should still be optional
		mesh_dir = _V(0, 0, 9.8 + offset);
		meshLM_1 = AddMesh(hlm_1, &mesh_dir);
	}

	// Dummy docking port so the auto burn feature of IMFD 4.2 is working
	// Remove it when a newer release of IMFD don't need that anymore
	VECTOR3 dockpos = {0,0,24.8 + offset};
	VECTOR3 dockdir = {0,0,1};
	VECTOR3 dockrot = {0,1,0};
	SetDockParams(dockpos, dockdir, dockrot);

	SetCameraOffset (_V(-1,1.0,31.15-STG1O));
    SetView(22.7, false);
}

void Saturn1b::SetSecondStageEngines ()

{
	ClearThrusters();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	//
	// ************************* propellant specs **********************************
	//

	if(ph_1st)
	{
		//
		// Delete remaining S1B stage propellant.
		//

		DelPropellantResource(ph_1st);
		ph_1st = 0;
	}

	if (!ph_3rd)
	{
		//
		// Create SIVB stage Propellant
		//

		ph_3rd  = CreatePropellantResource(SII_FuelMass);
	}

	if (!ph_ullage3)
	{
		//
		// Create SIVB stage ullage rocket propellant
		//

		ph_ullage3  = CreatePropellantResource(80.0);
	}

	//
	// display SIVB stage propellant level in generic HUD
	//

	SetDefaultPropellantResource (ph_3rd);

	//
	// *********************** thruster definitions ********************************
	//

	VECTOR3 m_exhaust_pos1= {0,0,-9.-STG1O+10};

	//
	// orbiter main thrusters
	//

	if (J2IsActive) {
		th_main[0] = CreateThruster (m_exhaust_pos1, _V( 0,0,1), THRUST_SECOND_VAC, ph_3rd, ISP_SECOND_VAC, ISP_SECOND_SL);
		thg_main = CreateThrusterGroup (th_main, 1, THGROUP_MAIN);
		AddExhaust (th_main[0], 30.0, 2.9, J2Tex);

		//
		// Set the actual stats.
		//

		SetSIVBMixtureRatio(MixtureRatio);

	} else {
		SetVentingJ2Thruster();
	}

	//
	//  Ullage rockets (3)
	//

	VECTOR3	m_exhaust_pos6= _V(-3.27,-0.46,-2-STG1O+9);
	VECTOR3 m_exhaust_pos7= _V(1.65,2.86,-2-STG1O+9);
	VECTOR3	m_exhaust_pos8= _V(1.65,-2.86,-2-STG1O+9);

	int i;

	//
	// Ullage rocket thrust and ISP is a guess for now.
	//

	th_ver[0] = CreateThruster (m_exhaust_pos6, _V( 0.45,0.0,1), 10000, ph_ullage3, 3000);
	th_ver[1] = CreateThruster (m_exhaust_pos7, _V( -0.23,-0.39,1), 10000, ph_ullage3, 3000);
	th_ver[2] = CreateThruster (m_exhaust_pos8, _V( -0.23,0.39,1), 10000, ph_ullage3, 3000);

	for (i = 0; i < 3; i++) {
		AddExhaust(th_ver[i], 7.0, 0.2, exhaust_tex);
		AddExhaustStream(th_ver[i], &solid_exhaust);
	}
	thg_ver = CreateThrusterGroup (th_ver, 3,THGROUP_USER);
}

void Saturn1b::SetVentingJ2Thruster() {

	if (stage != STAGE_ORBIT_SIVB || !ph_3rd)
		return;

	//
	// Clear old thrusters.
	//
	if (thg_main)
		DelThrusterGroup(THGROUP_MAIN, true);

	VECTOR3 m_exhaust_pos1= {0, 0, -9. - STG1O + 10};
	// Thrust "calibrated" for apoapsis after venting is about 167.5 nmi
	// To match the predicted dV of about 25 ft/s (21.7 ft/s actual / 25.6 predicted), use about 320 N thrust, but apoapsis is too high then (> 170 nmi)
	th_main[0] = CreateThruster(m_exhaust_pos1, _V(0, 0, 1), 220., ph_3rd, 300., 300.);
	thg_main = CreateThrusterGroup(th_main, 1, THGROUP_MAIN);

	fuel_venting_spec.tex = oapiRegisterParticleTexture ("ProjectApollo/Contrail_SaturnVenting");
	AddExhaustStream(th_main[0], &fuel_venting_spec);

	J2IsActive = false;
}

void Saturn1b::SeparateStage (int new_stage)

{
	VESSELSTATUS vs1;
	VESSELSTATUS vs2;

	VECTOR3 ofs1 = _V(0,0,0);
	VECTOR3 ofs2 = _V(0,0,0);

	VECTOR3 vel1 = _V(0,0,0);
	VECTOR3 vel2 = _V(0,0,0);

	GetStatus (vs1);
	GetStatus (vs2);

	vs1.eng_main = vs1.eng_hovr = 0.0;
	vs2.eng_main = vs2.eng_hovr = 0.0;

	if (stage == LAUNCH_STAGE_ONE && new_stage == LAUNCH_STAGE_SIVB)
	{
		ofs1 = OFS_STAGE1;
		vel1 = _V(0,0,-4.0);
	}

	if ((stage == PRELAUNCH_STAGE || stage == LAUNCH_STAGE_ONE) && new_stage > LAUNCH_STAGE_SIVB)
	{
		ofs1= _V(0, 0, 4.7);
		vel1 = _V(0,0,-4.0);
	}

	if (stage == LAUNCH_STAGE_SIVB && new_stage == CM_STAGE)
	{
		ofs1= _V(0, 0, 4.7);
		vel1 = _V(0,0,-2);
	}
	
	if ((stage == LAUNCH_STAGE_SIVB && new_stage != CM_STAGE) || stage == STAGE_ORBIT_SIVB)
	{
	 	ofs1 = _V(0, 0, 1.7);
		vel1 = _V(0, 0, 0);
	}

	if (stage == CSM_LEM_STAGE)
	{
	 	ofs1 = OFS_SM;
		vel1 = _V(0, 0, -0.1);
	}

	if (stage == CM_STAGE)
	{
		ofs1 = OFS_CM_CONE;
		vel1 = _V(1.0,1.0,1.0);
	}

	VECTOR3 rofs1, rvel1 = {vs1.rvel.x, vs1.rvel.y, vs1.rvel.z};
	VECTOR3 rofs2, rvel2 = {vs2.rvel.x, vs2.rvel.y, vs2.rvel.z};

	Local2Rel (ofs1, vs1.rpos);
	Local2Rel (ofs2, vs2.rpos);

	GlobalRot (vel1, rofs1);
	GlobalRot (vel2, rofs2);

	vs1.rvel.x = rvel1.x+rofs1.x;
	vs1.rvel.y = rvel1.y+rofs1.y;
	vs1.rvel.z = rvel1.z+rofs1.z;
	vs2.rvel.x = rvel2.x+rofs2.x;
	vs2.rvel.y = rvel2.y+rofs2.y;
	vs2.rvel.z = rvel2.z+rofs2.z;

	if (stage == CM_STAGE)
	{
		SetChuteStage1 ();
	}

    if (stage == LAUNCH_STAGE_ONE && new_stage == LAUNCH_STAGE_SIVB)
	{
	    vs1.vrot.x = 0.025;
		vs1.vrot.y = 0.025;
		vs1.vrot.z = 0.0;
		vs2.vrot.x = 0.0;
		vs2.vrot.y = 0.0;
		vs2.vrot.z = 0.0;
		StageS.play();

		//
		// Create S1b stage and set it up.
		//

		if (hstg1) {
			S1B *stage1 = (S1B *) oapiGetVesselInterface(hstg1);
			stage1->DefSetState(&vs1);

			S1BSettings S1Config;

			S1Config.SettingsType.S1B_SETTINGS_ENGINES = 1;
			S1Config.SettingsType.S1B_SETTINGS_FUEL = 1;
			S1Config.SettingsType.S1B_SETTINGS_GENERAL = 1;
			S1Config.SettingsType.S1B_SETTINGS_MASS = 1;

			S1Config.EngineNum = 8;
			S1Config.RetroNum = 4;
			S1Config.EmptyMass = SI_EmptyMass;
			S1Config.MainFuelKg = GetPropellantMass(ph_1st);
			S1Config.MissionTime = MissionTime;
			S1Config.VehicleNo = VehicleNo;
			S1Config.ISP_FIRST_SL = ISP_FIRST_SL;
			S1Config.ISP_FIRST_VAC = ISP_FIRST_VAC;
			S1Config.THRUST_FIRST_VAC = THRUST_FIRST_VAC;
			S1Config.CurrentThrust = GetThrusterLevel(th_main[0]);
			S1Config.LowRes = LowRes;

			stage1->SetState(S1Config);
		}

		SetSecondStage ();
		SetSecondStageEngines ();
		ShiftCentreOfMass (_V(0,0,12.25));
		SetThrusterGroupLevel(thg_ver,1.0);
	}

	if ((stage == LAUNCH_STAGE_SIVB && new_stage != CM_STAGE) || stage == STAGE_ORBIT_SIVB)
	{
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;

		CreateSIVBStage("ProjectApollo/nsat1stg2", vs1, false);

		SeparationS.play();
		SetCSMStage();

		ShiftCentreOfMass(_V(0, 0, 20.8));
	}

	if (stage == CSM_LEM_STAGE)
	{
		char VName[256];
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;
		SMJetS.play();

		GetApolloName(VName); strcat (VName, "-SM");
		hSMJet = oapiCreateVessel(VName, "ProjectApollo/SM", vs1);

		SMSettings SMConfig;

		SMConfig.SettingsType.SM_SETTINGS_ENGINES = 1;
		SMConfig.SettingsType.SM_SETTINGS_FUEL = 1;
		SMConfig.SettingsType.SM_SETTINGS_GENERAL = 1;
		SMConfig.SettingsType.SM_SETTINGS_MASS = 1;

		SMConfig.EmptyMass = SM_EmptyMass;
		SMConfig.MainFuelKg = GetPropellantMass(ph_sps);
		SMConfig.RCSAFuelKg = GetPropellantMass(ph_rcs0);
		SMConfig.RCSBFuelKg = GetPropellantMass(ph_rcs1);
		SMConfig.RCSCFuelKg = GetPropellantMass(ph_rcs2);
		SMConfig.RCSDFuelKg = GetPropellantMass(ph_rcs3);
		SMConfig.MissionTime = MissionTime;
		SMConfig.VehicleNo = VehicleNo;
		SMConfig.LowRes = LowRes;
		SMConfig.showHGA = !NoHGA;
		SMConfig.A13Exploded = ApolloExploded;

		SM *SMVessel = (SM *) oapiGetVesselInterface(hSMJet);
		SMVessel->SetState(SMConfig);

		// Store CM Propellant 
		double cmprop1 = -1;
		double cmprop2 = -1;
		if (ph_rcs_cm_1) cmprop1 = GetPropellantMass(ph_rcs_cm_1);
		if (ph_rcs_cm_2) cmprop2 = GetPropellantMass(ph_rcs_cm_2);

		SetReentryStage();

		// Restore CM Propellant
		if (cmprop1 != -1) SetPropellantMass(ph_rcs_cm_1, cmprop1); 
		if (cmprop2 != -1) SetPropellantMass(ph_rcs_cm_2, cmprop2); 

		ShiftCentreOfMass(_V(0, 0, 2.1));
	}

	if (stage == CM_STAGE)
	{
		SetChuteStage2 ();
	}

	if (stage == CM_ENTRY_STAGE_TWO)
	{
		SetChuteStage3 ();
	}

	if (stage == CM_ENTRY_STAGE_FOUR)
	{
		SetChuteStage4 ();
	}

	if (stage == CM_ENTRY_STAGE_FIVE)
	{
		SetSplashStage ();
	}

	if ((stage == PRELAUNCH_STAGE || stage == LAUNCH_STAGE_ONE) && new_stage == CM_STAGE)
	{
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;
		StageS.play();
		habort = oapiCreateVessel("Saturn_Abort", "ProjectApollo/Saturn1bAbort1", vs1);
		SetReentryStage();
		ShiftCentreOfMass(_V(0, 0, 35.15));
	}

	if (stage == LAUNCH_STAGE_SIVB && new_stage == CM_STAGE)
	{
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;
		StageS.play();
		habort = oapiCreateVessel("Saturn_Abort", "ProjectApollo/Saturn1bAbort2", vs1);
		SetReentryStage();
		ShiftCentreOfMass(_V(0, 0, 22.9));
	}
 }

//
// Load the meshes once per DLL.
//

void Saturn1bLoadMeshes()

{
	hSat1stg1 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg1");
	hSat1stg1low = oapiLoadMeshGlobal ("ProjectApollo/LowRes/nsat1stg1");
	hSat1intstg = oapiLoadMeshGlobal ("ProjectApollo/nsat1intstg");
	hSat1intstglow = oapiLoadMeshGlobal ("ProjectApollo/LowRes/nsat1intstg");
	hSat1stg2 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg2");
	hSat1stg2low = oapiLoadMeshGlobal ("ProjectApollo/LowRes/nsat1stg2");

	hSat1stg21 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg21");
	hSat1stg22 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg22");
	hSat1stg23 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg23");
	hSat1stg24 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg24");
	hastp = oapiLoadMeshGlobal ("ProjectApollo/nASTP3");
	hastp2 = oapiLoadMeshGlobal ("ProjectApollo/nASTP2");
	hCOAStarget = oapiLoadMeshGlobal ("ProjectApollo/sat_target");
	hNosecap = oapiLoadMeshGlobal ("ProjectApollo/nsat1aerocap");
	hlm_1 = oapiLoadMeshGlobal("ProjectApollo/LM_1");

	exhaust_tex = oapiRegisterExhaustTexture ("ProjectApollo/Exhaust2");
	solid_exhaust.tex = oapiRegisterParticleTexture("Contrail3");
}

//
// Update per-vessel handles to the appropriate low-res or high-res meshes.
//

void Saturn1b::SetupMeshes()

{
	if (LowRes) {
		hStage1Mesh = hSat1stg1low;
		hStage2Mesh = hSat1stg2low;
		hInterstageMesh = hSat1intstglow;
		hStageSLA1Mesh = hSat1stg21;
		hStageSLA2Mesh = hSat1stg22;
		hStageSLA3Mesh = hSat1stg23;
		hStageSLA4Mesh = hSat1stg24;
	}
	else {
		hStage1Mesh = hSat1stg1;
		hStage2Mesh = hSat1stg2;
		hInterstageMesh = hSat1intstg;
		hStageSLA1Mesh = hSat1stg21;
		hStageSLA2Mesh = hSat1stg22;
		hStageSLA3Mesh = hSat1stg23;
		hStageSLA4Mesh = hSat1stg24;
	}
}

void Saturn1b::CreateStageOne() {

	// Create hidden SIB vessel
	char VName[256];
	VESSELSTATUS vs;

	GetStatus(vs);
	strcpy (VName, GetName()); 
	strcat (VName, "-STG1");
	hstg1 = oapiCreateVessel(VName,"ProjectApollo/nsat1stg1", vs);

	// Load only the necessary meshes
	S1B *stage1 = (S1B *) oapiGetVesselInterface(hstg1);
	stage1->LoadMeshes(LowRes);
}

void Saturn1b::ActivatePrelaunchVenting()

{
	//
	// "tank venting" particle streams
	//
	static double lvl = 1.0;

	if (!prelaunchvent[0]) prelaunchvent[0] = AddParticleStream(&prelaunchvent_spec, _V(2, 1.5, 20 + STG0O), _V(1, 1, 0), &lvl);
	if (!prelaunchvent[1]) prelaunchvent[1] = AddParticleStream(&prelaunchvent_spec, _V(2, 2, 8 + STG0O), _V(1, 1, 0), &lvl);
	if (!prelaunchvent[2]) prelaunchvent[2] = AddParticleStream(&prelaunchvent_spec, _V(2, 2, 0.5 + STG0O), _V(1, 1, 0), &lvl);
}

void Saturn1b::DeactivatePrelaunchVenting()

{
	// "tank venting" particle streams
	int i;

	for (i = 0; i < 3; i++) {
		if (prelaunchvent[i]) {
			DelExhaustStream(prelaunchvent[i]);
			prelaunchvent[i] = NULL;
		}
	}
}

void Saturn1b::ActivateStagingVent()

{
	// "staging vent" particle streams
	static double lvl = 1.0;

	double zoffset = -2-STG1O+9;
	VECTOR3	m_exhaust_pos6= _V(0,5.07,zoffset);
	VECTOR3 m_exhaust_pos7= _V(0,-5.07,zoffset);
	VECTOR3	m_exhaust_pos8= _V(5.07,0,zoffset);
	VECTOR3 m_exhaust_pos9= _V(-5.07,0,zoffset);
	VECTOR3	m_exhaust_pos10= _V(3.55,3.7,zoffset);
	VECTOR3 m_exhaust_pos11= _V(3.55,-3.7,zoffset);
	VECTOR3	m_exhaust_pos12= _V(-3.55,3.7,zoffset);
	VECTOR3 m_exhaust_pos13= _V(-3.55,-3.7,zoffset);

	if (!stagingvent[0]) stagingvent[0] = AddParticleStream (&stagingvent_spec, m_exhaust_pos10, _V( 1, 1,-1), &lvl);
	if (!stagingvent[1]) stagingvent[1] = AddParticleStream (&stagingvent_spec, m_exhaust_pos11, _V( 1,-1,-1), &lvl);
	if (!stagingvent[2]) stagingvent[2] = AddParticleStream (&stagingvent_spec, m_exhaust_pos12, _V(-1, 1,-1), &lvl);
	if (!stagingvent[3]) stagingvent[3] = AddParticleStream (&stagingvent_spec, m_exhaust_pos13, _V(-1,-1,-1), &lvl);
	if (!stagingvent[4]) stagingvent[4] = AddParticleStream (&stagingvent_spec, m_exhaust_pos6,  _V( 0, 1,-1), &lvl);
	if (!stagingvent[5]) stagingvent[5] = AddParticleStream (&stagingvent_spec, m_exhaust_pos7,  _V( 0,-1,-1), &lvl);
	if (!stagingvent[6]) stagingvent[6] = AddParticleStream (&stagingvent_spec, m_exhaust_pos8,  _V( 1, 0,-1), &lvl);
	if (!stagingvent[7]) stagingvent[7] = AddParticleStream (&stagingvent_spec, m_exhaust_pos9,  _V(-1, 0,-1), &lvl);
}

void Saturn1b::DeactivateStagingVent()

{
	//
	// "staging vent" particle streams
	//
	int i;

	for (i = 0; i < 8; i++) {
		if (stagingvent[i]) {
			DelExhaustStream(stagingvent[i]);
			stagingvent[i] = NULL;
		}
	}
}