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
bool __AHXRSOCKET__::__createserver(AHXRSERVER * const & server_class, PCSTR port, int protocol, FPC_CALLBACK fpConnected, FPS_CALLBACK fpData) {
	WSADATA				wsaData;
	SOCKET				Socket_Listen	= server_class->Socket_Listen;
	SOCKET				Socket_Client	= server_class->Socket_Client;

	int					i_result;
	int					i_opt			= TRUE;

	struct				addrinfo * res	= NULL;
	struct              addrinfo hints;

	i_result			= WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (i_result != 0) { 
#ifdef AHXRLOGGER_PLUGIN
		ERROR("%s WSA Error %d\r\n", AHXRSOCKET_PREFIX, i_result); 
#else
		printf("%s WSA Error %d\r\n", AHXRSOCKET_PREFIX, i_result);
#endif
		return false; 
	}
	
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family		= AF_INET;
	hints.ai_socktype	= SOCK_STREAM;
	hints.ai_protocol	= protocol;
	hints.ai_flags		= AI_PASSIVE;
	
	i_result			= getaddrinfo(NULL, (PCSTR)port, &hints, &res);
	if (i_result != 0) { 
#ifdef AHXRLOGGER_PLUGIN
		ERROR("%s getaddrinfo error %d", AHXRSOCKET_PREFIX, i_result); 
#else
		printf("%s getaddrinfo error %d", AHXRSOCKET_PREFIX, i_result);
#endif
		return false; 
	}

	Socket_Listen		= socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (Socket_Listen == INVALID_SOCKET) { __AHXRSOCKET__::cleanWSA(res); return false;}

	if (setsockopt(Socket_Listen, SOL_SOCKET, (SO_REUSEADDR), (char*)&i_opt, sizeof i_opt) < 0)
#ifdef AHXRLOGGER_PLUGIN
		WARNING("There was a problem setting your socket options.");
#else
		printf("There was a problem setting your socket options.");
#endif

	i_result = bind(Socket_Listen, res->ai_addr, (int)res->ai_addrlen);
	if (i_result == SOCKET_ERROR) { __AHXRSOCKET__::cleanWSA(res, Socket_Listen); return false; }

	freeaddrinfo(res);
	server_class->Socket_Listen = Socket_Listen;

	while (1)  {
		i_result = listen(Socket_Listen, SOMAXCONN);
		if (i_result == SOCKET_ERROR) 
			break;
	
		Socket_Client = accept(Socket_Listen, NULL, NULL);
		if (Socket_Client == INVALID_SOCKET)
			break;

		__AHXRSOCKET__::s_server_res * s_params = new __AHXRSOCKET__::s_server_res;
		s_params->addrRes = res;
		fpData != NULL ? s_params->fpResData = fpData : s_params->fpResData = NULL;
		s_params->SocketResponse = Socket_Client;

		CreateThread(0, 0, __AHXRSOCKET__::__t__createServerRes, s_params, 0, 0);
		
		if( fpConnected != NULL )
			fpConnected(Socket_Client, res);
	}
	closesocket(Socket_Listen);
	WSACleanup();
	return true;
}

void AHXRSERVER::send_data(SOCKET clientSocket, const char * sendChar) {
	if (send(clientSocket, sendChar, (int)strlen(sendChar), 0) == SOCKET_ERROR) {
		closesocket(clientSocket);
		WSACleanup();
	}
}

void AHXRSERVER::close() {
	if (closesocket(Socket_Listen) == SOCKET_ERROR)
#ifdef AHXRLOGGER_PLUGIN
		ERROR("%s Couldn't close server (%d)", AHXRSOCKET_PREFIX, WSAGetLastError());
#else
		printf("%s Couldn't close server (%d)", AHXRSOCKET_PREFIX, WSAGetLastError());
#endif
}

bool AHXRSERVER::start_server(PCSTR port, int protocol, FPC_CALLBACK fpConnected, FPS_CALLBACK fpData, bool thread) {
	if (!thread) return __AHXRSOCKET__::__createserver(this, port, protocol, fpConnected, fpData);

	s_server_params * server_data	= new s_server_params;
	server_data->port				= port;
	server_data->protocol			= protocol;
	server_data->fpConnected		= fpConnected;
	server_data->fpData				= fpData;
	server_data->socket_l			= Socket_Listen;
	server_data->socket_c			= Socket_Client;
	server_data->s_class			= this;

	CreateThread(0, 0, __AHXRSOCKET__::__t__startserver, server_data, 0, 0);
	return true;
}

DWORD WINAPI __AHXRSOCKET__::__t__startserver(LPVOID lpParams) {
	s_server_params * s_data = (struct s_server_params *)lpParams;
	__AHXRSOCKET__::__createserver(s_data->s_class, s_data->port, s_data->protocol, s_data->fpConnected, s_data->fpData);
	return 0;
}


void __AHXRSOCKET__::cleanWSA(PADDRINFOA pAddr, SOCKET s) {
#ifdef AHXRLOGGER_PLUGIN
	ERROR("%s %d\r\n", AHXRSOCKET_PREFIX, WSAGetLastError());
#else
	printf("%s %d\r\n", AHXRSOCKET_PREFIX, WSAGetLastError());
#endif
	
	if (s != INVALID_SOCKET) {
		freeaddrinfo(pAddr);
		closesocket(s);
	}
	WSACleanup();
}