/**
 * Project: forensics-data-extractor
 * File name: indexing_engine.h
 * Description: describes the indexing engine that lists and processes the files
 *
 * @author Mathieu Grzybek on 2012-05-20
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

#ifndef INDEXING_ENGINE_H
#define INDEXING_ENGINE_H

#include <magic.h>
#include <zmq.hpp>

#include <QStandardItemModel>
#include <QTextEdit>
#include <QThread>
#include <QString>
#include <QDebug>
#include <QDir>

class Indexing_Engine : public QThread
{
	Q_OBJECT

	public:
		Indexing_Engine(void* z_context, const QString& r_path, QStandardItemModel* model_files_list);
		~Indexing_Engine();

		void	run();

		void	set_root_path(const QString& dir_path);

	signals:
		void	ready();

	private:
		zmq::context_t*	zmq_context;
		QString		root_path;
		QStandardItemModel* files_list;
//		magichandle_t*	magic_object;

		void	recursive_search(zmq::socket_t& socket, const QString& dir_path);
		void	send_zmq(const std::string& message, zmq::socket_t& socket);
};

#endif // SEARCH_ENGINE_H
