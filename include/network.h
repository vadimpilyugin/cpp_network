#pragma once
#include "zmq.hpp"
#include "printer.h"
#include "my_exception.h"
#include <string>

namespace Network {
	/*
	* Здесь находятся все, что относится к работе с сетью
	*/
	// Два send подряд или два receive подряд
	// Порядок должен быть строго по очереди
	class WrongOrderException: Exception::Exception {
		using Exception::Exception;
	};
	// Невозможно послать сообщение в данный момент. Может буфер переполнен, может сеть отвалилась
	class CannotSendException: Exception::Exception {
		using Exception::Exception;
	};
	// Нет готовых сообщений, чтобы принять. Нужен в случае неблокирующего приема
	class NoMessagesException: Exception::Exception {
		using Exception::Exception;
	};
	/*
	* Конечная точка это уникальный адрес
	*/
	struct Endpoint {
		// сериализация
		virtual std::string str() const noexcept { return std::string(); }
		// виртуальный деструктор
		virtual ~Endpoint() {}
	};
	struct IPCEndpoint: public Endpoint {
		std::string pathname;
		IPCEndpoint(const std::string pathname_ = std::string("*")): pathname(pathname_) {}
		virtual std::string str() const noexcept {
			return std::string("ipc://") + pathname;
		}
	};
	struct TCPEndpoint {
		std::string port_number;
		std::string ip_addr;
		TCPEndpoint(const std::string ip_addr_ = std::string("*"), const std::string port_number_ = std::string("*")): 
			port_number(port_number_), 
			ip_addr(ip_addr_) {}
		virtual std::string str() const noexcept {
			return std::string("tcp://") + ip_addr + std::string(":") + port_number;
		}
	};
	// Класс, хранящий контекст для всех сокетов
	class Context {
	public:
		static zmq::context_t *createContext () throw (Exception::Exception);
		static void destroyContext ();
		static zmq::context_t *getContext ();
	private:
		static zmq::context_t *context_p;
	};

	/*
	* Единственное средство связи между игроками. Представляет собой
	* одновременно и точку приема, и точку передачи. Прием и передача
	* строго чередуются: нельзя дважды отправить сообщение или принять его.
	*/
	class Socket {
	public:
		// Возвращает сокет, привязанный к точке
		static Socket bind (const std::string &endpoint, zmq::context_t *context) 
		throw 	(Exception::Exception);
		Socket bind (const Endpoint &endpoint, zmq::context_t *context) 
		throw (Exception::Exception);
		// Возвращает сокет, подключенный к удаленной точке
		static Socket connect (const std::string &endpoint, zmq::context_t *context)
		throw 	(Exception::Exception);
		// Деструктор
		~Socket();
		// вернет адрес, к которому привязан сокет
		std::string addr() const 
		throw 	(Exception::Exception);
		// посылает строку на другой сокет
		void send(const std::string msg) 
		throw (
			Exception::Exception,
			WrongOrderException,
			CannotSendException,
			Printer::AssertException
		);
		// принимает строку от другого сокета
		std::string recv(bool nonblock = true) 
		throw (
			Exception::Exception,
			WrongOrderException, 
			NoMessagesException
		);
		// Закрывает сокет, аналогично деструктору
		void close ();
		Socket (Socket &&other) {
			is_bound = other.is_bound;
			socket_p = other.socket_p;
			other.socket_p = nullptr;
			Printer::debug ("Сокет был передан в другие руки");
		}
	private:
		Socket (zmq::socket_t *socket_ = nullptr, const bool is_bound_ = false): 
			socket_p(socket_),
			is_bound(is_bound_) {}
		zmq::socket_t *socket_p;
		bool is_bound; // сокет привязан к точке(true) или подключается к ней(false)
	};
}
