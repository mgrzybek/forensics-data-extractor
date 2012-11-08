#ifndef WORKER_H
#define WORKER_H

#include <QThreadPool>
#include <QString>

#include <zmq.hpp>
#include <string>

#include "extractors/generic_extractor.h"
#include "extractors/chrome_extractor.h"
#include "extractors/firefox_extractor.h"
#include "exception.h"

typedef	QHash<QString, QRunnable*>	h_extractors;

class Worker : public QThread
{
	public:
		Worker(void* z_context, const std::string& input_uri, const std::string& output_uri);
		~Worker();

		void	run();

	public slots:
		void	stop();

	private:
		/*
		 * ZMQ stuff
		 */
		zmq::context_t*		zmq_context;
		zmq::socket_t*		z_input;
		std::string			z_input_uri;
		std::string			z_output_uri;

		/*
		 * Threads stuff
		 */
		QThreadPool		thread_pool;
		bool			can_run;
		h_extractors	available_extractors;

		Exception			error;

		void	start_class(const QString& header, const QString& file_path);
};

#endif // WORKER_H
