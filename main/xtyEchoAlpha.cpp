
#include "xty.h"

using namespace std;

static xtySession* g_session_pointer = NULL;

static void SIGINT_Handler(int signo)
{
	if (g_session_pointer)
	{
		g_session_pointer->Exit();
		delete g_session_pointer;
		g_session_pointer = NULL;
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

	g_session_pointer = new xtySession("echo-alpha", xtyPool::GetInstance());

	if (g_session_pointer)
		g_session_pointer->Start();

	// make sure that 'echo-beta' is already online
	sleep(5);
	
	for (size_t i = 0; i < 1024; i++)
	{	
		xtyDataMsg smsg;
		
		// write send msg content
		smsg.m_ToProcessIP = "127.0.0.1";
		smsg.m_ToProcessName = "echo-beta";
		smsg.m_Content = "alpha says hello to beta";
		
		// send hello to echo-beta
		g_session_pointer->SendMsg(smsg);
		
		// wait for response
		xtyDataMsg rmsg = g_session_pointer->RecvMsg();
		
		// dump recv msg content
		std::cout << rmsg.m_Content << std::endl;

		// optional sleep
		sleep(1);
	}

	if (g_session_pointer)
	{
		g_session_pointer->Exit();
		delete g_session_pointer;
		g_session_pointer = NULL;
	}
	
	xtyFundamentalFacility::CleanUpFacilities();
	
	return 0;
}

