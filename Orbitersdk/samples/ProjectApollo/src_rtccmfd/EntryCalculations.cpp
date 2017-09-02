#include "EntryCalculations.h"

namespace EntryCalculations
{
	void augekugel(double ve, double gammae, double &phie, double &Te)
	{
		double vefps, gammaedeg, K1, K2;

		vefps = ve / 0.3048;
		gammaedeg = gammae * DEG;
		if (vefps <= 21000)
		{
			K1 = 5500.0;
		}
		else
		{
			if (vefps <= 28000.0)
			{
				K1 = 2400.0 + 0.443*(28000.0 - vefps);
			}
			else
			{
				K1 = 2400.0;
			}
		}
		if (vefps <= 24000.0)
		{
			K2 = -3.2 - 0.001222*(24000.0 - vefps);
		}
		else
		{
			if (vefps <= 28400.0)
			{
				K2 = 1.0 - 0.00105*(28000.0 - vefps);
			}
			else
			{
				K2 = 2.4 + 0.000285*(vefps - 33625.0);
			}
		}
		phie = min(2000.0, K1 / (abs(abs(gammaedeg) - K2)));
		if (vefps < 26000.0)
		{
			Te = 8660.0 * phie / vefps;
		}
		else
		{
			Te = phie / 3.0;
		}
	}

	void landingsite(VECTOR3 REI, VECTOR3 VEI, double MJD_EI, double &lambda, double &phi)
	{
		double t32, v3, S_FPA, gammaE, phie, te, Sphie, Cphie, tLSMJD, l, m, n, mu;
		VECTOR3 R3, V3, UR3, U_H3, U_LS, LSEF;
		MATRIX3 R;
		OBJHANDLE hEarth;

		hEarth = oapiGetObjectByName("Earth");
		mu = GGRAV*oapiGetMass(hEarth);

		t32 = OrbMech::time_radius(REI, VEI, length(REI) - 30480.0, -1, mu);
		OrbMech::rv_from_r0v0(REI, VEI, t32, R3, V3, mu);
		UR3 = unit(R3);
		v3 = length(V3);
		S_FPA = dotp(UR3, V3) / v3;
		gammaE = asin(S_FPA);
		augekugel(v3, gammaE, phie, te);


		tLSMJD = MJD_EI + (t32 + te) / 24.0 / 3600.0;
		Sphie = sin(0.00029088821*phie);
		Cphie = cos(0.00029088821*phie);
		U_H3 = unit(crossp(crossp(R3, V3), R3));
		U_LS = UR3*Cphie + U_H3*Sphie;
		U_LS = _V(U_LS.x, U_LS.z, U_LS.y);
		R = OrbMech::GetRotationMatrix(hEarth, tLSMJD);
		LSEF = tmul(R, U_LS);
		l = LSEF.x;
		m = LSEF.z;
		n = LSEF.y;
		phi = asin(n);
		if (m > 0)
		{
			lambda = acos(l / cos(phi));
		}
		else
		{
			lambda = PI2 - acos(l / cos(phi));
		}
		if (lambda > PI) { lambda -= PI2; }
	}

	void Reentry(VECTOR3 REI, VECTOR3 VEI, double mjd0, bool highspeed, double &EntryLatPred, double &EntryLngPred, double &EntryRTGO, double &EntryVIO, double &EntryRET)
	{
		double t32, v3, S_FPA, gammaE, phie, te, t_LS, Sphie, Cphie, tLSMJD, l, m, n, phi, lambda2, EntryInterface, RCON, mu, EMSAlt;
		VECTOR3 R3, V3, UR3, U_H3, U_LS, LSEF;
		MATRIX3 R;
		OBJHANDLE hEarth;

		EntryInterface = 400000.0 * 0.3048;

		hEarth = oapiGetObjectByName("Earth");

		RCON = oapiGetSize(hEarth) + EntryInterface;
		mu = GGRAV*oapiGetMass(hEarth);

		if (highspeed)
		{
			EMSAlt = 297431.0*0.3048;
		}
		else
		{
			EMSAlt = 284643.0*0.3048;
		}

		t32 = OrbMech::time_radius(REI, VEI, RCON - 30480.0, -1, mu);
		OrbMech::rv_from_r0v0(REI, VEI, t32, R3, V3, mu);
		UR3 = unit(R3);
		v3 = length(V3);
		S_FPA = dotp(UR3, V3) / v3;
		gammaE = asin(S_FPA);
		augekugel(v3, gammaE, phie, te);

		t_LS = t32 + te;
		Sphie = sin(0.00029088821*phie);
		Cphie = cos(0.00029088821*phie);
		U_H3 = unit(crossp(crossp(R3, V3), R3));
		U_LS = UR3*Cphie + U_H3*Sphie;

		tLSMJD = mjd0 + t_LS / 24.0 / 3600.0;
		//U_LS = tmul(Rot, U_LS);
		U_LS = _V(U_LS.x, U_LS.z, U_LS.y);
		R = OrbMech::GetRotationMatrix(hEarth, tLSMJD);
		LSEF = tmul(R, U_LS);
		l = LSEF.x;
		m = LSEF.z;
		n = LSEF.y;
		phi = asin(n);
		if (m > 0)
		{
			lambda2 = acos(l / cos(phi));
		}
		else
		{
			lambda2 = PI2 - acos(l / cos(phi));
		}
		if (lambda2 > PI) { lambda2 -= PI2; }

		EntryLatPred = phi;
		EntryLngPred = lambda2;

		VECTOR3 R05G, V05G;
		double dt22 = OrbMech::time_radius(R3, V3, length(R3) - (300000.0 * 0.3048 - EMSAlt), -1, mu);
		OrbMech::rv_from_r0v0(R3, V3, dt22, R05G, V05G, mu);

		EntryRTGO = phie - 3437.7468*acos(dotp(unit(R3), unit(R05G)));
		EntryVIO = length(V05G);
		EntryRET = t32 + dt22;
	}
	
	VECTOR3 ThreeBodyAbort(double t_I, double t_EI, VECTOR3 R_I, VECTOR3 V_I, double mu_E, double mu_M, bool INRFVsign, VECTOR3 &R_EI, VECTOR3 &V_EI)
	{
		VECTOR3 R_I_star, delta_I_star, delta_I_star_dot, R_I_sstar, V_I_sstar, V_I_star, R_S, R_I_star_apo, R_E_apo, V_E_apo, V_I_apo;
		VECTOR3 dV_I_sstar, R_m, V_m;
		double t_S, tol, dt_S, r_s, EntryInterface, RCON;
		OBJHANDLE hEarth, hMoon;
		CELBODY *cMoon;
		double *MoonPos;

		hEarth = oapiGetObjectByName("Earth");
		hMoon = oapiGetObjectByName("Moon");
		cMoon = oapiGetCelbodyInterface(hMoon);
		
		r_s = 24.0*oapiGetSize(hEarth);//64373760.0;//14.0*oapiGetSize(hEarth);
		EntryInterface = 400000.0 * 0.3048;
		RCON = oapiGetSize(hEarth) + EntryInterface;
		tol = 20.0;

		MoonPos = new double[12];
		cMoon->clbkEphemeris(t_I, EPHEM_TRUEPOS | EPHEM_TRUEVEL, MoonPos);

		R_m = _V(MoonPos[0], MoonPos[2], MoonPos[1]);
		V_m = _V(MoonPos[3], MoonPos[5], MoonPos[4]);

		R_I_star = delta_I_star = delta_I_star_dot = _V(0.0, 0.0, 0.0);
		V_I_star = V_I;

		do
		{
			do
			{
				R_I_sstar = R_m + R_I_star + delta_I_star;
				V_I_sstar = V_m + V_I_star + delta_I_star_dot;
				Abort(R_I_sstar, V_I_sstar, RCON, (t_EI - t_I)*24.0*3600.0, mu_E, dV_I_sstar, R_EI, V_EI);
				V_I_sstar = V_I_sstar + dV_I_sstar;
				V_I_star = V_I_sstar - V_m - delta_I_star_dot;
				OrbMech::INRFV(R_I, V_I_star, r_s, INRFVsign, mu_M, V_I_apo, R_S, dt_S);
				t_S = t_I + dt_S / 24.0 / 3600.0;
				R_I_star_apo = R_I_star;
				R_I_star = R_S + V_I_star*(t_I - t_S) * 24.0 * 3600.0;

			} while (length(R_I_star - R_I_star_apo) > tol);

			OrbMech::oneclickcoast(R_I, V_I_apo, t_I, (t_EI - t_I) * 24.0 * 3600.0, R_E_apo, V_E_apo, hMoon, hEarth);
			OrbMech::rv_from_r0v0(R_E_apo, V_E_apo, (t_I - t_EI) * 24.0 * 3600.0, R_I_sstar, V_I_sstar, mu_E);
			delta_I_star = R_I_sstar - R_m - R_I_star;
			delta_I_star_dot = V_I_sstar - V_m - V_I_star;

		} while (length(R_EI - R_E_apo) > tol);

		return V_I_apo;
	}

	void Abort(VECTOR3 R0, VECTOR3 V0, double RCON, double dt, double mu, VECTOR3 &DV, VECTOR3 &R_EI, VECTOR3 &V_EI)
	{
		double k4, D1, D2, D3, D4, x2, v2, x2_apo, x2_err;
		VECTOR3 V2;

		k4 = -0.10453;
		D1 = 1.6595;
		D2 = -4.8760771e-4;
		D3 = 4.5419476e-8;
		D4 = -1.4317675e-12;

		x2 = k4;
		x2_err = 1.0;

		while (abs(x2_err) > 0.00001)
		{
			time_reentry(R0, V0, RCON, x2, dt, mu, V2, R_EI, V_EI);

			v2 = length(V_EI);
			x2_apo = x2;
			x2 = D1 + D2*v2 + D3*v2*v2 + D4*v2*v2*v2;
			x2_err = x2 - x2_apo;
		}
		DV = V2 - V0;
	}

	void time_reentry(VECTOR3 R0, VECTOR3 V0, double r1, double x2, double dt, double mu, VECTOR3 &V, VECTOR3 &R_EI, VECTOR3 &V_EI)
	{
		VECTOR3 U_R1, U_V1, eta, U_H;
		double r0, lambda, MA1, MA2, beta1, beta5, theta1, theta2, theta3, beta10, xmin, xmax, dxmax, C0, C1, C2, C3, x, dx;
		double x_old, dt_err, p, dt_act, dt_err_old, C_FPA;
		int i;

		MA1 = -6.986643e7;
		C0 = 1.81000432e8;
		C1 = 1.5078514;
		C2 = -6.49993054e-9;
		C3 = 9.769389245e-18;

		U_R1 = unit(R0);
		U_V1 = unit(V0);
		r0 = length(R0);
		C_FPA = dotp(U_R1, U_V1);
		if (abs(C_FPA) < 0.99966)
		{
			eta = crossp(R0, V0);
		}
		else
		{
			eta = _V(0.0, 0.0, 1.0);
		}

		if (eta.z < 0)
		{
			eta = -eta;
		}
		U_H = unit(crossp(eta, R0));

		MA2 = C0 + C1*r0 + C2*r0*r0 + C3*r0*r0*r0;

		lambda = r0 / r1;
		beta1 = 1.0 + x2*x2;
		beta5 = lambda*beta1;
		theta1 = beta5*lambda - 1.0;
		theta2 = 2.0*r0*(lambda - 1.0);
		theta3 = sqrt(mu) / r0;
		beta10 = beta5*(MA1 - r0) / (MA1 - r1) - 1.0;
		if (beta10 < 0.0)
		{
			xmin = 0.0;
		}
		else
		{
			xmin = -sqrt(beta10);
		}
		dxmax = -xmin / 16.0;
		beta10 = beta5*(MA2 - r0) / (MA2 - r1) - 1.0;
		if (beta10 < 0.0)
		{
			xmax = 0.0;
		}
		else
		{
			xmax = sqrt(beta10);
		}


		x = xmin;
		dx = dxmax;
		dt_err = 10.0;
		i = 0;
		while (abs(dt_err)>0.005)
		{
			p = theta2 / (theta1 - x*x);
			V = (U_R1*x + U_H)*theta3*sqrt(p);
			dt_act = OrbMech::time_radius(R0, V, r1, -1.0, mu);

			dt_err = dt - dt_act;

			if (i == 0)
			{
				dx = -dxmax;
			}
			else
			{
				dx = (x - x_old) / (dt_err - dt_err_old)*dt_err;
				if (abs(dx) > dxmax)
				{
					dx = OrbMech::sign(dx)*dxmax;
				}
			}

			dt_err_old = dt_err;
			x_old = x;
			x -= dx;

			i++;
		}
		OrbMech::rv_from_r0v0(R0, V, dt, R_EI, V_EI, mu);
	}

	double landingzonelong(int zone, double lat)
	{
		if (zone == 0)
		{
			return MPL(lat);
		}
		else if (zone == 1)
		{
			return EPL(lat);
		}
		else if (zone == 2)
		{
			return AOL(lat);
		}
		else if (zone == 3)
		{
			return IOL(lat);
		}
		else
		{
			return WPL(lat);
		}
	}

	double MPL(double lat) //Calculate the splashdown longitude from the latitude for the Mid-Pacific landing area
	{
		return -165.0*RAD;
	}

	double EPL(double lat) //Calculate the splashdown longitude from the latitude for the East-Pacific landing area
	{
		if (lat > 21.0*RAD)
		{
			return (-135.0*RAD + 122.0*RAD) / (40.0*RAD - 21.0*RAD)*(lat - 21.0*RAD) - 122.0*RAD;
		}
		else if (lat > -11.0*RAD)
		{
			return (-122.0*RAD + 89.0*RAD) / (21.0*RAD + 11.0*RAD)*(lat + 11.0*RAD) - 89.0*RAD;
		}
		else
		{
			return (-89.0*RAD + 83.0*RAD) / (-11.0*RAD + 40.0*RAD)*(lat + 40.0*RAD) - 83.0*RAD;
		}
	}

	double AOL(double lat)
	{
		if (lat > 10.0*RAD)
		{
			return -30.0*RAD;
		}
		else if (lat > -5.0*RAD)
		{
			return (-30.0*RAD + 25.0*RAD) / (10.0*RAD + 5.0*RAD)*(lat + 5.0*RAD) - 25.0*RAD;
		}
		else
		{
			return -25.0*RAD;
		}
	}

	double IOL(double lat)
	{
		return 65.0*RAD;
	}

	double WPL(double lat)
	{
		if (lat > 10.0*RAD)
		{
			return 150.0*RAD;
		}
		else if (lat > -15.0*RAD)
		{
			return (150.0*RAD - 170.0*RAD) / (10.0*RAD + 15.0*RAD)*(lat + 15.0*RAD) + 170.0*RAD;
		}
		else
		{
			return 170.0*RAD;
		}
	}
}

EarthEntry::EarthEntry(VECTOR3 R0B, VECTOR3 V0B, double mjd, OBJHANDLE gravref, double GETbase, double EntryTIG, double EntryAng, double EntryLng, bool entrynominal, bool entrylongmanual)
{
	MA1 = -6.986643e7;//8e8;
	C0 = 1.81000432e8;
	C1 = 1.5078514;
	C2 = -6.49993054e-9;
	C3 = 9.769389245e-18;
	D2 = -4.8760771e-4;
	D3 = 4.5419476e-8;
	D4 = -1.4317675e-12;
	k1 = 7.0e6;
	k2 = 6.495e6;
	k3 = -0.06105;//-0.043661;
	k4 = -0.10453;

	this->entrylongmanual = entrylongmanual;

	this->GETbase = GETbase;
	this->EntryAng = EntryAng;

	if (entrylongmanual)
	{
		this->EntryLng = EntryLng;
	}
	else
	{
		landingzone = (int)EntryLng;
		this->EntryLng = EntryCalculations::landingzonelong(landingzone, 0);
	}

	this->gravref = gravref;

	this->R0B = R0B;
	this->V0B = V0B;
	this->mjd = mjd;
	get = (mjd - GETbase)*24.0*3600.0;

	EntryInterface = 400000.0 * 0.3048;

	hEarth = oapiGetObjectByName("Earth");

	RCON = oapiGetSize(hEarth) + EntryInterface;
	RD = RCON;
	mu = GGRAV*oapiGetMass(hEarth);

	EntryTIGcor = EntryTIG;

	Tguess = PI2 / sqrt(mu)*OrbMech::power(length(R0B), 1.5);

	tigslip = 100.0;
	ii = 0;

	entryphase = 0;

	dt0 = EntryTIGcor - get;

	OrbMech::oneclickcoast(R0B, V0B, mjd, dt0, R11B, V11B, gravref, hEarth);

	x2 = OrbMech::cot(PI05 - EntryAng);
	if (length(R11B) > k1)
	{
		D1 = 1.6595;
	}
	else
	{
		D1 = 1.6865;//1.69107;//1.6865;
	}

	EMSAlt = 284643.0*0.3048;

	R_E = oapiGetSize(hEarth);
	earthorbitangle = (-31.7 - 2.15)*RAD;

	this->entrynominal = entrynominal;

	precision = 1;
	errorstate = 0;
}

void EarthEntry::xdviterator3(VECTOR3 R1B, VECTOR3 V1B, double xmin, double xmax, double &x)
{
	double xdes, fac1, fac2, sing, cosg, R0, R, VT0, VR0, h0, x_apo, p, dv;
	int i;
	VECTOR3 N, V;

	x_apo = 100000;
	i = 0;
	dv = 0.0;

	xdes = tan(earthorbitangle - acos(R_E / length(R1B)));
	fac1 = 1.0 / sqrt(xdes*xdes + 1.0);
	fac2 = -xdes / sqrt(xdes*xdes + 1.0);
	h0 = length(crossp(R1B, V1B));

	N = crossp(unit(R1B), unit(V1B));
	sing = length(N);
	cosg = dotp(unit(R1B), unit(V1B));
	x = cosg / sing;
	R0 = length(R1B);
	R = R0 / RCON;
	VT0 = h0 / R0;
	VR0 = dotp(R1B, V1B) / R0;
	while (abs(x_apo - x) > OrbMech::power(2.0, -20.0) && i <= 100)
	{
		p = 2.0*R0*(R - 1.0) / (R*R*(1.0 + x2*x2) - (1.0 + x*x));
		V = (unit(R1B)*x + unit(crossp(crossp(R1B, V1B), R1B)))*sqrt(mu*p) / R0;
		if (dv - length(V - V1B)>100.0)
		{
			dv -= 100.0;
		}
		else if (dv - length(V - V1B) < -100.0)
		{
			dv += 100.0;
		}
		else
		{
			dv = length(V - V1B);
		}
		x_apo = x;
		x = (VR0 - dv*fac2) / (VT0 - dv*fac1);
		if (x > xmax)
		{
			x = xmax;
		}
		else if (x < xmin)
		{
			x = xmin;
		}
		i++;
	}
}

void EarthEntry::xdviterator2(int f1, VECTOR3 R1B, VECTOR3 V1B, double theta1, double theta2, double theta3, VECTOR3 U_R1, VECTOR3 U_H, double dx, double xmin, double xmax, double &x)
{
	double xdes, xact, x_err, x_apo, x_err_apo, epsilon, p_CON, beta9;
	VECTOR3 Entry_DV, DV, V2;
	int i;
	i = 0;
	epsilon = pow(2.0, -20.0);

	xdes = tan(earthorbitangle - acos(R_E / length(R1B)));

	x_err_apo = 1000;

	while (abs(dx) > epsilon && i < 135)
	{
		dvcalc(V1B, theta1, theta2, theta3, x, U_R1, U_H, V2, DV, p_CON);

		Entry_DV = _V(dotp(DV, U_H), 0.0, -dotp(DV, U_R1));

		xact = Entry_DV.z / Entry_DV.x;
		x_err = xdes - xact;

		//if (i == 0)
		//{
		//	dx = -0.01*(xdes - xact);
		//}
		//else
		//{
		//dx = x_err*((x_apo - x) / (x_err - x_err_apo));
		//}
		if (abs(x_err) > abs(x_err_apo))
		{
			dx = -dx*0.5;
		}
		beta9 = x + 1.1*dx;
		if (beta9 > xmax)
		{
			dx = (xmax - x) / 2.0;
		}
		else if (beta9 < xmin)
		{
			dx = (xmin - x) / 2.0;
		}
		x_err_apo = x_err;
		x_apo = x;
		x += dx;
		i++;
	}
}

void EarthEntry::xdviterator(VECTOR3 R1B, VECTOR3 V1B, double theta1, double theta2, double theta3, VECTOR3 U_R1, VECTOR3 U_H, double dx, double xmin, double xmax, double &x)
{
	int i;
	double epsilon, p_CON, dvapo, beta8, beta9;
	VECTOR3 DV, V2;

	i = 0;
	epsilon = OrbMech::power(2.0, -20.0);

	dvcalc(V1B, theta1, theta2, theta3, x, U_R1, U_H, V2, DV, p_CON);

	beta8 = xmax - xmin;
	if (beta8 <= dxmax)
	{
		dx = 0.5*beta8*OrbMech::sign(dx);
	}

	while (abs(dx) > epsilon && i < 135)
	{
		dvapo = length(DV);
		xapo = x;
		x += dx;
		dvcalc(V1B, theta1, theta2, theta3, x, U_R1, U_H, V2, DV, p_CON);
		if (length(DV) > dvapo)
		{
			dx = -dx*0.5;
		}
		beta9 = x + 1.1*dx;
		if (beta9 > xmax)
		{
			dx = (xmax - x) / 2.0;
		}
		else if (beta9 < xmin)
		{
			dx = (xmin - x) / 2.0;
		}
		i++;
	}
}

void EarthEntry::dvcalc(VECTOR3 V1B, double theta1, double theta2, double theta3, double x, VECTOR3 U_R1, VECTOR3 U_H, VECTOR3 &V2, VECTOR3 &DV, double &p_CON)
{
	p_CON = theta2 / (theta1 - x*x);
	V2 = (U_R1*x + U_H)*theta3*sqrt(p_CON);
	DV = V2 - V1B;
}

void EarthEntry::reentryconstraints(int n1, VECTOR3 R1B, VECTOR3 REI, VECTOR3 VEI)
{
	if (n1 == 0)
	{
		if (EntryAng == 0)
		{
			if (length(R1B) > k1)
			{
				x2 = k4;
			}
			else
			{
				x2 = k3;
			}
		}
		//n1 = 1;
	}
	else
	{
		if (EntryAng == 0)
		{
			double v2;
			v2 = length(VEI);
			x2 = D1 + D2*v2 + D3*v2*v2 + D4*v2*v2*v2;
		}
		else
		{
			x2 = x2;
		}
	}
}

void EarthEntry::coniciter(VECTOR3 R1B, VECTOR3 V1B, double t1, double &theta_long, double &theta_lat, VECTOR3 &V2, double &x, double &dx, double &t21)
{
	VECTOR3 U_R1, U_H, REI, VEI;
	double MA2, x2_err, C_FPA;
	int n1;

	x2_err = 1.0;
	precomputations(1, R1B, V1B, U_R1, U_H, MA2, C_FPA);
	n1 = 1;
	while (abs(x2_err) > 0.00001 && n1 <= 10)
	{
		conicreturn(0, R1B, V1B, MA2, C_FPA, U_R1, U_H, V2, x, n1);
		t21 = OrbMech::time_radius(R1B, V2, RCON, -1, mu);
		OrbMech::rv_from_r0v0(R1B, V2, t21, REI, VEI, mu);
		x2_apo = x2;
		reentryconstraints(n1, R1B, REI, VEI);
		x2_err = x2_apo - x2;
		n1++;
	}
	t2 = t1 + t21;
	OrbMech::rv_from_r0v0(R1B, V2, t21, REI, VEI, mu);
	EntryCalculations::landingsite(REI, VEI, GETbase + t2 / 24.0 / 3600.0, theta_long, theta_lat);
}

void EarthEntry::precisioniter(VECTOR3 R1B, VECTOR3 V1B, double t1, double &t21, double &x, double &theta_long, double &theta_lat, VECTOR3 &V2)
{
	double RD, R_ERR, dRCON, rPRE_apo, r1b, lambda, beta1, beta5, theta1, theta2, p_CON, C_FPA, MA2, x2_err;
	VECTOR3 U_R1, U_V1, RPRE, VPRE, U_H, eta;
	int n1, n2;

	n1 = 0;
	n2 = 0;
	RCON = oapiGetSize(hEarth) + EntryInterface;
	RD = RCON;
	R_ERR = 1000.0;
	x2_err = 1.0;

	U_R1 = unit(R1B);
	U_V1 = unit(V1B);
	C_FPA = dotp(U_R1, U_V1);
	if (abs(C_FPA) < 0.99966)
	{
		eta = crossp(R1B, V1B);
	}
	else
	{
		eta = _V(0.0, 0.0, 1.0);
	}
	if (eta.z < 0)
	{
		eta = -eta;
	}
	U_H = unit(crossp(eta, R1B));
	r1b = length(R1B);
	MA2 = C0 + C1*r1b + C2*r1b*r1b + C3*r1b*r1b*r1b;

	lambda = r1b / RCON;
	beta1 = 1.0 + x2*x2;
	beta5 = lambda*beta1;
	theta1 = beta5*lambda - 1.0;
	theta2 = 2.0*r1b*(lambda - 1.0);
	p_CON = theta2 / (theta1 - x*x);

	if (RCON - p_CON*beta1 >= 0)
	{
		phi2 = 1.0;
	}
	else
	{
		phi2 = -1.0;
	}

	finalstatevector(R1B, V2, beta1, t21, RPRE, VPRE);
	//reentryconstraints(n1 + 1, R1B, VPRE);
	//x2 = x2_apo;
	//beta1 = 1.0 + x2*x2;
	R_ERR = length(RPRE) - RD;
	while (abs(x2_err) > 0.00001 && n2 <= 10)
	{
		n1 = 0;
		while (n1 == 0 || (abs(R_ERR) > 100.0 && n1 <= 20))
		{
			//finalstatevector(R1B, V2, beta1, t21, RPRE, VPRE);
			//R_ERR = length(RPRE) - RD;
			newrcon(n1, RD, length(RPRE), R_ERR, dRCON, rPRE_apo);
			conicreturn(1, R1B, V1B, MA2, C_FPA, U_R1, U_H, V2, x, n1);
			finalstatevector(R1B, V2, beta1, t21, RPRE, VPRE);
			R_ERR = length(RPRE) - RD;
			n1++;
		}
		x2_apo = x2;
		x2_err = x2_apo - x2;
		beta1 = 1.0 + x2*x2;
		n2++;
	}
	t2 = t1 + t21;
	EntryCalculations::landingsite(RPRE, VPRE, GETbase + t2 / 24.0 / 3600.0, theta_long, theta_lat);
	if (n1 == 21)
	{
		errorstate = 1;
	}
	if (n2 == 11)
	{
		errorstate = 2;
	}
}

void EarthEntry::precisionperi(VECTOR3 R1B, VECTOR3 V1B, double t1, double &t21, double &x, double &theta_long, double &theta_lat, VECTOR3 &V2)
{
	double R0, R1, R, x_apo, p, dv, h0, sing, cosg, VT0, VR0, xdes, fac1, fac2;
	VECTOR3 N, RPRE, VPRE;
	int i;

	x_apo = 100000;
	i = 0;

	xdes = tan(earthorbitangle - acos(R_E / length(R1B)));
	fac1 = 1.0 / sqrt(xdes*xdes + 1.0);
	fac2 = -xdes / sqrt(xdes*xdes + 1.0);
	h0 = length(crossp(R1B, V1B));
	N = crossp(unit(R1B), unit(V1B));
	sing = length(N);
	cosg = dotp(unit(R1B), unit(V1B));
	x = cosg / sing;
	R0 = length(R1B);
	R1 = R_E - 30.0*1852.0;
	R = R0 / R1;
	VT0 = h0 / R0;
	VR0 = dotp(R1B, V1B) / R0;
	while (abs(x_apo - x) > OrbMech::power(2.0, -20.0))
	{
		p = 2.0*R0*(R - 1.0) / (R*R - (1.0 + x*x));
		V2 = (unit(R1B)*x + unit(crossp(crossp(R1B, V1B), R1B)))*sqrt(mu*p) / R0;
		dv = length(V2 - V1B);
		x_apo = x;
		x = (VR0 - dv*fac2) / (VT0 - dv*fac1);
		i++;
	}
	t21 = OrbMech::time_radius_integ(R1B, V2, mjd + (dt0 + dt1) / 24.0 / 3600.0, RD, -1, hEarth, hEarth, RPRE, VPRE);
	N = crossp(unit(RPRE), unit(VPRE));
	sing = length(N);
	cosg = dotp(unit(RPRE), unit(VPRE));
	x2 = cosg / sing;
	t2 = t1 + t21;
	EntryCalculations::landingsite(RPRE, VPRE, GETbase + t2 / 24.0 / 3600.0, theta_long, theta_lat);
}

void EarthEntry::newrcon(int n1, double RD, double rPRE, double R_ERR, double &dRCON, double &rPRE_apo)
{
	double S;

	if (n1 == 0)
	{
		this->RCON = RD*RD / rPRE;
		dRCON = RCON - RD;
	}
	else
	{
		S = dRCON / (rPRE_apo - rPRE);
		if (abs(S + 2.0) > 2.0)
		{
			S = -4.0;
		}
		dRCON = S*R_ERR;
		this->RCON = RCON + dRCON;
		if (RCON < 1e6)
		{
			RCON = 1e6;
		}
	}
	rPRE_apo = rPRE;
}

void EarthEntry::finalstatevector(VECTOR3 R1B, VECTOR3 V2, double beta1, double &t21, VECTOR3 &RPRE, VECTOR3 &VPRE)
{
	VECTOR3 N;
	double beta12, x2PRE, c3, alpha_N, sing, cosg, p_N, beta2, beta3, beta4, RF, phi4, dt21, beta13, dt21apo, beta14;

	OrbMech::oneclickcoast(R1B, V2, mjd + (dt0 + dt1) / 24.0 / 3600.0, t21, RPRE, VPRE, hEarth, hEarth);

	beta12 = 100.0;
	x2PRE = 1000000;
	dt21apo = 100000000.0;

	while (abs(beta12) > 0.000007 && abs(x2 - x2PRE) > 0.00001)
	{
		c3 = length(RPRE)*pow(length(VPRE), 2.0) / mu;
		alpha_N = 2.0 - c3;
		N = crossp(unit(RPRE), unit(VPRE));
		sing = length(N);
		cosg = dotp(unit(RPRE), unit(VPRE));
		x2PRE = cosg / sing;
		p_N = c3*sing*sing;
		beta2 = p_N*beta1;
		beta3 = 1.0 - alpha_N*beta2;
		if (beta3 < 0)
		{
			beta4 = 1.0 / alpha_N;
		}
		else
		{
			beta4 = beta2 / (1.0 - phi2*sqrt(beta3));
		}
		beta12 = beta4 - 1.0;
		//if (abs(beta12) > 0.000007)
		//{
		RF = beta4*length(RPRE);
		if (beta12 > 0)
		{
			phi4 = -1.0;
		}
		else if (x2PRE > 0)
		{
			phi4 = -1.0;
		}
		else
		{
			phi4 = 1.0;
		}
		dt21 = OrbMech::time_radius(RPRE, VPRE*phi4, RF, -phi4, mu);
		dt21 = phi4*dt21;
		beta13 = dt21 / dt21apo;
		if (beta13 > 0)
		{
			beta14 = 1.0;
		}
		else
		{
			beta14 = -0.6;
		}
		if (beta13 / beta14 > 1.0)
		{
			dt21 = beta14*dt21apo;
		}
		dt21apo = dt21;
		OrbMech::oneclickcoast(RPRE, VPRE, mjd + (dt0 + dt1 + t21) / 24.0 / 3600.0, dt21, RPRE, VPRE, hEarth, hEarth);
		t21 += dt21;
		//}
	}
}

void EarthEntry::conicreturn(int f1, VECTOR3 R1B, VECTOR3 V1B, double MA2, double C_FPA, VECTOR3 U_R1, VECTOR3 U_H, VECTOR3 &V2, double &x, int &n1)
{
	double theta1, theta2, theta3, xmin, xmax, p_CON, beta6, dx;
	VECTOR3 DV;
	conicinit(R1B, MA2, xmin, xmax, theta1, theta2, theta3);
	if (f1 == 0)
	{
		if (C_FPA >= 0)
		{
			x = xmax;
			dx = -dxmax;
		}
		else
		{
			x = xmin;
			dx = dxmax;
		}

		if (ii == 0)
		{
			if (entrynominal)
			{
				xdviterator3(R1B, V1B, xmin, xmax, x);
			}
			else
			{
				xdviterator(R1B, V1B, theta1, theta2, theta3, U_R1, U_H, dx, xmin, xmax, x);
			}
		}
		else
		{
			if (entrynominal)
			{
				xdviterator3(R1B, V1B, xmin, xmax, x);
			}
			else
			{
				xdviterator(R1B, V1B, theta1, theta2, theta3, U_R1, U_H, dx, xmin, xmax, x);
			}
		}
	}
	else
	{
		double r1b, lambda, beta1, beta5;
		r1b = length(R1B);
		lambda = r1b / RCON;
		beta1 = 1.0 + x2*x2;
		beta5 = lambda*beta1;
		beta6 = beta5*(2.0 - lambda) - 1.0;
		if (beta6 > 0)
		{
			xlim = sqrt(beta6);
			if (phi2 == 1)
			{
				xmax = xlim;
				xmin = -xlim;
				if (x > 0)
				{
					x = xmax;
					dx = -dxmax;
				}
				else
				{
					x = xmin;
					dx = dxmax;
				}
			}
			else
			{
				if (x > 0)
				{
					xmin = xlim;
					x = xmax;
					dx = -dxmax;
				}
				else
				{
					xmax = -xlim;
					x = xmin;
					dx = dxmax;
				}
			}
		}
		else
		{
			if (phi2 == 1)
			{
				phi2 = -1.0;
				n1 = -1;
			}
			if (x > 0)
			{
				x = xmax;
				dx = -dxmax;
			}
			else
			{
				x = xmin;
				dx = dxmax;
			}
		}

		if (entrynominal)
		{
			xdviterator(R1B, V1B, theta1, theta2, theta3, U_R1, U_H, dx, xmin, xmax, x);
			xdviterator2(f1, R1B, V1B, theta1, theta2, theta3, U_R1, U_H, dx, xmin, xmax, x);
		}
		else
		{
			xdviterator(R1B, V1B, theta1, theta2, theta3, U_R1, U_H, dx, xmin, xmax, x);
		}
	}
	dvcalc(V1B, theta1, theta2, theta3, x, U_R1, U_H, V2, DV, p_CON);
}

void EarthEntry::conicinit(VECTOR3 R1B, double MA2, double &xmin, double &xmax, double &theta1, double &theta2, double &theta3)
{
	double r1b, lambda, beta1, beta5, beta10;
	r1b = length(R1B);

	lambda = r1b / RCON;
	beta1 = 1.0 + x2*x2;
	beta5 = lambda*beta1;
	theta1 = beta5*lambda - 1.0;
	theta2 = 2.0*r1b*(lambda - 1.0);
	theta3 = sqrt(mu) / r1b;
	beta10 = beta5*(MA1 - r1b) / (MA1 - RCON) - 1.0;
	if (beta10 < 0.0)
	{
		xmin = 0.0;
	}
	else
	{
		xmin = -sqrt(beta10);
	}
	dxmax = -xmin / 16.0;
	beta10 = beta5*(MA2 - r1b) / (MA2 - RCON) - 1.0;
	if (beta10 < 0.0)
	{
		xmax = 0.0;
	}
	else
	{
		xmax = sqrt(beta10);
	}
}

void EarthEntry::precomputations(bool x2set, VECTOR3 R1B, VECTOR3 V1B, VECTOR3 &U_R1, VECTOR3 &U_H, double &MA2, double &C_FPA)
{
	VECTOR3 U_V1, eta;
	double r1b;

	U_R1 = unit(R1B);
	U_V1 = unit(V1B);
	C_FPA = dotp(U_R1, U_V1);
	if (abs(C_FPA) < 0.99966)
	{
		eta = crossp(R1B, V1B);
	}
	else
	{
		eta = _V(0.0, 0.0, 1.0);
	}
	if (eta.z < 0)
	{
		eta = -eta;
	}
	U_H = unit(crossp(eta, R1B));
	r1b = length(R1B);
	MA2 = C0 + C1*r1b + C2*r1b*r1b + C3*r1b*r1b*r1b;
	if (x2set)
	{
		reentryconstraints(0, R1B, _V(0, 0, 0), _V(0, 0, 0));
	}
}

bool EarthEntry::EntryIter()
{
	double theta_long, theta_lat, dlng;
	VECTOR3 R1B, V1B, V2;

	errorstate = 0;

	dt1 = EntryTIGcor - get - dt0;
	OrbMech::oneclickcoast(R11B, V11B, mjd + dt0 / 24.0 / 3600.0, dt1, R1B, V1B, hEarth, hEarth);
	if (precision == 2)
	{
		precisionperi(R1B, V1B, EntryTIGcor, t21, x, theta_long, theta_lat, V2);
	}
	else
	{
		coniciter(R1B, V1B, EntryTIGcor, theta_long, theta_lat, V2, x, dx, t21);

		if (entryphase == 1 && precision == 1)
		{
			precisioniter(R1B, V1B, EntryTIGcor, t21, x, theta_long, theta_lat, V2);
		}
	}

	if (!entrylongmanual)
	{
		EntryLng = EntryCalculations::landingzonelong(landingzone, theta_lat);
	}

	dlng = EntryLng - theta_long;
	if (abs(dlng) > PI)
	{
		dlng = dlng - OrbMech::sign(dlng)*PI2;
	}
	
	if (ii == 0)
	{
		tigslip = Tguess*dlng / PI2;
		dlng_old = dlng;
		EntryTIGcor_old = EntryTIGcor;
	}
	else
	{
		tigslip = -dlng*(EntryTIGcor - EntryTIGcor_old) / (dlng - dlng_old);
		EntryTIGcor_old = EntryTIGcor;
		dlng_old = dlng;
	}
	if (abs(tigslip) > 100.0)
	{
		tigslip = 100.0*OrbMech::sign(tigslip);
	}
	EntryTIGcor += tigslip;

	ii++;

	if (((abs(dlng) > 0.005*RAD && ii < 60) || entryphase == 0))
	{
		if (abs(tigslip) < 10.0)// && abs(dx)<0.1)
		{
			if (entryphase == 0)
			{
				entryphase = 1;
				ii = 0;
			}
		}
		return false;
	}
	else
	{
		if (precision && entrynominal)
		{
			if (abs(x - xlim) < OrbMech::power(2.0, -20.0) || abs(x + xlim) < OrbMech::power(2.0, -20.0) || ii == 60)
				//if (ii == 40)
			{
				ii = 0;
				precision = 2;
				return false;
			}
		}
		VECTOR3 R05G, V05G, REI, VEI, R3, V3, UR3, U_R1, U_H, DV;
		double t32, dt22, v3, S_FPA, gammaE, phie, te, MA2, C_FPA;

		precomputations(0, R1B, V1B, U_R1, U_H, MA2, C_FPA);

		t2 = EntryTIGcor + t21;
		OrbMech::time_radius_integ(R1B, V2, mjd + (dt0 + dt1) / 24.0 / 3600.0, RD, -1, hEarth, hEarth, REI, VEI);//Maneuver to Entry Interface (400k ft)

		t32 = OrbMech::time_radius(REI, VEI, length(REI) - 30480.0, -1, mu);
		OrbMech::rv_from_r0v0(REI, VEI, t32, R3, V3, mu); //Entry Interface to 300k ft

		dt22 = OrbMech::time_radius(R3, V3, length(R3) - (300000.0 * 0.3048 - EMSAlt), -1, mu);
		OrbMech::rv_from_r0v0(R3, V3, dt22, R05G, V05G, mu); //300k ft to 0.05g

		UR3 = unit(R3);
		v3 = length(V3);
		S_FPA = dotp(UR3, V3) / v3;
		gammaE = asin(S_FPA);
		EntryCalculations::augekugel(v3, gammaE, phie, te);

		VECTOR3 Rsph, Vsph;

		OrbMech::oneclickcoast(R1B, V1B, mjd + (dt0 + dt1) / 24.0 / 3600.0, 0.0, Rsph, Vsph, hEarth, hEarth);
		DV = V2 - V1B;
		VECTOR3 i, j, k;
		MATRIX3 Q_Xx;
		j = unit(crossp(Vsph, Rsph));
		k = unit(-Rsph);
		i = crossp(j, k);
		Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);

		Entry_DV = mul(Q_Xx, DV);

		EntryRTGO = phie - 3437.7468*acos(dotp(unit(R3), unit(R05G)));
		EntryVIO = length(V05G);
		EntryRET = t2 + t32 + dt22;
		EntryAng = atan(x2);//asin(dotp(unit(REI), VEI) / length(VEI));

		EntryLngcor = theta_long;
		EntryLatcor = theta_lat;

		if (errorstate != 0)
		{
			precision = 9;
		}

		return true;
	}
}

Entry::Entry(VECTOR3 R0B, VECTOR3 V0B, double mjd, OBJHANDLE gravref, double GETbase, double EntryTIG, double EntryAng, double EntryLng, int critical, bool entrylongmanual)
{
	MA1 = -6.986643e7;//8e8;
	C0 = 1.81000432e8;
	C1 = 1.5078514;
	C2 = -6.49993054e-9;
	C3 = 9.769389245e-18;
	D2 = -4.8760771e-4;
	D3 = 4.5419476e-8;
	D4 = -1.4317675e-12;
	k1 = 7.0e6;
	k2 = 6.495e6;
	k3 = -0.06105;//-0.043661;
	k4 = -0.10453;

	this->entrylongmanual = entrylongmanual;

	this->GETbase = GETbase;
	this->EntryAng = EntryAng;

	if (entrylongmanual)
	{
		this->EntryLng = EntryLng;
	}
	else
	{
		landingzone = (int)EntryLng;
		this->EntryLng = EntryCalculations::landingzonelong(landingzone, 0);
	}

	this->critical = critical;
	this->gravref = gravref;

	this->R0B = R0B;
	this->V0B = V0B;
	this->mjd = mjd;
	get = (mjd - GETbase)*24.0*3600.0;

	EntryInterface = 400000.0 * 0.3048;

	hEarth = oapiGetObjectByName("Earth");

	RCON = oapiGetSize(hEarth) + EntryInterface;
	RD = RCON;
	mu = GGRAV*oapiGetMass(hEarth);

	EntryTIGcor = EntryTIG;

	ii = 0;

	entryphase = 0;

	dt0 = EntryTIGcor - get;

	SOIplan = NULL;

	OrbMech::oneclickcoast(R0B, V0B, mjd, dt0, R11B, V11B, gravref, hEarth);

	x2 = OrbMech::cot(PI05 - EntryAng);
	if (length(R11B) > k1)
	{
		D1 = 1.6595;
	}
	else
	{
		D1 = 1.6865;//1.69107;//1.6865;
	}

	EMSAlt = 297431.0*0.3048;
	revcor = -5;

	R_E = oapiGetSize(hEarth);

	precision = 1;
	errorstate = 0;
}

void Entry::newxt2(int n1, double xt2err, double &xt2_apo, double &xt2, double &xt2err_apo)
{
	double Dxt2;

	if (n1 == 1)
	{
		Dxt2 = xt2err;
	}
	else
	{
		Dxt2 = xt2err;//*((xt2_apo - xt2) / (xt2err - xt2err_apo));
	}
	xt2err_apo = xt2err;
	xt2_apo = xt2;
	xt2 = xt2 + Dxt2;
}

void Entry::xdviterator(VECTOR3 R1B, VECTOR3 V1B, double theta1, double theta2, double theta3, VECTOR3 U_R1, VECTOR3 U_H, double dx, double xmin, double xmax, double &x)
{
	int i;
	double epsilon, p_CON, dvapo, beta8, beta9;
	VECTOR3 DV, V2;

	i = 0;
	epsilon = OrbMech::power(2.0, -20.0);

	dvcalc(V1B, theta1, theta2, theta3, x, U_R1, U_H, V2, DV, p_CON);

	beta8 = xmax - xmin;
	if (beta8 <= dxmax)
	{
		dx = 0.5*beta8*OrbMech::sign(dx);
	}

	while (abs(dx) > epsilon && i < 135)
	{
		dvapo = length(DV);
		xapo = x;
		x += dx;
		dvcalc(V1B, theta1, theta2, theta3, x, U_R1, U_H, V2, DV, p_CON);
		if (length(DV) > dvapo)
		{
			dx = -dx*0.5;
		}
		beta9 = x + 1.1*dx;
		if (beta9 > xmax)
		{
			dx = (xmax - x) / 2.0;
		}
		else if (beta9 < xmin)
		{
			dx = (xmin - x) / 2.0;
		}
		i++;
	}
}

void Entry::dvcalc(VECTOR3 V1B, double theta1, double theta2, double theta3, double x, VECTOR3 U_R1, VECTOR3 U_H, VECTOR3 &V2, VECTOR3 &DV, double &p_CON)
{
	p_CON = theta2 / (theta1 - x*x);
	V2 = (U_R1*x + U_H)*theta3*sqrt(p_CON);
	DV = V2 - V1B;
}

void Entry::limitxchange(double theta1, double theta2, double theta3, VECTOR3 V1B, VECTOR3 U_R1, VECTOR3 U_H, double xmin, double xmax, double &x)
{
	double beta7, p_CON;
	VECTOR3 V2, DV;

	beta7 = abs(x - xapo);
	if (beta7>2.0*dxmax)
	{
		x = xapo;
		if (x > xmax)
		{
			x = xmax;
		}
		else
		{
			if (x < xmin)
			{
				x = xmin;
			}
		}
		dvcalc(V1B, theta1, theta2, theta3, x, U_R1, U_H, V2, DV, p_CON);
	}
}

void Entry::reentryconstraints(int n1, VECTOR3 R1B, VECTOR3 REI, VECTOR3 VEI)
{
	if (n1 == 0)
	{
		if (EntryAng == 0)
		{
			if (length(R1B) > k1)
			{
				x2 = k4;
			}
			else
			{
				x2 = k3;
			}
		}
		//n1 = 1;
	}
	else
	{
		if (EntryAng == 0)
		{
			double v2;
			v2 = length(VEI);
			x2 = D1 + D2*v2 + D3*v2*v2 + D4*v2*v2*v2;
		}
		else
		{
			x2 = x2;
		}
	}
}

void Entry::coniciter(VECTOR3 R1B, VECTOR3 V1B, double t1, double &theta_long, double &theta_lat, VECTOR3 &V2, double &x, double &dx, double &t21)
{
	VECTOR3 U_R1, U_H, REI, VEI;
	double MA2, x2_err, C_FPA;
	int n1;

	x2_err = 1.0;
	precomputations(1, R1B, V1B, U_R1, U_H, MA2, C_FPA);
	n1 = 1;
	while (abs(x2_err) > 0.00001 && n1 <= 10)
	{
		conicreturn(0, R1B, V1B, MA2, C_FPA, U_R1, U_H, V2, x, n1);
		t21 = OrbMech::time_radius(R1B, V2, RCON, -1, mu);
		OrbMech::rv_from_r0v0(R1B, V2, t21, REI, VEI, mu);
		x2_apo = x2;
		reentryconstraints(n1, R1B, REI, VEI);
		x2_err = x2_apo - x2;
		//newxt2(n1, x2_err, x2_apo, x2, x2_err_apo);
		n1++;
	}
	t2 = t1 + t21;
	OrbMech::rv_from_r0v0(R1B, V2, t21, REI, VEI, mu);
	EntryCalculations::landingsite(REI, VEI, GETbase + t2 / 24.0 / 3600.0, theta_long, theta_lat);
}

void Entry::precisioniter(VECTOR3 R1B, VECTOR3 V1B, double t1, double &t21, double &x, double &theta_long, double &theta_lat, VECTOR3 &V2)
{
	double RD, R_ERR, dRCON, rPRE_apo, r1b, lambda, beta1, beta5, theta1, theta2, p_CON, C_FPA, MA2, x2_err;
	VECTOR3 U_R1, U_V1, RPRE, VPRE, U_H, eta;
	int n1, n2;

	n1 = 0;
	n2 = 0;
	RCON = oapiGetSize(hEarth) + EntryInterface;
	RD = RCON;
	R_ERR = 1000.0;
	x2_err = 1.0;

	U_R1 = unit(R1B);
	U_V1 = unit(V1B);
	C_FPA = dotp(U_R1, U_V1);
	if (abs(C_FPA) < 0.99966)
	{
		eta = crossp(R1B, V1B);
	}
	else
	{
		eta = _V(0.0, 0.0, 1.0);
	}
	if (eta.z < 0)
	{
		eta = -eta;
	}
	U_H = unit(crossp(eta, R1B));
	r1b = length(R1B);
	MA2 = C0 + C1*r1b + C2*r1b*r1b + C3*r1b*r1b*r1b;

	lambda = r1b / RCON;
	beta1 = 1.0 + x2*x2;
	beta5 = lambda*beta1;
	theta1 = beta5*lambda - 1.0;
	theta2 = 2.0*r1b*(lambda - 1.0);
	p_CON = theta2 / (theta1 - x*x);

	if (RCON - p_CON*beta1 >= 0)
	{
		phi2 = 1.0;
	}
	else
	{
		phi2 = -1.0;
	}

	finalstatevector(R1B, V2, beta1, t21, RPRE, VPRE);
	//reentryconstraints(n1 + 1, R1B, VPRE);
	//x2 = x2_apo;
	//beta1 = 1.0 + x2*x2;
	R_ERR = length(RPRE) - RD;
	while (abs(x2_err) > 0.00001 && n2 <= 10)
	{
		n1 = 0;
		while (n1 == 0 || (abs(R_ERR) > 100.0 && n1 <= 20))
		{
			//finalstatevector(R1B, V2, beta1, t21, RPRE, VPRE);
			//R_ERR = length(RPRE) - RD;
			newrcon(n1, RD, length(RPRE), R_ERR, dRCON, rPRE_apo);
			conicreturn(1, R1B, V1B, MA2, C_FPA, U_R1, U_H, V2, x, n1);
			finalstatevector(R1B, V2, beta1, t21, RPRE, VPRE);
			R_ERR = length(RPRE) - RD;
			n1++;
		}
		x2_apo = x2;
		reentryconstraints(n2, R1B, RPRE, VPRE);
		x2_err = x2_apo - x2;
		beta1 = 1.0 + x2*x2;
		n2++;
	}
	t2 = t1 + t21;
	EntryCalculations::landingsite(RPRE, VPRE, GETbase + t2 / 24.0 / 3600.0, theta_long, theta_lat);
	if (n1 == 21)
	{
		errorstate = 1;
	}
	if (n2 == 11)
	{
		errorstate = 2;
	}
}

void Entry::newrcon(int n1, double RD, double rPRE, double R_ERR, double &dRCON, double &rPRE_apo)
{
	double S;

	if (n1 == 0)
	{
		this->RCON = RD*RD / rPRE;
		dRCON = RCON - RD;
	}
	else
	{
		S = dRCON / (rPRE_apo - rPRE);
		if (abs(S + 2.0) > 2.0)
		{
			S = -4.0;
		}
		dRCON = S*R_ERR;
		this->RCON = RCON + dRCON;
		if (RCON < 1e6)
		{
			RCON = 1e6;
		}
	}
	rPRE_apo = rPRE;
}

void Entry::finalstatevector(VECTOR3 R1B, VECTOR3 V2, double beta1, double &t21, VECTOR3 &RPRE, VECTOR3 &VPRE)
{
	VECTOR3 N;
	double beta12, x2PRE, c3, alpha_N, sing, cosg, p_N, beta2, beta3, beta4, RF, phi4, dt21, beta13, dt21apo, beta14;

	OrbMech::oneclickcoast(R1B, V2, mjd + (dt0 + dt1) / 24.0 / 3600.0, t21, RPRE, VPRE, hEarth, hEarth);

	beta12 = 100.0;
	x2PRE = 1000000;
	dt21apo = 100000000.0;

	while (abs(beta12) > 0.000007 && abs(x2 - x2PRE) > 0.00001)
	{
		c3 = length(RPRE)*pow(length(VPRE), 2.0) / mu;
		alpha_N = 2.0 - c3;
		N = crossp(unit(RPRE), unit(VPRE));
		sing = length(N);
		cosg = dotp(unit(RPRE), unit(VPRE));
		x2PRE = cosg / sing;
		p_N = c3*sing*sing;
		beta2 = p_N*beta1;
		beta3 = 1.0 - alpha_N*beta2;
		if (beta3 < 0)
		{
			beta4 = 1.0 / alpha_N;
		}
		else
		{
			beta4 = beta2 / (1.0 - phi2*sqrt(beta3));
		}
		beta12 = beta4 - 1.0;
		//if (abs(beta12) > 0.000007)
		//{
			RF = beta4*length(RPRE);
			if (beta12 > 0)
			{
				phi4 = -1.0;
			}
			else if (x2PRE > 0)
			{
				phi4 = -1.0;
			}
			else
			{
				phi4 = 1.0;
			}
			dt21 = OrbMech::time_radius(RPRE, VPRE*phi4, RF, -phi4, mu);
			dt21 = phi4*dt21;
			beta13 = dt21 / dt21apo;
			if (beta13 > 0)
			{
				beta14 = 1.0;
			}
			else
			{
				beta14 = -0.6;
			}
			if (beta13 / beta14 > 1.0)
			{
				dt21 = beta14*dt21apo;
			}
			dt21apo = dt21;
			OrbMech::oneclickcoast(RPRE, VPRE, mjd + (dt0 + dt1 + t21) / 24.0 / 3600.0, dt21, RPRE, VPRE, hEarth, hEarth);
			t21 += dt21;
		//}
	}
}

double Entry::dvmaxiterator(VECTOR3 R1B, VECTOR3 V1B, double theta1, double theta2, double theta3, VECTOR3 U_R1, VECTOR3 U_H, double xmin, double dxmax, double dv_des)
{
	double R0, R, x_apo, p, xx, dxx, dv, dv_apo;
	int i;
	VECTOR3 V;

	x_apo = 100000;
	xx = xmin;
	dxx = dxmax;
	i = 0;

	R0 = length(R1B);
	R = R0 / RCON;

	while (abs(x_apo - xx) > OrbMech::power(2.0, -20.0) && i <= 100)
	{
		p = 2.0*R0*(R - 1.0) / (R*R*(1.0 + x2*x2) - (1.0 + xx*xx));
		V = (unit(R1B)*xx + unit(crossp(crossp(R1B, V1B), R1B)))*sqrt(mu*p) / R0;

		dv = length(V - V1B);
		dv_err = dv_des - dv;

		if (i > 0)
		{
			dxx = (xx - x_apo) / (dv - dv_apo)*dv_err;
		}
		x_apo = xx;
		dv_apo = dv;
		xx += dxx;

		if (xx < xmin)
		{
			xx = xmin;
		}
		i++;
	}
	return xx;
}

void Entry::conicreturn(int f1, VECTOR3 R1B, VECTOR3 V1B, double MA2, double C_FPA, VECTOR3 U_R1, VECTOR3 U_H, VECTOR3 &V2, double &x, int &n1)
{
	double theta1, theta2, theta3, xmin, xmax, p_CON, beta6, dx;
	VECTOR3 DV;
	conicinit(R1B, MA2, xmin, xmax, theta1, theta2, theta3);
	if (f1 == 0)
	{
		if (ii == 0 && entryphase == 0)
		{
			x = dvmaxiterator(R1B, V1B, theta1, theta2, theta3, U_R1, U_H, xmin, dxmax, 2804.0);
			dx = dxmax;
		}
		if (ii == 0)
		{
			if (critical == 1 && entryphase == 0)
			{
				//xdviterator(R1B, V1B, theta1, theta2, theta3, U_R1, U_H, dx, xmin, xmax, x);
				VECTOR3 N;
				double sing, cosg;
				N = crossp(unit(R1B), unit(V1B));
				sing = length(N);
				cosg = dotp(unit(R1B), unit(V1B));
				x = cosg / sing;
				if (x*x > theta1)
				{
					x = 0.0;
				}
			}
			else if (critical == 3)
			{
				xdviterator(R1B, V1B, theta1, theta2, theta3, U_R1, U_H, dx, xmin, xmax, x);
			}
			//else
			//{
			//	dvcalc(V1B, theta1, theta2, theta3, x, U_R1, U_H, V2, DV, p_CON);
			//}
		}
		else
		{
			if (critical == 3)
			{
				xdviterator(R1B, V1B, theta1, theta2, theta3, U_R1, U_H, dx, xmin, xmax, x);
			}
		}
	}
	else
	{
		double r1b, lambda, beta1, beta5;
		r1b = length(R1B);
		lambda = r1b / RCON;
		beta1 = 1.0 + x2*x2;
		beta5 = lambda*beta1;
		beta6 = beta5*(2.0 - lambda) - 1.0;
		if (critical == 3)
		{
			if (beta6 > 0)
			{
				xlim = sqrt(beta6);
				if (phi2 == 1)
				{
					xmax = xlim;
					xmin = -xlim;
					if (x > 0)
					{
						x = xmax;
						dx = -dxmax;
					}
					else
					{
						x = xmin;
						dx = dxmax;
					}
				}
				else
				{
					if (x > 0)
					{
						xmin = xlim;
						x = xmax;
						dx = -dxmax;
					}
					else
					{

						xmax = -xlim;
						x = xmin;
						dx = dxmax;
					}
				}
			}
			else
			{
				if (phi2 == 1)
				{
					phi2 = -1.0;
					n1 = -1;
				}
				if (x > 0)
				{
					x = xmax;
					dx = -dxmax;
				}
				else
				{
					x = xmin;
					dx = dxmax;
				}
			}

			xdviterator(R1B, V1B, theta1, theta2, theta3, U_R1, U_H, dx, xmin, xmax, x);
		}
	}
	dvcalc(V1B, theta1, theta2, theta3, x, U_R1, U_H, V2, DV, p_CON);
}

void Entry::conicinit(VECTOR3 R1B, double MA2, double &xmin, double &xmax, double &theta1, double &theta2, double &theta3)
{
	double r1b, lambda, beta1, beta5, beta10;
	r1b = length(R1B);

	lambda = r1b / RCON;
	beta1 = 1.0 + x2*x2;
	beta5 = lambda*beta1;
	theta1 = beta5*lambda - 1.0;
	theta2 = 2.0*r1b*(lambda - 1.0);
	theta3 = sqrt(mu) / r1b;
	beta10 = beta5*(MA1 - r1b) / (MA1 - RCON) - 1.0;
	if (beta10 < 0.0)
	{
		xmin = 0.0;
	}
	else
	{
		xmin = -sqrt(beta10);
	}
	dxmax = -xmin / 16.0;
	beta10 = beta5*(MA2 - r1b) / (MA2 - RCON) - 1.0;
	if (beta10 < 0.0)
	{
		xmax = 0.0;
	}
	else
	{
		xmax = sqrt(beta10);
	}
}

void Entry::precomputations(bool x2set, VECTOR3 R1B, VECTOR3 V1B, VECTOR3 &U_R1, VECTOR3 &U_H, double &MA2, double &C_FPA)
{
	VECTOR3 U_V1, eta;
	double r1b;

	U_R1 = unit(R1B);
	U_V1 = unit(V1B);
	C_FPA = dotp(U_R1, U_V1);
	if (abs(C_FPA) < 0.99966)
	{
		eta = crossp(R1B, V1B);
	}
	else
	{
		eta = _V(0.0, 0.0, 1.0);
	}
	if (eta.z < 0)
	{
		eta = -eta;
	}
	U_H = unit(crossp(eta, R1B));
	r1b = length(R1B);
	MA2 = C0 + C1*r1b + C2*r1b*r1b + C3*r1b*r1b*r1b;
	if (x2set)
	{
		reentryconstraints(0, R1B, _V(0, 0, 0), _V(0, 0, 0));
	}
}

bool Entry::EntryIter()
{
	double theta_long, theta_lat, dlng;
	VECTOR3 R1B, V1B, V2;

	errorstate = 0;

	dt1 = EntryTIGcor - get - dt0;
	OrbMech::oneclickcoast(R11B, V11B, mjd + dt0 / 24.0 / 3600.0, dt1, R1B, V1B, hEarth, hEarth);
	
	if (entryphase == 0)
	{
		coniciter(R1B, V1B, EntryTIGcor, theta_long, theta_lat, V2, x, dx, t21);
	}
	else
	{
		precisioniter(R1B, V1B, EntryTIGcor, t21, x, theta_long, theta_lat, V2);
	}

	if (!entrylongmanual)
	{
		EntryLng = EntryCalculations::landingzonelong(landingzone, theta_lat);
	}

	dlng = EntryLng - theta_long;
	if (abs(dlng) > PI)
	{
		dlng = dlng - OrbMech::sign(dlng)*PI2;
	}
	if (critical == 3)
	{

	}
	else
	{
		if (ii == 0 && entryphase == 0)
		{
			dx = -dlng * RAD;
			xapo = x;
			dlngapo = theta_long;
			x += dx;
		}
		else
		{
			dx = (x - xapo) / (theta_long - dlngapo)*dlng;
			if (length(V2 - V1B) > 2804.0 && dx < 0)
			{
				dx = 0.5*max(1.0, revcor);
				revcor++;
			}
			else if (abs(dx) > dxmax)
			{
				dx = OrbMech::sign(dx)*dxmax;
			}
			xapo = x;
			dlngapo = theta_long;
			x += dx;

		}
	}


	ii++;

	if (entryphase == 0)
	{
		entryphase = 1;
		return false;
	}
	else if (((abs(dlng) > 0.005*RAD && ii < 60) || entryphase == 0) && critical != 3)
	{
		if (critical == 3 || (abs(dlng)<0.1*RAD) && abs(dx)<0.1)
		{
			if (entryphase == 0)
			{
				entryphase = 1;
				ii = 0;
			}
		}
		return false;
	}
	else
	{
		VECTOR3 R05G, V05G, REI, VEI, R3, V3, UR3, U_R1, U_H, DV;
		double t32, dt22, v3, S_FPA, gammaE, phie, te, MA2, C_FPA;

		precomputations(0, R1B, V1B, U_R1, U_H, MA2, C_FPA);

		t2 = EntryTIGcor + t21;
		OrbMech::time_radius_integ(R1B, V2, mjd + (dt0 + dt1) / 24.0 / 3600.0, RD, -1, hEarth, hEarth, REI, VEI);//Maneuver to Entry Interface (400k ft)

		t32 = OrbMech::time_radius(REI, VEI, length(REI) - 30480.0, -1, mu);
		OrbMech::rv_from_r0v0(REI, VEI, t32, R3, V3, mu); //Entry Interface to 300k ft

		dt22 = OrbMech::time_radius(R3, V3, length(R3) - (300000.0 * 0.3048 - EMSAlt), -1, mu);
		OrbMech::rv_from_r0v0(R3, V3, dt22, R05G, V05G, mu); //300k ft to 0.05g

		UR3 = unit(R3);
		v3 = length(V3);
		S_FPA = dotp(UR3, V3) / v3;
		gammaE = asin(S_FPA);
		EntryCalculations::augekugel(v3, gammaE, phie, te);

		VECTOR3 Rsph, Vsph;

		OrbMech::oneclickcoast(R1B, V1B, mjd + (dt0 + dt1) / 24.0 / 3600.0, 0.0, Rsph, Vsph, hEarth, SOIplan);
		//OrbMech::oneclickcoast(R1B, V2, mjd + (dt0 + dt1) / 24.0 / 3600.0, 0.0, Rsph, Vsph2, hEarth, SOIplan);
		DV = V2 - V1B;
		//DV = Vsph2 - Vsph;
		VECTOR3 i, j, k;
		MATRIX3 Q_Xx;
		j = unit(crossp(Vsph, Rsph));
		k = unit(-Rsph);
		i = crossp(j, k);
		Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);

		Entry_DV = mul(Q_Xx, DV);
		//U_R1 = unit(Rsph);
		//eta = crossp(Rsph, Vsph);
		//U_H = unit(crossp(eta, Rsph));

		//Entry_DV = _V(dotp(DV, U_H), 0.0, -dotp(DV, U_R1));
		EntryRTGO = phie - 3437.7468*acos(dotp(unit(R3), unit(R05G)));
		EntryVIO = length(V05G);
		EntryRET = t2 + t32 + dt22;
		EntryAng = atan(x2);//asin(dotp(unit(REI), VEI) / length(VEI));

		EntryLngcor = theta_long;
		EntryLatcor = theta_lat;

		if (errorstate != 0)
		{
			precision = 9;
		}

		return true;
	}
}

OBJHANDLE Entry::AGCGravityRef(VESSEL *vessel)
{
	OBJHANDLE gravref;
	VECTOR3 rsph;

	gravref = oapiGetObjectByName("Moon");
	vessel->GetRelativePos(gravref, rsph);
	if (length(rsph) > 64373760.0)
	{
		gravref = oapiGetObjectByName("Earth");
	}
	return gravref;
}

Flyby::Flyby(VECTOR3 R0M, VECTOR3 V0M, double mjd0, OBJHANDLE gravref, double MJDguess, double EntryLng, bool entrylongmanual, int returnspeed, int FlybyType)
{
	VECTOR3 R1B, V1B;

	this->EntryLng = EntryLng;

	hMoon = oapiGetObjectByName("Moon");
	hEarth = oapiGetObjectByName("Earth");
	this->entrylongmanual = entrylongmanual;

	if (entrylongmanual)
	{
		this->EntryLng = EntryLng;
	}
	else
	{
		landingzone = (int)EntryLng;
		this->EntryLng = EntryCalculations::landingzonelong(landingzone, 0);
	}
	this->FlybyType = FlybyType;
	this->mjd0 = mjd0;

	mu_E = GGRAV*oapiGetMass(hEarth);
	mu_M = GGRAV*oapiGetMass(hMoon);

	if (FlybyType == 0)
	{
		TIG = MJDguess;

		OrbMech::oneclickcoast(R0M, V0M, mjd0, (TIG - mjd0) * 24.0 * 3600.0, R1B, V1B, gravref, hMoon);

		double dt = OrbMech::timetoperi(R1B, V1B, mu_M);
		if (dt > 0)
		{
			INRFVsign = false;
		}
		else
		{
			INRFVsign = true;
		}
	}
	if (FlybyType == 1)
	{
		VECTOR3 R0MM, V0MM;
		INRFVsign = true;
		OrbMech::oneclickcoast(R0M, V0M, mjd0, 0.0, R0MM, V0MM, gravref, hMoon);
		double dt = OrbMech::timetoperi(R0MM, V0MM, mu_M);
		TIG = mjd0 + dt / 24.0 / 3600.0 + 2.0 / 24.0;

		OrbMech::oneclickcoast(R0M, V0M, mjd0, (TIG - mjd0) * 24.0 * 3600.0, R1B, V1B, gravref, hMoon);
	}

	Rig = R1B;
	Vig = V1B;
	Vig_apo = Vig;

	DT_TEI_EI = 62.0*3600.0 + OrbMech::timetoperi(Rig, Vig, mu_M);

	if (returnspeed == 0)
	{
		DT_TEI_EI += 24.0*3600.0;
	}
	else if (returnspeed == 2)
	{
		DT_TEI_EI -= 24.0*3600.0;
	}

	cMoon = oapiGetCelbodyInterface(hMoon);
	ii = 0;
	precision = 1;
}

bool Flyby::Flybyiter()
{
	double theta_long, theta_lat, dlng, dt;

	EIMJD = TIG + DT_TEI_EI / 24.0 / 3600.0;

	Vig_apo = EntryCalculations::ThreeBodyAbort(TIG, EIMJD, Rig, Vig, mu_E, mu_M, INRFVsign, R_EI, V_EI);

	EntryCalculations::landingsite(R_EI, V_EI, EIMJD, theta_long, theta_lat);

	dlng = EntryLng - theta_long;
	if (abs(dlng) > PI)
	{
		dlng = dlng - OrbMech::sign(dlng)*PI2;
	}

	if (!entrylongmanual)
	{
		EntryLng = EntryCalculations::landingzonelong(landingzone, theta_lat);
	}

	if (ii == 0)
	{
		dt = 1.0;
		dtapo = DT_TEI_EI;
		dlngapo = theta_long;
		DT_TEI_EI += dt;

	}
	else
	{
		dt = (DT_TEI_EI - dtapo) / (theta_long - dlngapo)*dlng;

		if (abs(dt) > 3600.0)
		{
			dt = OrbMech::sign(dt)*3600.0;
		}
		dtapo = DT_TEI_EI;
		dlngapo = theta_long;
		DT_TEI_EI += dt;

	}
	ii++;

	if (abs(dt) > 0.1)
	{
		return false;
	}
	else
	{
		double sing, cosg, x2;
		VECTOR3 i, j, k, N;
		MATRIX3 Q_Xx;
		j = unit(crossp(Vig, Rig));
		k = unit(-Rig);
		i = crossp(j, k);
		Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);

		Entry_DV = mul(Q_Xx, Vig_apo - Vig);
		EntryLatcor = theta_lat;
		EntryLngcor = theta_long;
		N = crossp(unit(R_EI), unit(V_EI));
		sing = length(N);
		cosg = dotp(unit(R_EI), unit(V_EI));
		x2 = cosg / sing;
		EntryAng = atan(x2);

		return true;
	}
}

TEI::TEI(VECTOR3 R0M, VECTOR3 V0M, double mjd0, OBJHANDLE gravref, double MJDguess, double EntryLng, bool entrylongmanual, int returnspeed, int RevsTillTEI)
{
	VECTOR3 R1B, V1B;

	this->EntryLng = EntryLng;

	hMoon = oapiGetObjectByName("Moon");
	hEarth = oapiGetObjectByName("Earth");
	this->entrylongmanual = entrylongmanual;

	if (entrylongmanual)
	{
		this->EntryLng = EntryLng;
	}
	else
	{
		landingzone = (int)EntryLng;
		this->EntryLng = EntryCalculations::landingzonelong(landingzone, 0);
	}
	this->mjd0 = mjd0;

	mu_E = GGRAV*oapiGetMass(hEarth);
	mu_M = GGRAV*oapiGetMass(hMoon);

	double dt;
	INRFVsign = true;

	if (MJDguess != mjd0)
	{
		dt = (MJDguess - mjd0)*24.0*3600.0;
	}
	else
	{
		double t_period;
		t_period = OrbMech::period(R0M, V0M, mu_M);
		dt = t_period*(double)RevsTillTEI;
	}

	OrbMech::oneclickcoast(R0M, V0M, mjd0, dt, R1B, V1B, gravref, hMoon);
	TIG = OrbMech::P29TimeOfLongitude(R1B, V1B, mjd0 + dt / 24.0 / 3600.0, hMoon, 180.0*RAD);
	OrbMech::oneclickcoast(R0M, V0M, mjd0, (TIG - mjd0)*24.0*3600.0, R1B, V1B, hMoon, hMoon);

	Rig = R1B;
	Vig = V1B;
	Vig_apo = Vig;
	
	DT_TEI_EI = 62.0*3600.0;

	if (returnspeed == 0)
	{
		DT_TEI_EI += 24.0*3600.0;
	}
	else if (returnspeed == 2)
	{
		DT_TEI_EI -= 24.0*3600.0;
	}

	cMoon = oapiGetCelbodyInterface(hMoon);
	ii = 0;
	jj = 0;
	dTIG = 30.0;
	precision = 1;
}

bool TEI::TEIiter()
{
	double theta_long, theta_lat, dlng, dt;

	EIMJD = TIG + DT_TEI_EI / 24.0 / 3600.0;

	Vig_apo = EntryCalculations::ThreeBodyAbort(TIG, EIMJD, Rig, Vig, mu_E, mu_M, INRFVsign, R_EI, V_EI);

	EntryCalculations::landingsite(R_EI, V_EI, EIMJD, theta_long, theta_lat);

	dlng = EntryLng - theta_long;
	if (abs(dlng) > PI)
	{
		dlng = dlng - OrbMech::sign(dlng)*PI2;
	}

	if (!entrylongmanual)
	{
		EntryLng = EntryCalculations::landingzonelong(landingzone, theta_lat);
	}

	if (ii == 0)
	{
		dt = 1.0;
		dtapo = DT_TEI_EI;
		dlngapo = theta_long;
		DT_TEI_EI += dt;

	}
	else
	{
		dt = (DT_TEI_EI - dtapo) / (theta_long - dlngapo)*dlng;

		if (abs(dt) > 3600.0)
		{
			dt = OrbMech::sign(dt)*3600.0;
		}
		dtapo = DT_TEI_EI;
		dlngapo = theta_long;
		DT_TEI_EI += dt;

	}
	ii++;

	if (abs(dt) > 0.1)
	{
		return false;
	}
	else
	{
		if (abs(dTIG) > 1.0)
		{

			ii = 0;

			if (jj < 2)
			{
				TIGvar[jj + 1] = (TIG - mjd0)*24.0*3600.0;
				dv[jj + 1] = length(Vig_apo - Vig);
			}
			else
			{
				dv[0] = dv[1];
				dv[1] = dv[2];
				dv[2] = length(Vig_apo - Vig);
				TIGvar[0] = TIGvar[1];
				TIGvar[1] = TIGvar[2];
				TIGvar[2] = (TIG - mjd0)*24.0*3600.0;

				dTIG = OrbMech::quadratic(TIGvar, dv) + (mjd0 - TIG)*24.0*3600.0;

				if (abs(dTIG) > 10.0*60.0)
				{
					dTIG = OrbMech::sign(dTIG)*10.0*60.0;
				}
			}

			OrbMech::oneclickcoast(Rig, Vig, TIG, dTIG, Rig, Vig, hMoon, hMoon);
			TIG += dTIG / 24.0 / 3600.0;
			jj++;

			return false;
		}
		else
		{
			double sing, cosg, x2;
			VECTOR3 i, j, k, N;
			MATRIX3 Q_Xx;
			j = unit(crossp(Vig, Rig));
			k = unit(-Rig);
			i = crossp(j, k);
			Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);

			Entry_DV = mul(Q_Xx, Vig_apo - Vig);
			EntryLatcor = theta_lat;
			EntryLngcor = theta_long;
			N = crossp(unit(R_EI), unit(V_EI));
			sing = length(N);
			cosg = dotp(unit(R_EI), unit(V_EI));
			x2 = cosg / sing;
			EntryAng = atan(x2);

			return true;
		}
	}
}