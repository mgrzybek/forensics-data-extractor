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
		void*	z_context,
		const std::string&	z_output_uri,
		const QString& file_path
		) : Generic_Extractor(z_context, z_output_uri, file_path)
{
	error.calling_method = "Web_Browser_Extractor";
}

Web_Browser_Extractor::~Web_Browser_Extractor() {
}

void Web_Browser_Extractor::run()
{
	files_filter();
	set_file_to_analysed();
	update_db_places();
	update_db_search();
	qDebug() << "Web_Browser_Extrator: thread end";
//	emit refresh_models();
}

void	Web_Browser_Extractor::update_map(QMap<QString, uint>& map, const QString& key, const uint& value) {
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
	QString	queries;
	dir_path.clear(); // TODO: check if it is useful

	qDebug() << "URL Map size: " << url_map.size();

	for ( QMap<QString, uint>::const_iterator iter = url_map.begin() ; iter != url_map.end() ; iter++ ) {
		QString	query;

		query = "INSERT OR IGNORE INTO place (site, hits) VALUES ('" % QString(iter.key()).replace("'", "\'") % "',0);";
		queries += query;
		queries += ZMQ_QUERIES_SEPARATOR;

		query = "UPDATE place SET hits = hits + " % QString::number(iter.value()) % " WHERE site = '" % iter.key() % "';";
		queries += query;
		queries += ZMQ_QUERIES_SEPARATOR;
	}

	queries.resize(queries.size() - 1);

	if ( queries.size() > 0 )
		send_zmq(queries);

	// TODO: keep clear() ?
	url_map.clear();
}

void	Web_Browser_Extractor::update_db_search() {
	QString	queries;
	qDebug() << "Keywords Map size: " << keyword_map.size();

	for ( QMap<QString, uint>::const_iterator iter = keyword_map.begin() ; iter != keyword_map.end() ; iter++ ) {
		QString	query;

		query = "INSERT OR IGNORE INTO search (name, hits) VALUES ('" % QString(iter.key()).replace("'", "\'") % "',0);";
		queries += query;
		queries += ZMQ_QUERIES_SEPARATOR;

		query = "UPDATE search SET hits = hits + " % QString::number(iter.value()) % " WHERE name = '" % iter.key() % "';";
		queries += query;
		queries += ZMQ_QUERIES_SEPARATOR;
	}

	queries.resize(queries.size() - 1);

	if ( queries.size() > 0 )
		send_zmq(queries);

	// TODO: keep clear() ?
	keyword_map.clear();
}

void	Web_Browser_Extractor::set_file_to_analysed() {
	QString	query;

	query = "UPDATE parsed_file SET analysed = 1 WHERE file = '" % file % "';";
	send_zmq(query);

	query.clear();
}
