/**
 * Project: forensics-data-extractor
 * File name: extraction.cpp
 * Description: describes the command line tool to extraction intelligence
 * from an image or a directory
 *
 * @author Mathieu Grzybek on 2013-01-02
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

#include "extraction.h"

int	main(int argc, char *argv[]) {
	QCoreApplication::setOrganizationName("Forensics-Data-Extractor");
	QCoreApplication::setApplicationName("Extractor");

	QThreadPool	thread_pool;
	QString		db_file;
	zmq::context_t	zmq_context(1);

	if ( argc < 3 ) {
		usage();
		return EXIT_FAILURE;
	}

	if ( strcmp(argv[1], "-d" ) == 0 ) {
		db_file = argv[2];
	} else {
		usage();
		return EXIT_FAILURE;
	}

	Database	database(db_file);
	Receiver	receiver((void*)&zmq_context, ZMQ_INPROC_URL, &database);
	Worker		worker((void*) &zmq_context, &database, ZMQ_INPROC_URL);

	try {
		struct_file	file;

		receiver.start();

		while ( database.walk_files_to_extract(file) == true ) {
			worker.start();
			worker.wait();
		}
	} catch (std::exception& e) {
		std::cerr << e.what();
		return EXIT_FAILURE;
	}

	send_zmq(zmq_context, ZMQ_INPROC_URL, ZMQ_PUSH, "END;");
//	thread_pool.waitForDone();
	receiver.wait();

	std::cout << database.get_row_count("analysed_file") << " files analysed" << std::endl;
	std::cout << thread_pool.maxThreadCount() << " threads used" << std::endl;

	return EXIT_SUCCESS;
}

void	send_zmq(zmq::context_t& zmq_context, const std::string& zmq_uri, const int& socket_type, const QString& message) {
	zmq::socket_t	socket(zmq_context, socket_type);
	zmq::message_t	z_message(message.size() + 1);

#ifdef WINDOWS_OS
	_snprintf_s((char*)z_message.data(), message.size() + 1, message.size() + 1, "%s", message.toLatin1().constData());
#else
	snprintf((char*)z_message.data(), message.size() + 1, "%s", message.toLatin1().constData());
#endif

	try {
		socket.connect(zmq_uri.c_str());
		socket.send(z_message);
	//} catch (const zmq::error_t& z_error) {
	} catch (const std::exception z_error) {
		qCritical() << "send_zmq: cannot connect against " << zmq_uri.c_str() << ": " << z_error.what();
	}

	socket.close();
}

void	usage() {
	std::cerr<< "extractor -d <database_file>" << std::endl;
}
