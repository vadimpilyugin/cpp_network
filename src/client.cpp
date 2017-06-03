#include "printer.h"
#include "network.h"

#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>

#define sleep(n)	Sleep(n)
#endif

int main() {
	Printer::debug ("Программа для связи двух точек");
	// Клиент подключается к серверу на порту 5555
	Network::Socket socket = Network::Socket::connect (Network::TCPEndpoint ("127.0.0.1", "5555").str());
	// посылает сообщение
	socket.send ("Hi! How are you doin'?");
	// и ждет ответа
	std::string msg = socket.recv();
	sleep(2);
	socket.send ("May i have a look on your ID card?");
	msg = socket.recv();
	sleep(2);
	return 0;
}