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
	TCPEndpoint endpoint ("127.0.0.1", "5555");
	Socket socket = Socket::bind (endpoint.str());
	// Ждем сообщений от клиентов
	std::string msg = socket.recv();
	// Получили сообщение, делаем работу
	sleep(2);
	// Посылаем ответ
	socket.send ("I'm okay, sir");
	msg = socket.recv();
	sleep(2);
	socket.send ("Yeah, sure");
	return 0;
}