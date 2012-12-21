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
		 * @param	socket	the ZMQ socket to use
		 */
		File_System_Wrapper(Database* db);
		File_System_Wrapper(void* z_socket, Database* db);

		/*
		 * recursive_directory_search
		 *
		 * @arg	dir_path	:	the directory to scan
		 *
		 */
		void	recursive_directories_search(const QString& dir_path);

	private:
		/*
		 * send_zmq
		 *
		 * Used to send messages through ZMQ system
		 *
		 * @arg	message	:	the string to send
		 * @arg	socket	:	the ZMQ socket to use
		 *
		 */
		void	send_zmq(const std::string& message);

		bool		continue_scan;
		Database*	database;
		zmq::socket_t*	socket;
		Exception	e;
};

#endif // FILE_SYSTEM_WRAPPER_H
