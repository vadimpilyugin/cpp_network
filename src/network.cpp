#include "network.h"
#include "zmq.h"

const char *Network::Exception::what() const noexcept {
	return msg.c_str();
}