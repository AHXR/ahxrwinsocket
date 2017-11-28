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
#include				"stdafx.h"
#include				"ahxrwinsock.h"
#include				<string>

using namespace			std;
using namespace			System;

void					onClientConnect();
void					onClientReceiveData(char * data);

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
	cout << "We have connected to the host!" << endl;
}

void onClientReceiveData(char * data) {
	cout << "\nThe server says: " << data << endl;
}