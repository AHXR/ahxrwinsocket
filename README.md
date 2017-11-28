# ahxrwinsocket - Light C++ Winsock Wrapper Library

```c++
// client_example.cpp
#include				"ahxrwinsock.h"
#include				<string>

using namespace	std;
void onClientConnect();
void onClientReceiveData( char * data );
void onClientSendData( bool result, const char * dataSent );

int main()
{
	string		str_host;
	string		str_port;
	AHXRCLIENT  MyClient;
	bool		b_res;

	cout		<< "[ahxrwinsock client example (TCP)]\n\nEnter the host address:\nHost: ";
	cin			>> str_host;

	cout		<< "\nNow enter the port #:\nPort: ";
	cin			>> str_port;

	b_res		= MyClient.init(str_host.c_str(), str_port.c_str(), IPPROTO_TCP, onClientConnect);
	cout		<< "Please wait...\n";

	// Giving the client time to connect.
	Sleep(500);

	if (!b_res) {
		cout << "Failed to connect!";
		exit(0);
	}

	string		str_buffer;
	MyClient.listen(onClientReceiveData);

	while (1) {
		cout	<< "\nEnter a message you would like to send to the server! Type 'exit' when you want to leave!\n";
		cin		>> str_buffer;

		if (str_buffer.compare( "exit" ) == 0) {
			MyClient.close();
			exit(0);
		}

		MyClient.send_data( str_buffer.c_str() );
	}

	system("pause");
    return 0;
}

void onClientConnect() {
	....
}

void onClientSendData( bool result, const char * dataSent ) {
	....
}

void OnClientReceiveData( char * data ) {
	....
}
```

## Install (Windows)
Simply copy the header files to your project's build directory.

---

## Server Management

```c++
AHXRSERVER MyServerName;
MyServerName.start_server( "7777", TCP_SERVER );
```

Creating a server is simply done like the example above. The function takes a port and protocol that it can use. You are allowed to use any protocol that is available with Winsock. Below are some
ahxrsocket defined macros for simplicity purposes:

| Protocol | Macro |
| :---:    | :---: |
| TCP | TCP_SERVER |
| IP | IP_SERVER |
| IPV6 | IPV6_SERVER |
| RM | RM_SERVER |
| UDP | UDP_SERVER |

Server control can be managed with the methods below:

```c++
void start_server(PCSTR port, int protocol, FPC_CALLBACK fpConnected=0, FPS_CALLBACK fpData=0, bool thread = true);
void send_data(SOCKET clientSocket, const char * sendChar);
void close();
```

The example earlier showed how simple start_server is; however there are 3 optional arguments. *fpConnected* and *fpData* introduce callbacks for the server. The final argument *thread* controls whether
you want the server to run as a multi-threaded function. The callbacks are important for the server because it allows you to gather information from any client that connects to it. See **server_example.cpp** for a 
complete example.

```c++
void onServerClientConnect( CLIENTDATA info );
void onServerReceiveData(SOCKET clientSocket, CLIENTDATA info, char * data);

int main( )
{
	MyServer.start_server
	( 
		s_port.c_str(), 
		TCP_SERVER, 
		onServerClientConnect, 
		onServerReceiveData,
		false // Letting the server control this thread - true by default
	);
	return 0;
}

void onServerClientConnect( CLIENTDATA info ) { // CLIENTDATA is another keyword for "addrinfo *"
	....
}

void onServerReceiveData(SOCKET clientSocket, CLIENTDATA info, char * data) {
	....
}
```

---

## Client Management

```c++
AHXRCLIENT  MyClient;
MyClient.init( "my_host_name", "7777", TCP_SERVER, onClientConnect);
```

Creating a client works similiar to creating a server. It's done with a few simple steps. Unlike the server, the onClientConnect argument is not optional. AHXRCLIENT can be used to connect to other
hosts outside of ahxrwinsocket. Client control can be managed with the methods below:

```c++
bool init( PCSTR server, PCSTR port, int protocol, FPCLIENT_CB fpCB );
bool send_data( const char * sendChar );
void listen( FP_RES fpRes, bool thread = true );
void close();
```

**init** takes a callback argument by FPCLIENT_CB. This callback will trigger once the client has connected to the server. The send_data function is self-explanatory. However, I've created separate methods
that control whether you want a callback to used once the data is sent. By default, the client does not automatically listen for send that is sent from the server. This is why the listen function is here. 
This function accepts a response callback and is threaded by default. Example (see client_example for a full example):

```c++
void onClientConnect();
void onClientSendData( bool result, const char * dataSent );
void OnClientReceiveData( char * data );

int main() {
	AHXRCLIENT  MyClient;
	bool b_res = MyClient.init( "my_host_name", "7777", TCP_SERVER, onClientConnect);
	
	// Giving the client time to connect.
	Sleep(500);
	
	if( !b_res ) 
	{
		puts("Unable to connect to server");
		exit(0);
	}
	
	MyClient.toggle_send_callback( true );
	MyClient.set_toggle_callback( onClientSendData );
	
	MyClient.send_data( "Hello" );
	MyClient.listen( OnClientReceiveData, false ); // Purposely single-threading this for a response.
	return 0;
}

void onClientConnect() {
	....
}

void onClientSendData( bool result, const char * dataSent ) {
	if( !result )
		printf("Couldn't send %s\n", dataSent );
	else
		puts("Data sent!");
}

void OnClientReceiveData( char * data ) {
	printf("Server says %s\n", data );
}
```

## Buffer

If you plan on sending larger data, define DEFAULT_BUFF before including this library. Or you can redefine DEFAULT_BUFF later on. By default, DEFAULT_BUFF = 512. 