#include "printer.h"
#include "network.h"
using namespace Network;

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
	// Сервер стартует на порту 5555
	TCPEndpoint endpoint ("*", "5555");
	TCPEndpoint endpoint2 ("*", "5556");
	Socket socket = Socket::bind (endpoint.str(), context);
	Socket socket2 = Socket::bind (endpoint2.str(), context);
	Printer::error (socket.addr (), "EP1");
	Printer::error (socket2.addr (), "EP2");
	// Главный цикл
	while (1) {
		// Ждем сообщений от клиентов
		try {
			std::string msg = socket.recv();
			socket.send ("Привет от сервера, первый сокет!");
		}
		catch (Network::NoMessagesException &exc) {
			Printer::note ("Нет сообщений от первого сокета");
		}
		try {
			std::string msg = socket2.recv();
			socket2.send ("Привет от сервера, второй сокет!");
		}
		catch (Network::NoMessagesException &exc) {
			Printer::note ("Нет сообщений от второго сокета");
		}
		// Ждем сообщений
		sleep(2);
	}
	socket.close ();
	socket2.close ();
	Network::Context::destroyContext ();
	return 0;
}
