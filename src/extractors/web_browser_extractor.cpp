/**
 * Project: forensics-data-extractor
 * File name: web_browser_extractor.cpp
 * Description: describes the abstract object used to process web browsers' data
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

#include "extractors/web_browser_extractor.h"

Web_Browser_Extractor::Web_Browser_Extractor(
		void*			z_context,
		Database*		db
		)
{
	if ( z_context == NULL ) {
		e.calling_method = "Web_Browser_Extractor";
		e.msg = "z_context is NULL";

		throw e;
	}

	zmq_context	= (zmq::context_t*) z_context;

	if ( db == NULL ) {
		e.calling_method = "Web_Browser_Extractor";
		e.msg = "db is NULL";

		throw e;
	}

	database	= db;

	models	= NULL;
}

Web_Browser_Extractor::~Web_Browser_Extractor() {

}

void Web_Browser_Extractor::run()
{
	zmq::socket_t	socket(*zmq_context, ZMQ_SUB);
	bool			connected = false;

	while ( connected == false ) {
		try {
			socket.setsockopt(ZMQ_SUBSCRIBE, 0, 0);
#ifdef WINDOWS_OS
			socket.connect("tcp://127.0.0.1:5555");
#else
			socket.connect("inproc://forensics-parser.inproc");
#endif
			connected = true;
			while (1) {
				QString			string_message;
				zmq::message_t	z_message;

				socket.recv(&z_message);
				string_message = static_cast<char*>(z_message.data());

				if ( string_message.compare("END;") != 0 )
					files_filter(string_message);
				else
					break;
			}
		} catch (const std::exception& e) {
			qCritical() << "Web_Browser_Extractor: " << e.what();
		}
	}

	connected = false;
	set_files_to_analysed();
	update_db_places();
	update_db_search();
	emit finished();
	qDebug() << "Web_Browser_Extrator: thread end";
}

void	Web_Browser_Extractor::update_map(QMap<QString, uint>& map, const QString& key, const uint& value) {
	// TODO: check if we need to avoid null values (Chrome: visit_count == 0 -> 1 hit?)
	if ( value > 0 ) {
		QMap<QString, uint>::iterator iter = map.find(key);

		if ( iter == map.end() ) {
			map.insert(key, value);
		} else {
			iter.value() += value;
		}
	}
}

void	Web_Browser_Extractor::update_db_places() {
	dir_path.clear(); // TODO: check if it is useful

	qDebug() << "URL Map size: " << url_map.size();

	for ( QMap<QString, uint>::const_iterator iter = url_map.begin() ; iter != url_map.end() ; iter++ ) {
		QStringList	queries;
		QString		query;

		query = "INSERT OR IGNORE INTO place (site, hits) VALUES ('" % iter.key() % "',0);";
		queries << query;
		query = "UPDATE place SET hits = hits + " % QString::number(iter.value()) % " WHERE site = '" % iter.key() % "';";
		queries << query;

		//qDebug() << "update_model_places()" << queries;

		database->exec(queries);
	}

	// TODO: keep clear() ?
	url_map.clear();
}

void	Web_Browser_Extractor::update_db_search() {
	qDebug() << "Keywords Map size: " << keyword_map.size();

	for ( QMap<QString, uint>::const_iterator iter = keyword_map.begin() ; iter != keyword_map.end() ; iter++ ) {
		QStringList	queries;
		QString		query;

		query = "INSERT OR IGNORE INTO search (name, hits) VALUES ('" % iter.key() % "',0);";
		queries << query;
		query = "UPDATE search SET hits = hits + " % QString::number(iter.value()) % " WHERE name = '" % iter.key() % "';";
		queries << query;

		//qDebug() << "update_model_places()" << queries;

		database->exec(queries);
	}

	// TODO: keep clear() ?
	keyword_map.clear();
}

void	Web_Browser_Extractor::set_files_to_analysed() {
	QString		query;
	QStringList	queries;

	Q_FOREACH(QString file, files) {
		query = "UPDATE parsed_file SET analysed = 1 WHERE file = '" % file % "';";
		queries << query;
		query.clear();
	}

	database->exec(queries);
}
