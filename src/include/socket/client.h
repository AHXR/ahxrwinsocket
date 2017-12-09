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
void AHXRCLIENT::close() {
	if (shutdown(Socket_Client, SD_BOTH) == SOCKET_ERROR)  {
#ifdef AHXRLOGGER_PLUGIN
		ERROR("%s failed with error: %d\n", AHXRSOCKET_CLIENT_PREFIX, WSAGetLastError());
#else
		printf("%s failed with error: %d\n", AHXRSOCKET_CLIENT_PREFIX, WSAGetLastError());
#endif
		closesocket(Socket_Client);
		WSACleanup();
	}
}

void AHXRCLIENT::listen(FP_RES fpRes, bool thread) {
	this->fp_params = fpRes;
	thread ? CreateThread(0, 0, __AHXRSOCKET__::__t__listenThread, this, 0, 0) : this->__listenfunc(fpRes);
}

void AHXRCLIENT::__listenfunc(FP_RES fpRes) {
	int			i_result;
	TCHAR		c_rec[DEFAULT_BUFF];
	do {
		memset(c_rec, 0, DEFAULT_BUFF);
		i_result = recv(Socket_Client, c_rec, DEFAULT_BUFF, 0);
		if (i_result > 0)
			fpRes(c_rec);
		else if (i_result == 0)
#ifdef AHXRLOGGER_PLUGIN
			WARNING("%s Listener stopped..\n", AHXRSOCKET_CLIENT_PREFIX);
#else
			printf("%s Listener stopped..\n", AHXRSOCKET_CLIENT_PREFIX);
#endif
		else
#ifdef AHXRLOGGER_PLUGIN
			ERROR("%s Error while listening ... %d\n", AHXRSOCKET_CLIENT_PREFIX, WSAGetLastError());
#else
			printf("%s Error while listening ... %d\n", AHXRSOCKET_CLIENT_PREFIX, WSAGetLastError());
#endif

	} while (i_result > 0);

	closesocket(Socket_Client);
	WSACleanup();
}

void AHXRCLIENT::set_toggle_callback(FPCLIENT_T pointerFunc) {
	Toggle_Callback = pointerFunc;
}

void AHXRCLIENT::toggle_send_callback(bool toggle) {
	b_toggle = toggle;
}

bool AHXRCLIENT::send_data( const char * sendChar) {
	bool		b_result;

	if (send(Socket_Client, sendChar, (int)strlen(sendChar), 0) == SOCKET_ERROR) {
		closesocket(Socket_Client);
		WSACleanup();
		b_result = false;
	}
	else b_result = true;

	if (b_toggle == true)
		Toggle_Callback(b_result, sendChar);

	return b_result;
}

bool AHXRCLIENT::init(PCSTR server, PCSTR port, int protocol, FPCLIENT_CB fpCB) {
	WSADATA				wsaData;
	SOCKET				Socket_Listen = INVALID_SOCKET;

	int					i_result;

	struct				addrinfo * res = NULL;
	struct              addrinfo * ptr = NULL;
	struct              addrinfo hints;

	i_result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (i_result != 0) { 
#ifdef AHXRLOGGER_PLUGIN
		ERROR("%s WSA Error %d\r\n", AHXRSOCKET_CLIENT_PREFIX, i_result); 
#else
		printf("%s WSA Error %d\r\n", AHXRSOCKET_CLIENT_PREFIX, i_result);
#endif
		return false; 
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = protocol;

	i_result = getaddrinfo(server, port, &hints, &res);
	if (i_result != 0) { 
#ifdef AHXRLOGGER_PLUGIN
		ERROR("%s getaddrinfo error %d\r\n", AHXRSOCKET_CLIENT_PREFIX, i_result); 
#else
		printf("%s getaddrinfo error %d\r\n", AHXRSOCKET_CLIENT_PREFIX, i_result);
#endif
		return false; 
	}

	for (ptr = res; ptr != NULL; ptr = ptr->ai_next) {
		Socket_Client = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (Socket_Client == INVALID_SOCKET) { __AHXRSOCKET__::cleanWSA(res); return false; }

		i_result = connect(Socket_Client, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (i_result == SOCKET_ERROR) {
			__AHXRSOCKET__::cleanWSA(res, Socket_Client);
			Socket_Client = INVALID_SOCKET;
			continue;
		}
		break;
	}

	if (Socket_Client == INVALID_SOCKET) {
		__AHXRSOCKET__::cleanWSA(res);
		return false;
	}

	freeaddrinfo(res);
	Client_Callback = fpCB;
	fpCB();
	return true;
}