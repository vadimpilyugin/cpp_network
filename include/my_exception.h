#pragma once
#include <exception>
#include <string>

namespace Exception {
	class Exception: public std::exception {
		std::string msg;
	public:
		Exception(const std::string _msg): msg(_msg) {}
		virtual const char *what() const noexcept {
			return msg.c_str();
		}
	};
}