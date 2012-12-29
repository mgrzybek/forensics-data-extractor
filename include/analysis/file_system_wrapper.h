/**
 * Project: forensics-data-extractor
 * File name: file_system_wrapper.h
 * Description: describes the parser used to process mounted file systems
 *
 * @author Mathieu Grzybek on 20??-??-??
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

#ifndef FILE_SYSTEM_WRAPPER_H
#define FILE_SYSTEM_WRAPPER_H

#include <zmq.hpp>
#include <QStringList>
#include <QString>
#include <QDir>

#include "common.h"
#include "analysis/database.h"
#include "analysis/checksum.h"

class File_System_Wrapper
{
	public:
		/**
		 * File_System_Wrapper
		 *
		 * The constructor of the class
		 * It will not send messages using ZMQ
		 *
		 * @param	db	the database used to store the results
		 * @param	dir_path	the directory to scan
		 */
		File_System_Wrapper(Database* db, const QString& dir_path);

		/**
		 * File_System_Wrapper
		 *
		 * The constructor of the class
		 * It will send messages using ZMQ
		 *
		 * @param	socket	the ZMQ socket to use
		 * @param	db	the database used to store the results
		 * @param	dir_path	the directory to scan
		 */
		File_System_Wrapper(void* z_socket, Database* db, const QString& dir_path);

		/**
		 * recursive_directory_search
		 *
		 * Walks a directory to process files
		 *
		 * @param	dir_path	the directory to scan
		 *
		 */
		void	recursive_directories_search();

	private:

		/**
		 * recursive_directory_search
		 *
		 * Walks a directory to process files
		 *
		 * @param	dir_path	the directory to scan
		 *
		 */
		void	recursive_directories_search(const QString& dir_path);

		/**
		 * send_zmq
		 *
		 * Used to send messages through ZMQ system
		 *
		 * @param	message	the string to send
		 * @param	socket	the ZMQ socket to use
		 *
		 */
		void	send_zmq(const std::string& message);

		/**
		 * continue_scan
		 *
		 * This boolean is used to stop the process
		 * It is updated by the SIGNAL / SLOTS methods
		 */
		bool		continue_scan;

		/**
		 * database
		 *
		 * The database object to use to store the results
		 */
		Database*	database;

		/**
		 * socket
		 *
		 * The ZMQ socket used to send messages
		 */
		zmq::socket_t*	socket;

		/**
		 * e
		 *
		 * This exception is used to report problems
		 */
		Exception	e;

		/**
		 * source_dir_path
		 *
		 * The path of the directory to scan
		 */
		QString		source_dir_path;

#ifdef QT_5
		/**
		 * mime_type_database
		 *
		 * This object is used to determine files' MIMI-Types
		 */
		QMimeDatabase	mime_type_database;
#endif
};

#endif // FILE_SYSTEM_WRAPPER_H
