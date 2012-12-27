#include "extractors/generic_extractor.h"

Generic_Extractor::Generic_Extractor(void* z_context, const std::string& z_output_uri, const QString& file_path) : QRunnable()
{
	error.calling_method = "Generic_Extractor::Generic_Extractor";

	if ( z_context == NULL ) {
		error.msg = "z_context is NULL";

		throw error;;
	}

	if ( z_output_uri.empty() == true) {
		error.msg = "z_output_uri is empty";
		throw error;
	}

	if ( file_path.isEmpty() == true ) {
		error.msg = "file_path is empty";
		throw error;
	}

	zmq_context	= (zmq::context_t*) z_context;
	zmq_output_uri = z_output_uri;
	file = file_path;
}

void	Generic_Extractor::send_zmq(const QString& message) {
	if ( message.length() < 5 ) {
		qCritical() << error.calling_method << ": message 's length is too short: " << message;
		return;
	}

	zmq::socket_t	socket(*zmq_context, ZMQ_PUSH);
	zmq::message_t	z_message(message.size() + 1);

#ifdef WINDOWS_OS
    _snprintf((char*)z_message.data(), message.size() + 1, "%s", message.toLatin1().constData());
#else
    snprintf((char*)z_message.data(), message.size() + 1, "%s", message.toLatin1().constData());
#endif

	try {
		socket.connect(zmq_output_uri.c_str());
		socket.send(z_message);
	//} catch (const zmq::error_t& z_error) {
	} catch (const std::exception z_error) {
        qCritical() << error.calling_method << ": Cannot connect against " << zmq_output_uri.c_str() << ": " << z_error.what();
	}

	socket.close();
}
