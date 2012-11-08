#include "fake_worker.h"

Fake_Worker::Fake_Worker(void* z_context, const char* listening_zmq_uri) : QThread() {
	zmq_context = (zmq::context_t*) z_context;
	zmq_uri = listening_zmq_uri;
}

Fake_Worker::~Fake_Worker() {
	zmq_context = NULL;
	zmq_uri.clear();
}

void	Fake_Worker::run() {
	zmq::socket_t	socket(*zmq_context, ZMQ_PULL);
	bool		connected = false;

	while ( connected == false ) {
		try {
			socket.connect(zmq_uri.c_str());
			connected = true;
		} catch (const zmq::error_t z_error) {
			sleep(5);
		}
	}

	while ( true ) {
		zmq::message_t	z_message;
		std::string	s_message;

		socket.recv(&z_message);
		s_message = static_cast<char*>(z_message.data());

		std::cout << s_message << std::endl;
	}

	socket.close();
}

