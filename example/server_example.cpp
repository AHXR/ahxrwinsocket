/*
	@title
		ahxrwinsock
	@author
		AHXR (https://github.com/AHXR)
	@copyright
		2017

	ahxrwinsock is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	ahxrwinsock is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with ahxrwinsock.  If not, see <http://www.gnu.org/licenses/>.
*/
//=======================================================
#include			"stdafx.h"
#include			"ahxrwinsock.h"
#include			<iostream>

//#define				SERVER_SILENT // Silent server; no callbacks; multi-threaded

using namespace		System;
using namespace     std;

#ifndef SERVER_SILENT
	void				onServerClientConnect( CLIENTDATA info );
	void				onServerReceiveData( SOCKET clientSocket, CLIENTDATA info, char * data );
#endif

AHXRSERVER			MyServer;

int main()
{
	string			s_port;
	
	cout			<< "[ahxrwinsock example (TCP)]\n\nEnter the port you would like to use for your server.\nPort: ";
	cin				>> s_port;
	cout			<< "Starting server...\n\n";

#ifndef SERVER_SILENT
	MyServer.start_server
	( 
		s_port.c_str(), 
		TCP_SERVER, 
		onServerClientConnect, 
		onServerReceiveData,
		false // Letting the server control this thread - true by default
	);
#else
	// Testing no callbacks silent multi-threaded server.
	MyServer.start_server( s_port.c_str(), TCP_SERVER );
	system("pause");
#endif
    return 0;
}

#ifndef SERVER_SILENT
void onServerClientConnect( CLIENTDATA info ) {
	SOCKADDR_IN			socket_in;
	getsockname(0, (SOCKADDR *) & socket_in, ( int * ) sizeof( socket_in ) );
	
	cout << "IP: " << inet_ntoa(socket_in.sin_addr) << " has connected to the server.\n";
}

void onServerReceiveData(SOCKET clientSocket, CLIENTDATA info, char * data) {
	cout << "Client sent data: " << data << endl;

	if (!strcmp(data, "hi"))
		MyServer.send_data(clientSocket, "howdy!");
	else if( !strcmp( data, "bye") ) 
		MyServer.send_data(clientSocket, "goodbye!");
	else
		MyServer.send_data(clientSocket, "This is a response message");
}
#endif