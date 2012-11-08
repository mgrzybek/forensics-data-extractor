#include <zmq.hpp>
#include <iostream>

#include <QThread>

class Fake_Worker : public QThread {
	Q_OBJECT

	public:
		Fake_Worker(void* z_context, const char* listening_zmq_uri);
		~Fake_Worker();

		void	run();

	private:
		zmq::context_t*	zmq_context;
		std::string	zmq_uri;
};

