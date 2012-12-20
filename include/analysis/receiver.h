#ifndef RECEIVER_H
#define RECEIVER_H

#include <QDebug>
#include <QThread>
#include <zmq.hpp>

#include "common.h"
#include "database.h"
#include "exception.h"

class Receiver : public QThread
{
	Q_OBJECT

	public:
		Receiver(void* z_context, Database* db);
		~Receiver();

		void	run();

	public slots:
		void	stop();

	private:
		zmq::context_t*	zmq_context;
		Database*	database;

		Exception	error;
		bool		can_run;
};

#endif // RECEIVER_H
