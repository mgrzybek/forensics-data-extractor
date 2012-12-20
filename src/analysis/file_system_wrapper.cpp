/**
 * Project:
 * File name:
 * Description:
 *
 * @author Mathieu Grzybek on 20??-??-??
 * @copyright 20?? Mathieu Grzybek. All rights reserved.
 * @version $Id: code-gpl-license.txt,v 1.2 2004/05/04 13:19:30 garry Exp $
 *
 * @see The GNU Public License (GPL) version 3 or higher
 *
 *
 * ? is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
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

#include "analysis/file_system_wrapper.h"

File_System_Wrapper::File_System_Wrapper(Database* db) {
	if ( db == NULL ) {
		e.calling_method = "File_System_Wrapper";
		e.msg = "db is NULL";
		throw e;
	}

	database = db;

	continue_scan = true;
}

void File_System_Wrapper::recursive_directories_search(zmq::socket_t& socket, const QString& dir_path) {
	QDir		path(dir_path);
	QStringList	directories = path.entryList(QDir::AllDirs | QDir::NoDot | QDir::NoDotDot | QDir::Hidden | QDir::NoSymLinks);
	QStringList	files = path.entryList(QDir::Files | QDir::Hidden);

	if ( continue_scan == false )
		return;

	Q_FOREACH(QString file, files) {
		struct_file	s_file;

		// We need  to initialize these QString to prevent segfaults in checksum_calculator
		s_file.sha1 = "";
		s_file.md5 = "";

		s_file.full_path = dir_path;
		s_file.full_path += "/";
		s_file.full_path += file;

		if ( database->is_parsed_file(s_file) == false ) {
			Checksum	checksum_calculator;
			checksum_calculator.process_all(&s_file);

			// TODO: add known files databases support (NSRL) to prevent the ZMQ message to be sent
			send_zmq(s_file.full_path.toAscii().constData(), socket);
			database->insert_file(s_file);
		}
	}

	Q_FOREACH(QString dir, directories) {
		QString abs_path_dir;

		abs_path_dir = dir_path;
		abs_path_dir += "/";
		abs_path_dir += dir;

		recursive_directories_search(socket, abs_path_dir);
	}
}

void File_System_Wrapper::send_zmq(const std::string& message, zmq::socket_t& socket) {
	zmq::message_t	z_msg(message.size() + 1);
	snprintf((char*)z_msg.data(), message.size() + 1, "%s", message.c_str());
	socket.send(z_msg);
}