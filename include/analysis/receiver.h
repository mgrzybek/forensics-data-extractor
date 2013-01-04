#ifndef RECEIVER_H
#define RECEIVER_H

#include <QDebug>
#include <QThread>
#include <zmq.hpp>

#include "common.h"
#include "database.h"

class Receiver : public QThread
{
	Q_OBJECT

	public:
		Receiver(void* z_context, const std::string& z_pull_uri, Database* db);
		~Receiver();

		void	run();

	public slots:
		void	stop();

	private:
		zmq::context_t*	zmq_context;
		std::string	zmq_pull_uri;
		Database*	database;

		Exception	error;
		bool		can_run;
};

#endif // RECEIVER_H
