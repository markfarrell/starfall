
#include "Send.h"
#include "Receive.h"
#include "LoginServer.h"
#include "Database.h"


int main ( void )
{
	Send::Init();
	Receive::Init();
	LoginHTTPRequest::Init();

	LoginServer loginServer;


	while(true) { }

	return 0;
}