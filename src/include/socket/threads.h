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
DWORD WINAPI __AHXRSOCKET__::__t__listenThread(LPVOID lpParams) {
	LPAHXRCLIENT
		c_listen = (LPAHXRCLIENT)lpParams
	;
	c_listen->__listenfunc(c_listen->fp_params);
	return 0;
}

DWORD WINAPI __AHXRSOCKET__::__t__createServerRes(LPVOID lpParams) {
	int							i_result;
	char						c_rec[DEFAULT_BUFF];

	__AHXRSOCKET__::s_server_res * Socket_Response = (__AHXRSOCKET__::s_server_res *)lpParams;
	do {
		i_result = recv(Socket_Response->SocketResponse, c_rec, DEFAULT_BUFF, 0);
		if (i_result > 0) {
			if (i_result < DEFAULT_BUFF - 1)
				c_rec[i_result] = '\0';

			if(Socket_Response->fpResData != NULL)
				Socket_Response->fpResData( Socket_Response->SocketResponse, Socket_Response->addrRes, c_rec);
		}
		else if (i_result == 0)
#ifdef AHXRLOGGER_PLUGIN
			WARNING("%s Connection closed.", AHXRSOCKET_PREFIX);
#else
			printf("%s Connection closed.", AHXRSOCKET_PREFIX);
#endif
		else
			if (WSAGetLastError() != WSAESHUTDOWN)
#ifdef AHXRLOGGER_PLUGIN
				WARNING("%s Issue with server response %d", AHXRSOCKET_PREFIX, WSAGetLastError());
#else
				printf("%s Issue with server response %d", AHXRSOCKET_PREFIX, WSAGetLastError());
#endif


	} while (i_result > 0);

	return 0;
}