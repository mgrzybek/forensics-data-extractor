#include "analysis/receiver.h"

Receiver::Receiver(void* z_context, const std::string& z_pull_uri, Database* db) : QThread()
{
	error.calling_method = "Receiver::Receiver";

	if ( z_context == NULL ) {
		error.msg = "z_context is NULL";
		throw error;
	}

	if ( z_pull_uri.empty() == true ) {
		error.msg = "z_pumm_uri is empty";
		throw error;
	}

	if ( db == NULL ) {
		error.msg = "db is NULL";
		throw error;
	}

	zmq_context = (zmq::context_t*) z_context;
	zmq_pull_uri = z_pull_uri;
	database = db;

	can_run = false;
}

Receiver::~Receiver() {
	zmq_context = NULL;
	database = NULL;
}

void	Receiver::run() {
	zmq::socket_t	socket(*zmq_context, ZMQ_PULL);
	bool			connected = false;

	while ( connected == false ) {
		try {
			// TODO: use several URI to bind
			socket.bind(zmq_pull_uri.c_str());
			connected = true;
			can_run = true;
			qDebug() << error.calling_method << ": socket binded on " << zmq_pull_uri.c_str();
		} catch (const zmq::error_t z_error) {
			qCritical() << error.calling_method << ": Cannot bind " << zmq_pull_uri.c_str() << ": " << z_error.what();
			sleep(5);
		}
	}

	while (can_run == true) {
		zmq::message_t	z_message;
		QString			string_message;

		socket.recv(&z_message);
		string_message = static_cast<char*>(z_message.data());

		qDebug() << error.calling_method << " received: " << string_message;

		if ( string_message.compare("END;") == 0 ) {
			stop();
		} else {
			if ( string_message.indexOf(ZMQ_QUERIES_SEPARATOR) == -1 )
				database->exec(string_message);
			else
				database->exec(string_message.split(ZMQ_QUERIES_SEPARATOR));
		}
	}

	socket.close();
}

void	Receiver::stop() {
	qDebug() << "Receiver::stop()";
	can_run = false;
}
