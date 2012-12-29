/**
 * Project: forensics-data-extractor
 * File name: file_system_wrapper.cpp
 * Description: implements the parser used to process mounted file systems
 *
 * @author Mathieu Grzybek on 2012-12-21
 * @copyright 2012 Mathieu Grzybek. All rights reserved.
 * @version $Id: code-gpl-license.txt,v 1.2 2004/05/04 13:19:30 garry Exp $
 *
 * @see The GNU Public License (GPL) version 3 or higher
 *
 *
 * forensics-data-extractor is free software; you can redistribute it and/or modify
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

File_System_Wrapper::File_System_Wrapper(Database* db, const QString& dir_path) {
	if ( db == NULL ) {
		e.calling_method = "File_System_Wrapper";
		e.msg = "db is NULL";
		throw e;
	}
	database = db;

	socket = NULL;
	continue_scan = true;
	source_dir_path = dir_path;
}

File_System_Wrapper::File_System_Wrapper(void* z_socket, Database* db, const QString& dir_path) {
	e.calling_method = "File_System_Wrapper";

	if ( z_socket == NULL ) {
		e.msg = "z_socket is NULL";
		throw e;
	}
	socket = (zmq::socket_t*) z_socket;

	if ( db == NULL ) {
		e.msg = "db is NULL";
		throw e;
	}
	database = db;

	continue_scan = true;
	source_dir_path = dir_path;
}

void File_System_Wrapper::recursive_directories_search() {
	database->insert_source(source_dir_path, DIRECTORY);
	recursive_directories_search(source_dir_path);
}

void File_System_Wrapper::recursive_directories_search(const QString& dir_path) {
	QDir		path(dir_path);
	QStringList	directories = path.entryList(QDir::AllDirs | QDir::NoDot | QDir::NoDotDot | QDir::Hidden | QDir::NoSymLinks);
	QStringList	files = path.entryList(QDir::Files | QDir::Hidden);

	if ( continue_scan == false )
		return;

	Q_FOREACH(QString file, files) {
		struct_file	s_file;

		// We need to initialize these QStrings to prevent segfaults in checksum_calculator
		s_file.sha1 = "";
		s_file.md5 = "";
		s_file.source = source_dir_path;
		s_file.full_path = dir_path;
		s_file.full_path += file;
		s_file.inode = -1;

		QFileInfo	file_info(s_file.full_path);

#ifdef QT_5
		s_file.mime_type = mime_type_database.mimeTypeForFile(file_info);
#endif

		s_file.size = file_info.size();

		if ( database->is_parsed_file(s_file) == false ) {
			Checksum	checksum_calculator;
			checksum_calculator.process_all(&s_file);

			// TODO: add known files databases support (NSRL) to prevent the ZMQ message to be sent
			if ( socket != NULL )
				send_zmq(s_file.full_path.toLatin1().constData());
			database->insert_file(s_file);
		}
	}

	Q_FOREACH(QString dir, directories) {
		QString abs_path_dir;

		abs_path_dir = dir_path;
		abs_path_dir += "/";
		abs_path_dir += dir;

		recursive_directories_search(abs_path_dir);
	}
}

void File_System_Wrapper::send_zmq(const std::string& message) {
	if ( socket == NULL )
		return;

	zmq::message_t	z_msg(message.size() + 1);
#ifdef WINDOWS_OS
	_snprintf_s((char*)z_msg.data(), message.size() + 1, message.size() + 1, "%s", message.c_str());
#else
	snprintf((char*)z_msg.data(), message.size() + 1, "%s", message.c_str());
#endif
	socket->send(z_msg);
}
