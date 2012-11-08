#ifndef GENERIC_EXTRACTOR_H
#define GENERIC_EXTRACTOR_H

#include <QRunnable>

#include <string>
#include <zmq.hpp>

#include "common.h"

class Generic_Extractor : public QRunnable
{
	public:
		Generic_Extractor(void* z_context, const std::string& z_output_uri, const QString& file_path);

		virtual void	run() = 0;
		static regex_list	get_regexes();

	protected:
		zmq::context_t*	zmq_context;
		std::string	zmq_output_uri;

		QString		file;

		Exception	error;

		void	send_zmq(const QString& message);
};

#endif // GENERIC_EXTRACTOR_H
