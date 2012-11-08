#include "include/extractors/worker.h"

Worker::Worker(void* z_context, const std::string& input_uri, const std::string& output_uri)  : QThread() {
	error.calling_method = "Worker::Worker";

	if ( z_context == NULL ) {
		error.msg = "z_context is NULL";
		throw error;
	}

	if ( input_uri.empty() == true ) {
		error.msg = "input_uri is empty";
		throw error;
	}

	if ( output_uri.empty() == true ) {
		error.msg = "output_uri is empty";
		throw error;
	}

	zmq_context = (zmq::context_t*) z_context;
	z_input_uri = input_uri;
	z_output_uri = output_uri;

	can_run = true;
}

Worker::~Worker() {
	thread_pool.waitForDone();

	zmq_context = NULL;
}

void	Worker::run() {
	zmq::socket_t	z_input(*zmq_context, ZMQ_PULL);
	bool	connected = false;

	error.calling_method = "Worker::run";

	while ( connected == false ) {
		try {
			z_input.connect(z_input_uri.c_str());
			qDebug() << error.calling_method << " connected against " << z_input_uri.c_str();
			connected = true;
		} catch (const zmq::error_t z_error) {
			qCritical() << error.calling_method << ": Cannot connect against " << z_input_uri.c_str() << ": " << z_error.what();
			sleep(5);
		}
	}

	while (can_run == true) {
		zmq::message_t	z_message;
		QString		header;
		QString		file_path;

		z_input.recv(&z_message);
		header = static_cast<char*>(z_message.data());

		z_input.recv(&z_message);
		file_path = static_cast<char*>(z_message.data());

		if ( header.isEmpty() == false and file_path.isEmpty() == false ) {
			start_class(header, file_path);
		}
	}

	thread_pool.waitForDone();
	can_run = false;
}

void	Worker::stop() {
	can_run = false;
}

void	Worker::start_class(const QString& header, const QString& file_path) {
	Generic_Extractor*	thread = NULL;

	if ( header == "chrome" )
		thread = new Chrome_Extractor(zmq_context, z_output_uri, file_path);

	if ( header == "firefox" )
		thread = new Firefox_Extractor(zmq_context, z_output_uri, file_path);

	if ( thread != NULL )
		thread_pool.start(thread);
}
