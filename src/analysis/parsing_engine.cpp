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

#include "analysis/parsing_engine.h"

// TODO: use some C++11 feature: call constructor form another constructor
Parsing_Engine::Parsing_Engine(void* z_context, const QString& r_path, Database* db, generic_database_list* known_f_dbs) : QRunnable() {
	e.calling_method = "Parsing_Engine";

	if ( z_context == NULL ) {
		e.msg = "z_context is NULL";
		throw e;
	}
	zmq_context = (zmq::context_t*) z_context;

	if ( r_path.isEmpty() == true ) {
		e.msg = "r_path is empty";
		throw e;
	}
	root_path = r_path;

//	magic_object = magic_open(MAGIC_CHECK);

	if ( db == NULL ) {
		e.msg = "Database is NULL";
		throw e;
	}
	database = db;

	known_files_dbs = known_f_dbs;

//	if ( magic_object == NULL )
//		qCritical() << "Cannot init the magic library";

	continue_scan = true;

	if ( autoDelete(true) == false ) {
		error.msg = "Cannot set true on autoDelete";
		throw error;
	}
}

Parsing_Engine::Parsing_Engine(const QString& r_path, Database* db, generic_database_list* known_f_dbs) : QRunnable() {
	if ( r_path.isEmpty() == true ) {
		e.msg = "r_path is empty";
		throw e;
	}
	root_path = r_path;

//	magic_object = magic_open(MAGIC_CHECK);

	if ( db == NULL ) {
		e.msg = "Database is NULL";
		throw e;
	}
	database = db;

	zmq_context = NULL;
	known_files_dbs = known_f_dbs;

//	if ( magic_object == NULL )
//		qCritical() << "Cannot init the magic library";

	continue_scan = true;

	if ( autoDelete(true) == false ) {
		error.msg = "Cannot set true on autoDelete";
		throw error;
	}
}

Parsing_Engine::~Parsing_Engine() {
}

void Parsing_Engine::run() {
	continue_scan = true;
	e.calling_method = "Parsing_Engine::run";

	try {
		if ( root_path.isEmpty() == false ) {
			QFileInfo	file_info(root_path);

			if ( zmq_context != NULL ) {
				zmq::socket_t	socket(*zmq_context, ZMQ_PUSH);

				socket.bind(ZMQ_INPROC_PARSER_PUSH);
				qDebug() << e.calling_method << ": socket binded to " << ZMQ_INPROC_PARSER_PUSH;

				if ( file_info.isDir() == true ) {
					File_System_Wrapper	directory_process(&socket, database, root_path);
					directory_process.recursive_directories_search();
				} else {
					Sleuthkit_Wrapper	image_process(&socket, database, root_path);
					image_process.image_process();
				}

				socket.close();
			} else {
				if ( file_info.isDir() == true ) {
					File_System_Wrapper	directory_process(database, root_path);
					directory_process.recursive_directories_search();
				} else {
					Sleuthkit_Wrapper	image_process(database, root_path);
					image_process.image_process();
				}
			}
		}
	} catch (const std::exception& e) {
		qCritical() << "Parsing_Engine: " << e.what();
	}

	qDebug() << e.calling_method << "Parsing completed";
}

void Parsing_Engine::set_root_path(const QString& dir_path) {
	root_path = dir_path;
}

void Parsing_Engine::stop() {
	continue_scan = false;
}

void Parsing_Engine::send_zmq(const struct_file& file, zmq::socket_t& socket) {
	/*
	 * Headers
	 */
	// send the target node
//	zmq::message_t	z_msg_target(file.node.size() + 1);
//	snprintf((char*)z_msg_target.data(), file.node.size() + 1, "%s", file.node.toAscii().constData());
//	socket.send(z_msg_target, ZMQ_SENDMORE);
	// send the extractor
	zmq::message_t	z_msg_extractor(file.extractor.size() + 1);
#ifdef WINDOWS_OS
	_snprintf_s((char*)z_msg_extractor.data(), file.extractor.size() + 1, file.extractor.size() + 1, "%s", file.extractor.toLatin1().constData());
#else
	snprintf((char*)z_msg_extractor.data(), file.extractor.size() + 1, "%s", file.extractor.toLatin1().constData());
#endif
	socket.send(z_msg_extractor, ZMQ_SNDMORE);

	/*
	 * Data
	 */
	zmq::message_t	z_msg_data(file.full_path.size() + 1);
#ifdef WINDOWS_OS
	_snprintf_s((char*)z_msg_data.data(), file.full_path.size() + 1, file.full_path.size() + 1, "%s", file.full_path.toLatin1().constData());
#else
	snprintf((char*)z_msg_data.data(), file.full_path.size() + 1, "%s", file.full_path.toLatin1().constData());
#endif
	socket.send(z_msg_data);
}

bool	Parsing_Engine::is_known(const struct_file& file) {
	if ( known_files_dbs == NULL )
		return false;

	Q_FOREACH(Generic_Database* g_db, *known_files_dbs) {
		if ( g_db->is_known(file) == true ) {
			QString	query = "UPDATE parsed_file SET known = '";

			query += g_db->get_name();
			query += "';";

			database->exec(query);
			return true;
		}
	}

	return false;
}

