#pragma once
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
		virtual const char *what() const noexcept;
	};
	// class Endpoint
}