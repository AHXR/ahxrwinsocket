#pragma once

typedef void(*FPCLIENT_CB) ();
typedef void(*FPC_CALLBACK) (SOCKET clientSocket, addrinfo* addressInfo);
typedef void(*FPS_CALLBACK) (SOCKET clientSocket, addrinfo * addressInfo, char * data);
typedef void(*FP_RES) (char * recData);
typedef void(*FPCLIENT_T) (bool result, const char * dataSent);

typedef addrinfo * CLIENTDATA;

#define					AHXRSOCKET_PREFIX				"AHXRSOCKET:"
#define					AHXRSOCKET_CLIENT_PREFIX		"AHXRSOCKET CLIENT:"

#ifndef DEFAULT_BUFF
	#define				DEFAULT_BUFF					512
#endif

#define					AHXRCLIENT						__AHXRSOCKET__::Client
#define					LPAHXRCLIENT					__AHXRSOCKET__::Client *
#define					AHXRSERVER						__AHXRSOCKET__::Server
#define					LPAHXRSERVER					__AHXRSOCKET__::Server *

#define					TCP_SERVER						IPPROTO_TCP
#define					IP_SERVER						IPPROTO_IP
#define					IPV6_SERVER						IPPROTO_IPV6
#define					RM_SERVER						IPPROTO_RM
#define					UDP_SERVER						IPPROTO_UDP