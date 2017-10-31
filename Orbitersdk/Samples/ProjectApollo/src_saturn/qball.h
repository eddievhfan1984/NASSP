/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Q-Ball (Header)

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

class QBall
{
public:
	QBall();
	void Init(Saturn *vessel);
	double GetAOA();
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void SetPowerOn() { IsPowered = true; }
	void SetPowerOff() { IsPowered = false; }
protected:
	bool IsPowered;

	Saturn *sat;
};

#define QBALL_START_STRING		"QBALL_BEGIN"
#define QBALL_END_STRING		"QBALL_END"