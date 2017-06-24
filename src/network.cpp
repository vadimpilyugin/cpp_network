#define NDEBUG
#include "network.h"

using namespace Network;

zmq::context_t *Context::context_p = nullptr;

zmq::context_t *Context::createContext () throw (Exception::Exception) {
	if (context_p != nullptr)
		return context_p;
	else {
		try {
			// создаем контекст
			context_p = new zmq::context_t (1);
			return context_p;
		}
		catch (zmq::error_t &error) {
			Printer::error ("Не удалось создать контекст");
			throw Exception::Exception (error.what ());
		}
	}
	
}

void Context::destroyContext () {
	if (context_p != nullptr) {
		delete context_p;
		context_p = nullptr;
	}
	else 
		Printer::error ("Попытка уничтожить несуществующий контекст!");
}

zmq::context_t *Context::getContext () {
	return context_p;
}

Socket Socket::bind (const std::string &endpoint, zmq::context_t *context_p) 
throw (Exception::Exception)
{
	zmq::socket_t *socket_p = nullptr;
	try {
		// нужно привязаться к данной точке
		// создаем сокет REP
		socket_p = new zmq::socket_t(*context_p, ZMQ_REP);
		// пытаемся привязать сокет
		socket_p -> bind (endpoint);
	}
	catch (zmq::error_t &error) {
		Printer::error (error.what(), "Не удалось привязать сокет");
		Printer::debug (endpoint, "Endpoint");
		// очистить уже выделенные ресурсы
		if(socket_p != nullptr)
			delete socket_p;
		// выкинуть исключение
		Printer::debug("Hello, world");
		throw Exception::Exception (error.what());
	}
	Printer::debug ("Сокет был создан и привязан");
	Printer::debug (endpoint, "Endpoint");
	return Socket (socket_p, true);
}

Socket Socket::bind (const Endpoint &endpoint, zmq::context_t *context_p) 
throw (Exception::Exception)
{
	zmq::socket_t *socket_p = nullptr;
	try {
		// нужно привязаться к данной точке
		// создаем сокет REP
		socket_p = new zmq::socket_t(*context_p, ZMQ_REP);
		// пытаемся привязать сокет
		socket_p -> bind (endpoint.str());
	}
	catch (zmq::error_t &error) {
		Printer::error (error.what(), "Не удалось привязать сокет");
		Printer::debug (endpoint.str(), "Endpoint");
		// очистить уже выделенные ресурсы
		if(socket_p != nullptr)
			delete socket_p;
		// выкинуть исключение
		Printer::debug("Hello, world");
		throw Exception::Exception (error.what());
	}
	Printer::debug ("Сокет был создан и привязан");
	Printer::debug (endpoint.str(), "Endpoint");
	return Socket (socket_p, true);
}

Socket Socket::connect (const std::string &endpoint, zmq::context_t *context_p)
throw 	(Exception::Exception)
{
	zmq::socket_t *socket_p = nullptr;
	try {
		// в этом случае нужно подключиться к указанной точке
		// создаем сокет REQ
		socket_p = new zmq::socket_t (*context_p, ZMQ_REQ);
		socket_p -> connect (endpoint);
	}
	catch (zmq::error_t &error) {
		Printer::error (error.what(), "Не удалось подключиться");
		Printer::error (endpoint, "Endpoint");
		// очистить уже выделенные ресурсы
		if(socket_p != nullptr)
			delete socket_p;
		// выкинуть исключение
		throw Exception::Exception (error.what());
	}
	Printer::debug ("Сокет был создан и подключен");
	Printer::debug (endpoint, "Endpoint");
	return Socket (socket_p, false);
}

void Socket::close () {
	// очистить выделенные ресурсы
	if(socket_p != nullptr) {
		delete socket_p;
		socket_p = nullptr;
	}
	else
		Printer::note("Сокет уже закрыт!", "Socket");
	
}

Socket::~Socket() {
	close ();
	Printer::debug("Сокет был уничтожен");
}

std::string Socket::addr() const
throw 	(Exception::Exception)
{
	if(!is_bound) {
		Printer::error("Сокет не был привязан ни к одной точке", "Socket.addr");
		throw Exception::Exception ("Сокет не имеет привязанного адреса");
	}
	// заводим буфер
	size_t buflen = 1000;
	char *buffer = new char [buflen];
	try {
		// вызываем getsockopt
		socket_p -> getsockopt (ZMQ_LAST_ENDPOINT, buffer, &buflen);
	}
	catch (zmq::error_t &error) {
		delete [] buffer;
		Printer::error (error.what(), "Socket.getsockopt");
		throw Exception::Exception (error.what());
	}
	// записываем в std::string
	std::string result(buffer);
	// очищаем память
	delete [] buffer;
	return result;
}

void Socket::send(const std::string msg) 
throw (
	Exception::Exception,
	WrongOrderException,
	CannotSendException,
	Printer::AssertException
)
{
	try {
		// создаем сообщение
		zmq::message_t request (msg.length());
		// копируем символы из строки в буфер
		memcpy (request.data(), msg.c_str(), msg.length());
		Printer::debug(msg, "Посылаем сообщение");
		// пытаемся послать сообщение
		bool is_sent = socket_p -> send(request);
		if (!is_sent) {
			// повторить позже
			Printer::error("Не получилось послать сообщение", "Socket.send");
			throw CannotSendException (zmq_strerror(zmq_errno()));
		}
		Printer::debug ("Сообщение отправлено!", "Socket.send");
	}
	catch (zmq::error_t &error) {
		if(error.num() == EFSM) {
			// неправильная очередность
			Printer::error("Неправильный порядок запросов-ответов", "Socket.send");
			throw WrongOrderException (error.what());
		}
		Printer::error(error.what(), "Произошла ошибка при отправке");
		throw Exception::Exception (error.what());
	}
}

std::string Socket::recv(bool nonblock) 
throw (
	Exception::Exception,
	WrongOrderException, 
	NoMessagesException
)
{
	try {
		// создаем сообщение
		zmq::message_t reply;
		// пытаемся принять
		bool is_recv;
		if (nonblock)
			is_recv = socket_p -> recv (&reply, ZMQ_DONTWAIT);
		else
			is_recv = socket_p -> recv (&reply);
		if (!is_recv) {
			// сейчас нет сообщений, повторить позже
			Printer::note("Нет сообщений в данный момент", "Socket.recv");
			throw NoMessagesException (zmq_strerror(zmq_errno()));
		}
		// заводим буфер для данных и 0 в конце
		char *reply_data = new char[reply.size()+1];
		// копируем сообщение в буфер
		memcpy (reply_data, reply.data(), reply.size());
		// ставим 0 в конце
		reply_data[reply.size()] = 0;
		// по буферу строим std::string
		std::string reply_string (reply_data);
		Printer::debug(reply_string, "Получено сообщение");
		delete [] reply_data;
		return reply_string;
	}
	catch (zmq::error_t &error) {
		if(error.num() == EFSM) {
			// неправильная очередность
			Printer::error ("Неправильный порядок запросов-ответов", "Socket.recv");
			throw WrongOrderException (error.what());
		}
		Printer::error(error.what(), "Произошла ошибка при приеме");
		throw Exception::Exception (error.what());
	}
}