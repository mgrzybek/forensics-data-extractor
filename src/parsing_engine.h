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
#include <QDir>

class Parsing_Engine : public QThread
{
	Q_OBJECT

	public:
		Parsing_Engine(void* z_context, const QString& r_path, QStandardItemModel* model_files_list);
		~Parsing_Engine();

		void	run();

		void	set_root_path(const QString& dir_path);

	signals:
		void	ready();

	private slots:
			//			void	stop_scan();

	private:
			/*
			 * Attributes
			 */
			// Messaging
			zmq::context_t*	zmq_context;
			bool		continue_scan;

			// Files data
			QString			root_path;
			QStandardItemModel	*files_list;
//			magichandle_t*	magic_object;

			/*
			 * Methods
			 */
			void	recursive_search(zmq::socket_t& socket, const QString& dir_path);
			void	send_zmq(const std::string& message, zmq::socket_t& socket);
};

#endif // PARSING_ENGINE_H

