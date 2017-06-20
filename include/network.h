#pragma once
#include "zmq.hpp"
#include "printer.h"
#include <exception>
#include <string>

namespace Network {
	/*
	* Здесь находятся все, что относится к работе с сетью
	*/
	class Exception: public std::exception {
		std::string msg;
	public:
		Exception(const std::string _msg): msg(_msg) {}
		virtual const char *what() const noexcept {
			return msg.c_str();
		}
	};
	// Два send подряд или два receive подряд
	// Порядок должен быть строго по очереди
	class WrongOrderException: public Exception {
		using Network::Exception::Exception;
	};
	// Невозможно послать сообщение в данный момент. Может буфер переполнен, может сеть отвалилась
	class CannotSendException: public Exception {
		using Network::Exception::Exception;
	};
	// Нет готовых сообщений, чтобы принять. Нужен в случае неблокирующего приема
	class NoMessagesException: public Exception {
		using Network::Exception::Exception;
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
	/*
	* Единственное средство связи между игроками. Представляет собой
	* одновременно и точку приема, и точку передачи. Прием и передача
	* строго чередуются: нельзя дважды отправить сообщение или принять его.
	*/
	class Socket {
	public:
		// Возвращает сокет, привязанный к точке
		static Socket bind (const std::string &endpoint) 
		throw 	(Exception);
		Socket bind (const Endpoint &endpoint) 
		throw (Exception);
		// Возвращает сокет, подключенный к удаленной точке
		static Socket connect (const std::string &endpoint)
		throw 	(Exception);
		// Деструктор
		~Socket();
		// вернет адрес, к которому привязан сокет
		std::string addr() const 
		throw 	(Exception);
		// посылает строку на другой сокет
		void send(const std::string msg) 
		throw (
			Exception,
			WrongOrderException,
			CannotSendException,
			Printer::AssertException
		);
		// принимает строку от другого сокета
		std::string recv(bool nonblock = false) 
		throw (
			Exception,
			WrongOrderException, 
			NoMessagesException
		);
		Socket (Socket &&other) {
			is_bound = other.is_bound;
			context_p = other.context_p;
			socket_p = other.socket_p;
			other.context_p = nullptr;
			other.socket_p = nullptr;
			Printer::debug ("Сокет был передан в другие руки");
		}
	private:
		Socket (zmq::context_t *context_ = nullptr, zmq::socket_t *socket_ = nullptr, const bool is_bound_ = false): 
			context_p(context_),
			socket_p(socket_),
			is_bound(is_bound_) {}
		zmq::context_t *context_p;
		zmq::socket_t *socket_p;
		bool is_bound; // сокет привязан к точке(true) или подключается к ней(false)
	};
}
