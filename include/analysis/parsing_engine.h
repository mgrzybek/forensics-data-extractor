/**
 * Project: forensics-data-extractor
 * File name: parsing_engine.h
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

#ifndef PARSING_ENGINE_H
#define PARSING_ENGINE_H

//#include <magic.h>
#include <zmq.hpp>

#include <QStandardItemModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTextEdit>
#include <QDateTime>
#include <QProcess>
#include <QThread>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QMap>
#include <QDir>

#include "common.h"
#include "database.h"
#include "checksum.h"
#include "databases/generic_database.h"
#include "extractors/extractor_select.h"

class Parsing_Engine : public QThread
{
	Q_OBJECT

	public:
		/*
		 * Constructor
		 *
		 * @arg	z_context	: the ZMQ context to be used
		 * @arg	r_path		: the path to parse
		 * @arg	db			: the database used to store the analysis's data
		 * @arg known_d_dbs	: the list of the known files databases
		 *
		 */
		Parsing_Engine(void* z_context, const QString& r_path, Database* db, generic_database_list* known_f_dbs);

		/*
		 * Destructor
		 */
		~Parsing_Engine();

		/*
		 * run
		 *
		 * This method needs to be written to allow QThread to start
		 *
		 */
		void	run();

		/*
		 * set_root_path
		 *
		 * Update root_path attribute
		 *
		 * @arg	dir_path	:	the new path
		 *
		 */
		void	set_root_path(const QString& dir_path);

	signals:
		void	ready();

	private slots:
		void	stop();

	private:
		/*
		 * Attributes
		 */
		// Messaging
		zmq::context_t*	zmq_context;
		bool			continue_scan;

		// Files data
		QString			root_path;
		Database*		database;
		generic_database_list*	known_files_dbs;
//		magichandle_t*	magic_object;

		Exception		e;

		/*
		 * recursive_search
		 *
		 * @arg	socket	:	the ZMQ socket to use
		 * @arg	dir_path	:	 the directory to scan
		 *
		 */
		void	recursive_search(zmq::socket_t& socket, const QString& dir_path);

		/*
		 * send_zmq
		 *
		 * Used to send messages through ZMQ system
		 *
		 * @arg	file	:	the file to send
		 * @arg	socket	:	the ZMQ socket to use
		 *
		 */
		void send_zmq(const struct_file& file, zmq::socket_t& socket);

		/*
		 * is_known
		 *
		 * Uses the known_files_databases list to answer the question:
		 * -> is this file known?
		 *
		 * @arg		: the file to check
		 * @return	: true (yes)
		 */
		bool	is_known(const struct_file& file);
};

#endif // PARSING_ENGINE_H

