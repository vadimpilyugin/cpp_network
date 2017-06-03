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
	Network::Socket socket = Network::Socket::connect (Network::TCPEndpoint ("194.87.96.23", "5555").str());
	for (int i = 0; i < 5; i++) {
		// посылает сообщение
		socket.send (std::string("Привет серверу от клиента номер ") + std::to_string (i));
		// и ждет ответа
		std::string msg = socket.recv();
		sleep(1);
	}
	return 0;
}
