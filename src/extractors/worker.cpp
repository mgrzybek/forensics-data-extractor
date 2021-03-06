#include "extractors/worker.h"

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

Worker::Worker(void* z_context, Database* db, const std::string& output_uri) : QThread() {
	error.calling_method = "Worker::Worker";

	if ( z_context == NULL ) {
		error.msg = "z_context is NULL";
		throw error;
	}

	if ( output_uri.empty() == true ) {
		error.msg = "output_uri is empty";
		throw error;
	}

	if ( db == NULL ) {
		error.msg = "db is NULL";
		throw error;
	}

	zmq_context = (zmq::context_t*) z_context;
	analysis_db = db;

	can_run = true;
}

Worker::~Worker() {
	thread_pool.waitForDone();

	zmq_context = NULL;
}

void	Worker::run() {
	if ( analysis_db == NULL )
		run_zmq();
	else
		run_database();

	thread_pool.waitForDone();
	can_run = false;
}

void	Worker::stop() {
	qDebug() << "Worker::stop()";
	can_run = false;
}

void	Worker::refresh_models() {
	qDebug() << "Worker::refresh_models()";
	emit refresh_models();
}

void	Worker::start_class(const QString& header, const QString& file_path) {
	Generic_Extractor*	thread = NULL;

	if ( header == "chrome" )
		thread = new Chrome_Extractor(zmq_context, z_output_uri, file_path);

	if ( header == "firefox" )
		thread = new Firefox_Extractor(zmq_context, z_output_uri, file_path);

	if ( header == "internet_explorer" )
		thread = new Internet_Explorer_Extractor(zmq_context, z_output_uri, file_path);

	if ( thread != NULL )
		thread_pool.start(thread);
}

void	Worker::run_zmq() {
	zmq::socket_t	z_input(*zmq_context, ZMQ_PULL);
	bool		connected = false;

	error.calling_method = "Worker::run_zmq";

	qDebug() << error.calling_method << "started";

	while ( connected == false ) {
		try {
			// TODO: use several URI to connect against
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

		if ( header.isEmpty() == false && file_path.isEmpty() == false ) {
			start_class(header, file_path);
		}
	}

	qDebug() << error.calling_method << "completed";
}

void	Worker::run_database() {
	error.calling_method = "Worker::run_database";
	struct_file	file;

	qDebug() << error.calling_method << "started";

	while ( analysis_db->walk_files_to_extract(file) == true && can_run == true ) {
		if ( file.extractor.isEmpty() == false && file.full_path.isEmpty() == false ) {
			start_class(file.extractor, file.full_path);
		}
	}

	qDebug() << error.calling_method << "completed";
}
