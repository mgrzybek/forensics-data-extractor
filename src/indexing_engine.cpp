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

Indexing_Engine::Indexing_Engine(const QString& r_path, QTextEdit* output)
{
	root_path = r_path;
	output_area = output;
}

Indexing_Engine::Indexing_Engine(const QString& r_path)
{
	root_path = r_path;
	output_area = NULL;
}

Indexing_Engine::~Indexing_Engine() {
	output_area = NULL;
}

void Indexing_Engine::run() {
	if ( root_path.isEmpty() == false ) {
		static zmq::context_t context(1);
		static zmq::socket_t socket(context, ZMQ_PUB);

		recursive_search(socket, root_path);
		qDebug() << "Indexing finished, sending 'END;' message to subscribers..";
		send_zmq("END;", socket);
	}
}

void Indexing_Engine::set_root_path(const QString& dir_path) {
	root_path = dir_path;
}

void Indexing_Engine::recursive_search(zmq::socket_t& socket, const QString& dir_path) {

	socket.bind("inproc://forensics-indexer.inproc");

	QDir        path(dir_path);
	QStringList directories = path.entryList(QDir::AllDirs | QDir::NoDot | QDir::NoDotDot | QDir::Hidden);
	QStringList files = path.entryList(QDir::Files | QDir::Hidden);

	Q_FOREACH(QString file, files) {
		QString abs_path_file;

		abs_path_file = dir_path;
		abs_path_file += "/";
		abs_path_file += file;

		send_zmq(abs_path_file.toStdString(), socket);

//		if ( output_area == NULL )
//			qDebug() << abs_path_file;
//		else
//			output_area->append(abs_path_file);
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
