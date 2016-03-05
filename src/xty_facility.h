
#ifndef XTY_FACILITY_H
#define XTY_FACILITY_H

class xtyFundamentalFacility
{
public:
	xtyFundamentalFacility();
	~xtyFundamentalFacility();

private:
	xtyFundamentalFacility(const xtyFundamentalFacility&);
	xtyFundamentalFacility& operator = (const xtyFundamentalFacility&);

public:
	static void InitFacilities();
	static void CleanUpFacilities();
};

#endif

