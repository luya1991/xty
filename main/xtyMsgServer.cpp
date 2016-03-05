
#include "xty.h"

using namespace std;

static unsigned int g_time_eternity = 2147483647;
static xtyMsgServer* g_msg_server_pointer = NULL;

static void SIGINT_Handler(int signo)
{
	if (g_msg_server_pointer)
	{
		g_msg_server_pointer->Exit();
		delete g_msg_server_pointer;
		g_msg_server_pointer = NULL;
	}
	
	xtyFundamentalFacility::CleanUpFacilities();

	exit(signo);
}

int main()
{
	xtyFundamentalFacility::InitFacilities();

	struct sigaction act, oact;

	act.sa_handler = SIGINT_Handler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);

	sigaction(SIGINT, &act, &oact);

	g_msg_server_pointer = new xtyMsgServer(xtyPool::GetInstance());

	if (g_msg_server_pointer)
		g_msg_server_pointer->Start();

	// cancelled on 2016-02-05
	// sleep(g_time_eternity);

	while (1)
	{
		sleep(5);
		g_msg_server_pointer->DumpActiveClientInfo();
	}
	
	/* cancelled on 2016-02-05
	if (g_msg_server_pointer)
	{
		g_msg_server_pointer->Exit();
		delete g_msg_server_pointer;
		g_msg_server_pointer = NULL;
	}

	xtyFundamentalFacility::CleanUpFacilities();
	*/
	
	return 0;
}

