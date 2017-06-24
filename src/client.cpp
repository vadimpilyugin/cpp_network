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
	// Создаем контекст
	zmq::context_t *context = Network::Context::createContext ();
	// Клиент подключается к серверу на порту 5555
	// Network::TCPEndpoint ("194.87.96.23", "5555")
	// 194.87.96.23
	// 127.0.0.1
	// 192.168.1.236
	Network::Socket socket = Network::Socket::connect (Network::TCPEndpoint ("194.87.96.23", "5555").str(), context);
	Network::Socket socket2 = Network::Socket::connect (Network::TCPEndpoint ("127.0.0.1", "5556").str(), context);
	// for (int i = 0; i < 5; i++) {
		// посылает сообщение
		socket.send (std::string ("Client #1: Hello"));
		socket2.send (std::string ("Client #2: Hello"));
		// и ждет ответа
		bool first_recv = true, second_recv = false;
		while (! (first_recv && second_recv)) {
			if (!first_recv)
				try {
					std::string msg = socket.recv ();
					first_recv = true;
				}
				catch (Network::NoMessagesException &exc) {
					Printer::note ("Нет сообщений от первого сервера");
				}
			if (!second_recv)
				try {
					std::string msg = socket2.recv ();
					second_recv = true;
				}
				catch (Network::NoMessagesException &exc) {
					Printer::note ("Нет сообщений от второго сервера");
				}
			sleep(1);
		}
	// }
	Printer::error ("Foo");
	socket.close ();
	socket2.close ();
	Printer::error ("Bar");
	Network::Context::destroyContext ();
	Printer::error ("Baz");
	return 0;
}
