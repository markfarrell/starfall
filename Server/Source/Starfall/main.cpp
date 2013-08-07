//Copyright (c) 2013 Mark Farrell

#include "Starfall/Send.h"
#include "Starfall/Receive.h"
#include "Starfall/LoginServer.h"
#include "Starfall/Database.h"

using namespace Starfall;

int main ( void )
{
	Send::Init();
	Receive::Init();
	LoginHTTPRequest::Init();

	LoginServer loginServer;


	while(true) { }

	return 0;
}