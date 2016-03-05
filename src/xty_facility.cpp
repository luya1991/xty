
#include "xty.h"

xtyFundamentalFacility::xtyFundamentalFacility()
{

}

xtyFundamentalFacility::~xtyFundamentalFacility()
{

}

void xtyFundamentalFacility::InitFacilities()
{
	xtyError::Create();
	
	xtyLog::Create();

	xtyPool::Create();
	
	// 2015-12-16 abandoned
	// the program cannot find 'xtySharedPool::Destroy()' in it's addr-space
	// haven't found out the reason yet, maybe be some memory-mapping problem I guess
	// it's very weird, when there is no client, it runs ok, but when clients come, SIGSEGV occurrs!!
	// xtySharedPool::Create();
	
	xtyConfParse::Create();
}

void xtyFundamentalFacility::CleanUpFacilities()
{
	xtyConfParse::Destroy();
	
	// 2015-12-16 abandoned
	// the program cannot find 'xtySharedPool::Destroy()' in it's addr-space
	// haven't found out the reason yet, maybe be some memory-mapping problem I guess
	// it's very weird, when there is no client, it runs ok, but when clients come, SIGSEGV occurrs!!
	// xtySharedPool::Destroy();

	xtyPool::Destroy();

	xtyLog::Destroy();
	
	xtyError::Destroy();
}

