/**
 * Project: forensics-data-extractor
 * File name: node.cpp
 * Description: the main function of the headless worker
 *
 * @author Mathieu Grzybek on 2012-11-05
 * @copyright 2012 Mathieu Grzybek. All rights reserved.
 * @version $Id: code-gpl-license.txt,v 1.2 2004/05/04 13:19:30 garry Exp $
 *
 * @see The GNU Public License (GPL) version 3 or higher
 *
 *
 * forensics-data-extractor is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "include/node.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("Forensics-Data-Extractor");
	QCoreApplication::setApplicationName("Worker");

	zmq::context_t	zmq_context(1);

	QString	config_file;
	bool	daemonize = false;

	if ( argc < 4 ) {
		usage();
		return EXIT_FAILURE;
	}

	for (int i = 1 ; i < argc ; i++ ) {
		if ( strcmp(argv[i], "-f") == 0 ) {
			config_file = argv[i+1];
		}
		if ( strcmp(argv[i], "-d") == 0 ) {
			daemonize = true;
		}
	}

	if ( config_file.isEmpty() == true ) {
		usage();
		return EXIT_FAILURE;
	}

	try {
		Node	node(&zmq_context, config_file, daemonize);

		node.run();

	} catch (const Exception& error) {
		return EXIT_FAILURE;
	} catch (const std::exception& error) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

Node::Node(void* z_context, const QString& config_file, bool daemonize) {
	/*
	 * Let's get the ports to bind and start the worker
	 */
	// Get the settings
	QSettings	settings(config_file, QSettings::IniFormat);

	if ( z_context == NULL ) {
		error.msg = "z_context is NULL";
		throw error;
	}

	if ( settings.contains("zmq_input_uri") == false ) {
		error.msg = "Cannot find zmq_input_uri in settings";
		throw error;
	}

	if ( settings.contains("zmq_output_uri") == false ) {
		error.msg = "Cannot find zmq_output_uri in settings";
		throw error;
	}

	if ( settings.contains("lock_file") == false ) {
		error.msg = "Cannot find lock_file in settings";
		throw error;
	}

	std::string	zmq_input_uri = settings.value("zmq_input_uri").toString().toAscii().constData();
	std::string	zmq_output_uri = settings.value("zmq_output_uri").toString().toAscii().constData();

	lock_file = settings.value("lock_file").toString().toAscii().constData();
	daemon = daemonize;

	zmq_context = (zmq::context_t*) z_context;

	worker = new Worker(&zmq_context, zmq_input_uri, zmq_output_uri);

	/*
	 * Signals Handling
	 */
	if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sighupFd))
		qFatal("Couldn't create HUP socketpair");

	if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigtermFd))
		qFatal("Couldn't create TERM socketpair");

	snHup = new QSocketNotifier(sighupFd[1], QSocketNotifier::Read, this);
	connect(snHup, SIGNAL(activated(int)), this, SLOT(handleSigHup()));

	snTerm = new QSocketNotifier(sigtermFd[1], QSocketNotifier::Read, this);
	connect(snTerm, SIGNAL(activated(int)), this, SLOT(handleSigTerm()));
}

Node::~Node() {
	if ( worker != NULL ) {
		emit stop_worker();
		worker->wait();

		delete worker;
		worker = NULL;
	}
}

void	Node::run() {
	/*
	 * Deal with the signals and daemonize
	 */
	if ( daemon == true )
		daemonize();

	/*
	 * Start the worker
	 */
	connect(this, SIGNAL(stop_worker()), worker, SLOT(stop_worker()));
	worker->start();
	worker->wait();
}

void Node::hupSignalHandler(int) {
	char a = 1;
	::write(sighupFd[0], &a, sizeof(a));
}

void Node::termSignalHandler(int) {
	char a = 1;
	::write(sigtermFd[0], &a, sizeof(a));
}

void Node::handleSigTerm() {
	snTerm->setEnabled(false);
	char tmp;
	::read(sigtermFd[1], &tmp, sizeof(tmp));

	// do Qt stuff
	emit stop_worker();

	snTerm->setEnabled(true);
}

void Node::handleSigHup() {
	snHup->setEnabled(false);
	char tmp;
	::read(sighupFd[1], &tmp, sizeof(tmp));

	// do Qt stuff
	emit stop_worker();

	snHup->setEnabled(true);
}

void	Node::daemonize() {
	int		result;
	std::ofstream	f;

	result = fork();
	if (result < 0)
		exit(1); /* fork error */
	if (result > 0)
		exit(0); /* parent exits */

	/* child (daemon) continues */
	setsid(); /* obtain a new process group */

	for (result = getdtablesize() ; result >= 0 ; --result)
		close(result); /* close all descriptors */

	chdir("/tmp"); /* change running directory */

	/* first instance continues */
	f.open(lock_file.c_str());
	f << getpid();
	f.close();
}

static int setup_unix_signal_handlers() {
	struct sigaction hup, term;

	hup.sa_handler = Node::hupSignalHandler;
	sigemptyset(&hup.sa_mask);
	hup.sa_flags = 0;
	hup.sa_flags |= SA_RESTART;

	if (sigaction(SIGHUP, &hup, 0) > 0)
		return 1;

	term.sa_handler = Node::termSignalHandler;
	sigemptyset(&term.sa_mask);
	term.sa_flags |= SA_RESTART;

	if (sigaction(SIGTERM, &term, 0) > 0)
		return 2;

	return 0;
}
