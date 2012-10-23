/**
 * Project: forensics-data-extractor
 * File name: parsing_engine.cpp
 * Description: describes the parsing engine that lists and processes the files
 *
 * @author Mathieu Grzybek on 2012-08-21
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

#include "parsing_engine.h"

Parsing_Engine::Parsing_Engine(void* z_context, const QString& r_path, Database* db) : QThread() {
	zmq_context = (zmq::context_t*) z_context;

	if ( r_path.isEmpty() == true )
		throw "r_path empty";

	root_path = r_path;

//	magic_object = magic_open(MAGIC_CHECK);

	if ( db == NULL )
		throw "Databse is NULL";

	database = db;

//	if ( magic_object == NULL )
//		qCritical() << "Cannot init the magic library";

//	try {
//		dbc.connect("localhost");
//	} catch (const DBException& e) {
//	QErrorMessage msg = new QErrorMessage();
//		QCritical() << e.what();
//	}
	connect(this, SIGNAL(scan_stop()), this, SLOT(stop_scan()));
	continue_scan = true;
}

Parsing_Engine::~Parsing_Engine() {
}

void Parsing_Engine::run() {
	continue_scan = true;

	try {
		if ( root_path.isEmpty() == false ) {
			zmq::socket_t socket(*zmq_context, ZMQ_PUB);
#ifdef WINDOWS_OS
			socket.bind("tcp://127.0.0.1:5555");
#else
			socket.bind("inproc://forensics-parser.inproc");
#endif
			emit ready();
			// Pause to let the extractors start and connect
			wait(1); // TODO: is it really useful ?

			recursive_search(socket, root_path);

			qDebug() << "Parsing finished, sending 'END;' message to subscribers...";
			send_zmq("END;", socket);
		}
	} catch (const std::exception& e) {
		qCritical() << "Parsing_Engine: " << e.what();
	}
}

void Parsing_Engine::set_root_path(const QString& dir_path) {
	root_path = dir_path;
}

void Parsing_Engine::recursive_search(zmq::socket_t& socket, const QString& dir_path) {
	QDir		path(dir_path);
	QStringList	directories = path.entryList(QDir::AllDirs | QDir::NoDot | QDir::NoDotDot | QDir::Hidden | QDir::NoSymLinks);
	QStringList	files = path.entryList(QDir::Files | QDir::Hidden);

	if ( continue_scan == false )
		return;

	// TODO: create file structs (name + checksums) to be able to deal with big inserts or NoSQL
	Q_FOREACH(QString file, files) {
		struct_file	s_file;

		s_file.full_path = dir_path;
		s_file.full_path += "/";
		s_file.full_path += file;

		Checksum	checksum_calculator(&s_file);
		checksum_calculator.process_all();

		send_zmq(s_file.full_path.toAscii().constData(), socket);
		database->insert_file(s_file);
	}

	Q_FOREACH(QString dir, directories) {
		QString abs_path_dir;

		abs_path_dir = dir_path;
		abs_path_dir += "/";
		abs_path_dir += dir;

		recursive_search(socket, abs_path_dir);
	}
}

void Parsing_Engine::send_zmq(const std::string& message, zmq::socket_t& socket) {
	zmq::message_t	z_msg(message.size() + 1);
	snprintf((char*)z_msg.data(), message.size() + 1, "%s", message.c_str());
	socket.send(z_msg);
}
/*
   void Parsing_Engine::stop_scan() {
   continue_scan = false;
   }
*/

