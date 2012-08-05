/**
 * Project: forensics-data-extractor
 * File name: indexing_engine.cpp
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

#include "indexing_engine.h"

Indexing_Engine::Indexing_Engine(void* z_context, Sqlite_Backend* index_db, const QString& r_path, QStandardItemModel* model_files_list) : QThread()
{
	zmq_context = (zmq::context_t*) z_context;
	db = index_db;
	root_path = r_path;
//	magic_object = magic_open(MAGIC_CHECK);
	files_list = model_files_list;

//	if ( magic_object == NULL )
//		qCritical() << "Cannot init the magic library";

//	try {
//		dbc.connect("localhost");
//	} catch (const DBException& e) {
	//	QErrorMessage msg = new QErrorMessage();
//		QCritical() << e.what();
//	}
}

Indexing_Engine::~Indexing_Engine() {
}

void Indexing_Engine::run() {
	try {
		if ( root_path.isEmpty() == false ) {
			zmq::socket_t socket(*zmq_context, ZMQ_PUB);
#ifdef WINDOWS_OS
			socket.bind("tcp://127.0.0.1:5555");
#else
			socket.bind("inproc://forensics-indexer.inproc");
#endif
			emit ready();
			wait(1);

			recursive_search(socket, root_path);

			qDebug() << "Indexing finished, sending 'END;' message to subscribers...";
			send_zmq("END;", socket);
		}
	} catch (const std::exception& e) {
		qCritical() << "Indexing_Engine: " << e.what();
	}
}

void Indexing_Engine::set_root_path(const QString& dir_path) {
	root_path = dir_path;
}

void Indexing_Engine::index_file(const QString& directory, const QString& file) {
	QFileInfo	file_info(directory, file);
	QString		query;

	db->open_db();

// TODO: escape the "'" characters from the fields

//	query = "INSERT INTO file (file_id, basename, complete_suffix, canonical_path, size, last_modified, last_read) VALUES ('";
	query = "INSERT INTO file (basename, complete_suffix, canonical_path, size, last_modified, last_read) VALUES ('";

	query += file_info.baseName() + "','";
	query += file_info.completeSuffix() + "','";
	query += file_info.canonicalPath() + "','";
	query += QString::number(file_info.size()) + "','"; // in bytes
	query += file_info.lastModified().toString(Qt::ISODate) + "','";
	query += file_info.lastRead().toString(Qt::ISODate) + "');";

	qDebug() << query;

	db->exec(query);
}

void Indexing_Engine::recursive_search(zmq::socket_t& socket, const QString& dir_path) {
	QDir		path(dir_path);
	QStringList	directories = path.entryList(QDir::AllDirs | QDir::NoDot | QDir::NoDotDot | QDir::Hidden);
	QStringList	files = path.entryList(QDir::Files | QDir::Hidden);

	Q_FOREACH(QString file, files) {
		QString		abs_path_file;
		QList<QStandardItem*>	row;

		abs_path_file = dir_path;
		abs_path_file += "/";
		abs_path_file += file;

		index_file(dir_path, file);

		send_zmq(abs_path_file.toStdString(), socket);

		row << new QStandardItem(abs_path_file);
		files_list->appendRow(row);
	}

	Q_FOREACH(QString dir, directories) {
		QString abs_path_dir;

		abs_path_dir = dir_path;
		abs_path_dir += "/";
		abs_path_dir += dir;

		recursive_search(socket, abs_path_dir);
	}
}

void Indexing_Engine::send_zmq(const std::string& message, zmq::socket_t& socket) {
	zmq::message_t	z_msg(message.size() + 1);
	snprintf((char*)z_msg.data(), message.size() + 1, "%s", message.c_str());
	socket.send(z_msg);
}
