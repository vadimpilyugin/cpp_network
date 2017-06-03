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
	// Сервер стартует на порту 5555
	TCPEndpoint endpoint ("*", "5555");
	Socket socket = Socket::bind (endpoint.str());
	// Главный цикл
	while (1) {
		// Ждем сообщений от клиентов
		std::string msg = socket.recv();
		// Получили сообщение, делаем работу
		sleep(1);
		// Посылаем ответ
		socket.send ("Привет от openplatform.me!");
	}
	return 0;
}
