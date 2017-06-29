/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: Saturn 5
  Virtual cockpit code

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
#include <stdio.h>
#include <math.h>
#include "soundlib.h"
#include "resource.h"

#define LOADBMP(id) (LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (id)))

#include "nasspdefs.h"
#include "nasspsound.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "saturn.h"
#include "saturnv.h"
#include "tracer.h"
#include "papi.h"


void Saturn::InitVC (int vc)
{
	//int i;
	TRACESETUP("Saturn::InitVC");

	switch (vc) {
	case 0:
		//LOAD SURFACES
		//srf[#] = oapiCreateSurface (LOADBMP (BMP_IDENITIFIER}};   oapiSetSurfaceColourKey (srf[#], 0);
		
		//reset state flags (see DeltaGlider for similar)
		break;
	}
}

bool Saturn::clbkLoadVC (int id)
{
	TRACESETUP("Saturn::clbkLoadVC");

	InVC = true;
	InPanel = false;

	if ((viewpos >= SATVIEW_ENG1) && (viewpos <= SATVIEW_ENG6))
		return true;

	ReleaseSurfaces();
	InitVC(id);

	SetView(true);
	
	// Test stuff
	//SURFHANDLE tex# = oapiGetTextureHandle (vcmeshidentifier, meshgroup#);
	//int i;
	//SetCameraDefaultDirection (_V(0,0,1));
	//default camera direction: forward
	//SetCameraShiftRange (_V(#,#,#), _V(#,#,#), _V(#,#,#));
	// leaning forward/left/right

	return VCRegistered;
}

bool Saturn::RegisterVC()
{
	VCRegistered = true;

	switch (viewpos) {

	case SATVIEW_LEFTSEAT: //can only reach as far over as middle of panel 2 and side panels
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);


		//register areas and AreaClickmodes
		//oapiVCRegisterArea (areaidentifier, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_"event1"|PANEL_MOUSE_"event2");
		//oapiVCSetAreaClickmode_Spherical (areaidentifier, _V(#x,#y,#z,#radius);
		//oapiVCSetAreaClickmode_Quadrilateral (areaidentifier, _V(uplftvect), _V(uprtvect), _V(lwrlftvect), _V(lwrrtvect));
		oapiVCRegisterArea (AID_MASTER_ALARM, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
			oapiVCSetAreaClickmode_Spherical (AID_MASTER_ALARM, _V(-0.205,-0.243,0.61),0.015);
		oapiVCRegisterArea (AID_DSKY_KEY, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
			oapiVCSetAreaClickmode_Quadrilateral (AID_DSKY_KEY, _V(0.162,-0.397,0.557), _V(0.34,-0.397,0.557), _V(0.162,-0.474,0.53), _V(0.34,-0.474,0.53));
        
		return true;

	case SATVIEW_CENTERSEAT: //can only reach Main Console
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);

		//register areas and AreaClickmodes
		//oapiVCRegisterArea (areaidentifier, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_"event1"|PANEL_MOUSE_"event2");
		//oapiVCSetAreaClickmode_Spherical (areaidentifier, _V(#x,#y,#z,#radius);
		//oapiVCSetAreaClickmode_Quadrilateral (areaidentifier, _V(uplftvect), _V(uprtvect), _V(lwrlftvect), _V(lwrrtvect));
		oapiVCRegisterArea (AID_MASTER_ALARM, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
			oapiVCSetAreaClickmode_Spherical (AID_MASTER_ALARM, _V(0.804,0.653,0.463),0.015);
		oapiVCRegisterArea (AID_DSKY_KEY, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
			oapiVCSetAreaClickmode_Quadrilateral (AID_DSKY_KEY, _V(0.442,0.5,0.407), _V(0.258,0.5,0.407), _V(0.258,0.424,0.38), _V(0.442,0.424,0.38));
        
		return true;

	case SATVIEW_RIGHTSEAT: //can only reach as far over as middle of panel 2 and side panels
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);

		//register areas and AreaClickmodes
		//oapiVCRegisterArea (areaidentifier, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_"event1"|PANEL_MOUSE_"event2");
		//oapiVCSetAreaClickmode_Spherical (areaidentifier, _V(#x,#y,#z,#radius);
		//oapiVCSetAreaClickmode_Quadrilateral (areaidentifier, _V(uplftvect), _V(uprtvect), _V(lwrlftvect), _V(lwrrtvect));

		return true;

	case SATVIEW_GNPANEL: //can only reach Lower Equipment Bay
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.8 * PI, 0.4 * PI);
		SetCameraShiftRange (_V(-0.4,0,0), _V(0,0,0), _V(0,0,0));

		
		//register areas and AreaClickmodes
		//oapiVCRegisterArea (areaidentifier, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_"event1"|PANEL_MOUSE_"event2");
		//oapiVCSetAreaClickmode_Spherical (areaidentifier, _V(#x,#y,#z,#radius);
		//oapiVCSetAreaClickmode_Quadrilateral (areaidentifier, _V(uplftvect), _V(uprtvect), _V(lwrlftvect), _V(lwrrtvect));

		return true;

	case SATVIEW_LEFTDOCK: //can only control COAS and limited eye movement
		SetCameraRotationRange(0,0,0,0);

		//register areas and AreaClickmodes
		//oapiVCRegisterArea (areaidentifier, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_"event1"|PANEL_MOUSE_"event2");
		//oapiVCSetAreaClickmode_Spherical (areaidentifier, _V(#x,#y,#z,#radius);
		//oapiVCSetAreaClickmode_Quadrilateral (areaidentifier, _V(uplftvect), _V(uprtvect), _V(lwrlftvect), _V(lwrrtvect));
		
		return true;

	case SATVIEW_RIGHTDOCK: //can only control COAS and limited eye movement
		SetCameraRotationRange(0,0,0,0);
		
		return true;

	default:
		VCRegistered = false;
		return false;
	}
}

// --------------------------------------------------------------
// Respond to virtual cockpit mouse event
// --------------------------------------------------------------
bool Saturn::clbkVCMouseEvent (int id, int event, VECTOR3 &p)
{
	TRACESETUP("Saturn::clbkVCMouseEvent");
	switch (id) {
	//case areaidentifier:
	    //event stuff here
		//return true;
	//case areaidentifier:
		//blah blah blah
		//return true;
	//}

	case AID_MASTER_ALARM:
	//case AID_MASTER_ALARM2:
	//case AID_MASTER_ALARM3:
		sprintf(oapiDebugString(), "Alarming...");
		return cws.CheckMasterAlarmMouseClick(event);

	case AID_DSKY_KEY:
		sprintf(oapiDebugString(), "DSKY-ing...");
		int mx;
		int my;

		mx = int(p.x * 288);
		my = int(p.y * 121);

		if (event & PANEL_MOUSE_LBDOWN) {
			dsky.ProcessKeyPress(mx, my);
		} else if (event & PANEL_MOUSE_LBUP) {
			dsky.ProcessKeyRelease(mx, my);
		}
		return true;

	/*case AID_DSKY2_KEY:
		if (event & PANEL_MOUSE_LBDOWN) {
			dsky2.ProcessKeyPress(mx, my);
		} else if (event & PANEL_MOUSE_LBUP) {
			dsky2.ProcessKeyRelease(mx, my);
		}
		return true;*/
	}
	return false;
}

// --------------------------------------------------------------
// Respond to virtual cockpit area redraw request
// --------------------------------------------------------------
bool Saturn::clbkVCRedrawEvent (int id, int event, SURFHANDLE surf)
{
	TRACESETUP("Saturn::clbkVCRedrawEvent");
	//int i;

	switch (id) {
	//case areaidentifier
	//	Redraw Panel stuff
	//	return true if dynamic texture modified, false if not

	/*
	
	case AID_DSKY_KEY:
		dsky.RenderKeys(surf, srf[SRF_DSKYKEY]);
		return true;

	case AID_ABORT_BUTTON:
		if (ABORT_IND) {
			oapiBlt(surf,srf[SRF_ABORT], 0, 0, 62, 0, 62, 31);
		}
		else {
			oapiBlt(surf,srf[SRF_ABORT], 0, 0, 0, 0, 62, 31);
		}
		return true;
		
	case AID_MASTER_ALARM:
		cws.RenderMasterAlarm(surf, srf[SRF_MASTERALARM_BRIGHT], CWS_MASTERALARMPOSITION_LEFT);
		return true;

	case AID_MASTER_ALARM2:
		cws.RenderMasterAlarm(surf, srf[SRF_MASTERALARM_BRIGHT], CWS_MASTERALARMPOSITION_RIGHT);
		return true;

	case AID_MASTER_ALARM3:
		cws.RenderMasterAlarm(surf, srf[SRF_MASTERALARM_BRIGHT], CWS_MASTERALARMPOSITION_NONE);
		return true;
		*/

	case AID_MASTER_ALARM:  //temp place holder so C++ doesn't complain
		return false;

	default:
		return false;
	}
}


void Saturn::JostleViewpoint(double amount)

{
	double j = ((double) ((rand() & 65535) - 32768) * amount) / 3276800.0;
	ViewOffsetx += j;

	j = ((double) ((rand() & 65535) - 32768) * amount) / 3276800.0;
	ViewOffsety += j;

	j = ((double) ((rand() & 65535) - 32768) * amount) / 3276800.0;
	ViewOffsetz += j;

	if (ViewOffsetx > 0.10)
		ViewOffsetx = 0.10;
	if (ViewOffsetx < -0.10)
		ViewOffsetx = -0.10;

	if (ViewOffsety > 0.10)
		ViewOffsety = 0.10;
	if (ViewOffsety < -0.10)
		ViewOffsety = -0.10;

	if (ViewOffsetz > 0.05)
		ViewOffsetz = 0.05;
	if (ViewOffsetz < -0.05)
		ViewOffsetz = -0.05;

	SetView();
}

void Saturn::SetView()

{
	SetView(CurrentViewOffset, false);
}

void Saturn::SetView(double offset)

{
	SetView(offset, false);
}

void Saturn::SetView(bool update_direction)

{
	SetView(CurrentViewOffset, update_direction);
}

void Saturn::SetView(double offset, bool update_direction)

{
	VECTOR3 v;
	TRACESETUP("Saturn::SetView");
	CurrentViewOffset = offset;
	double fov = -1;

	//
	// Engineering cameras
	//
	if (viewpos >= SATVIEW_ENG1)
	{
		VECTOR3 e1 = _V(0, 0, 0), e2 = _V(0, 0, 0), e3 = _V(0, 0, 0), e4 = _V(0, 0, 0), e5 = _V(0, 0, 0), e6 = _V(0, 0, 0);	
		VECTOR3 v1 = _V(0, 0, 0), v2 = _V(0, 0, 0), v3 = _V(0, 0, 0), v4 = _V(0, 0, 0), v5 = _V(0, 0, 0), v6 = _V(0, 0, 0);
		VECTOR3 cd;

		//
		// We really need different cameras for Saturn V and 1b.
		//

		switch (stage) {
		case PRELAUNCH_STAGE:
			e3 = _V(0.0, 7.5, -10.0+STG0O);
			v3 = _V(0.0, -0.1, -1.0);
			e4 = _V(7.5, 0.0, -10.0+STG0O);
			v4 = _V(-0.1, 0.0, -1.0);
			e5 = _V(0.0, -7.5, -10.0+STG0O);
			v5 = _V(0.0, 0.1, -1.0);
			e6 = _V(-7.5, 0.0, -10.0+STG0O);
			v6 = _V(0.1, 0.0, -1.0);
			break;

		case LAUNCH_STAGE_ONE:
			e1 = _V(4.0, 0.0, -39.0+STG0O);
			v1 = _V(-0.15, 0, 1.0);
			e2 = _V(3.5, 0.0, -31.0+STG0O);
			v2 = _V(-0.15, 0, -1.0);
			e3 = _V(0.0, 7.5, -10.0+STG0O);
			v3 = _V(0.0, -0.1, -1.0);
			e4 = _V(7.5, 0.0, -10.0+STG0O);
			v4 = _V(-0.1, 0.0, -1.0);
			e5 = _V(0.0, -7.5, -10.0+STG0O);
			v5 = _V(0.0, 0.1, -1.0);
			e6 = _V(-7.5, 0.0, -10.0+STG0O);
			v6 = _V(0.1, 0.0, -1.0);
			break;

		case LAUNCH_STAGE_TWO:
		case LAUNCH_STAGE_TWO_ISTG_JET:
			e2 = _V(3.5, 0.0, -31.0-STG1O);
			v2 = _V(-0.15, 0, -1.0);
			e3 = _V(0.0, 7.5, -10.0-STG1O);
			v3 = _V(0.0, -0.1, -1.0);
			e4 = _V(7.5, 0.0, -10.0-STG1O);
			v4 = _V(-0.1, 0.0, -1.0);
			e5 = _V(0.0, -7.5, -10.0-STG1O);
			v5 = _V(0.0, 0.1, -1.0);
			e6 = _V(-7.5, 0.0, -10.0-STG1O);
			v6 = _V(0.1, 0.0, -1.0);
			break;

		//
		// Switch back to commander view if we're past the point where we can
		// display anything useful.
		//

		case LAUNCH_STAGE_SIVB:
			viewpos = SATVIEW_LEFTSEAT;
			SetView(offset, true);
			return;
		}

		switch (viewpos) {
		case SATVIEW_ENG1:
			v = e1;
			cd = v1;
			break;

		case SATVIEW_ENG2:
			v = e2;
			cd = v2;
			break;

		case SATVIEW_ENG3:
			v = e3;
			cd = v3;
			break;

		case SATVIEW_ENG4:
			v = e4;
			cd = v4;
			break;

		case SATVIEW_ENG5:
			v = e5;
			cd = v5;
			break;

		case SATVIEW_ENG6:
			v = e6;
			cd = v6;
			break;
		}

		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
		SetCameraDefaultDirection(cd);
		oapiCameraSetCockpitDir(0,0);
	}

	// 
	// 2D panel 
	// Direction/rotation range is in clbkLoadPanel
	//
	else if (InPanel) {
		if (PanelId == SATPANEL_LEFT_RNDZ_WINDOW) {
			v = _V(-0.710051624, 1.305366408, offset - 3.0);

		} else if (PanelId == SATPANEL_RIGHT_RNDZ_WINDOW) {
			v = _V(0.710051624, 1.305366408, offset - 3.0);

		} else if (PanelId == SATPANEL_HATCH_WINDOW) {
			v = _V(0, 0, offset + 1.0);

		} else {
			// "Calibrated" for optics cover jettison as seen through the scanning telescope 
			v = _V(0, 0, offset + 0.2);
		}

		if (PanelId == SATPANEL_SEXTANT) { // Sextant
			fov = 1. * RAD; 
		
		} else if (PanelId == SATPANEL_TELESCOPE) { // Telescope
			fov = 30. * RAD;
		}
	} 

	//
	// Generic cockpit
	// Direction/rotation range is in clbkLoadGenericCockpit
	//
	else if (!InVC) {		
		v = _V(0, 0, offset - 3.0);
	} 

	//
	// Virtual cockpit
	//
	else {
		switch (viewpos) {
			case SATVIEW_LEFTSEAT:
				v = _V(-0.6, 0.9, offset);
				break;

			case SATVIEW_CENTERSEAT:
				v = _V(0, 0.9, offset);
				break;

			case SATVIEW_RIGHTSEAT:
				v = _V(0.6, 0.9, offset);
				break;

			case SATVIEW_LEFTDOCK:
				v = _V(-0.65, 1.05, 0.25 + offset);
				break;
			
			case SATVIEW_RIGHTDOCK:
				v = _V(0.65, 1.05, 0.25 + offset);
				break;

			case SATVIEW_GNPANEL:
				v = _V(0.0, -0.15, 0.5 + offset);
				break;
		}

		if (update_direction) {
			SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
			if (viewpos == SATVIEW_GNPANEL) {
				SetCameraDefaultDirection(_V(0.0,-1.0, 0.0));
			} else {
				SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
			}
			oapiCameraSetCockpitDir(0,0);
		}

		v.x += ViewOffsetx;
		v.y += ViewOffsety;
		v.z += ViewOffsetz;
	}
	SetCameraOffset(v);

	VCCameraOffset.x = v.x - VCMeshOffset.x;
	VCCameraOffset.y = v.y - VCMeshOffset.y;
	VCCameraOffset.z = v.z - VCMeshOffset.z;

	if (InVC) {
		RegisterVC();
	}

	//
	// FOV handling
	//

	if (FovExternal == 0) {
		if (fov == -1) {
			if (FovFixed) {
				papiCameraSetAperture(FovSave);
				if (!GenericFirstTimestep) {
					FovFixed = false;
					FovExternal = 0;
				}
			}
		} else {
			if (!FovFixed && !GenericFirstTimestep) {
				FovSave = papiCameraAperture();
				FovFixed = true;
			}
			papiCameraSetAperture(fov);
		}
	}
}


