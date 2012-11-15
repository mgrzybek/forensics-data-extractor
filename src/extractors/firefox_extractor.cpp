/**
 * Project: forensics-data-extractor
 * File name: firefox_extractor.cpp
 * Description: describes the object used to process Mozilla Firefox data
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

#include "extractors/firefox_extractor.h"
/*
Firefox_Extractor::Firefox_Extractor(
		void*			z_context,
		web_browser_models*	models
		) : Web_Browser_Extractor(
			z_context,
			models
			)
{
	url_map.clear();
}
*/
Firefox_Extractor::Firefox_Extractor(
		void*		z_context,
		const std::string& z_output_uri,
		const QString& file_path
	) : Web_Browser_Extractor(
				z_context, z_output_uri, file_path
			)
{
	url_map.clear();
}

Firefox_Extractor::~Firefox_Extractor() {
}

void Firefox_Extractor::files_filter() {
	QStringList file_path_parts = file.split("/", QString::SkipEmptyParts);
	QString file_name = file_path_parts.at(file_path_parts.size() - 1);

	if ( file_name.compare("cookies.sqlite") == 0 ) {
		extract_cookies();
	}
	if ( file_name.compare("downloads.sqlite") == 0 ) {
		extract_downloads();
	}
	if ( file_name.compare("forms.sqlite") == 0 ) {
		extract_forms();
	}
	if ( file_name.compare("places.sqlite") == 0 ) {
		extract_places();
	}
//	if ( file_name.compare("search.sqlite") == 0 ) {
//		extract_search();
//	}
	if ( file.compare("signons.sqlite") == 0 ) {
		extract_signons();
	}
}

regex_list	Firefox_Extractor::get_regexes() {
	regex_list	result;

	result.append(QRegExp("^cookies.sqlite$"));
	result.append(QRegExp("^downloads.sqlite$"));
	result.append(QRegExp("^forms.sqlite$"));
	result.append(QRegExp("^places.sqlite$"));
	result.append(QRegExp("^signons.sqlite$"));

	return result;
}

void Firefox_Extractor::extract_places() {
	{
		SQLITE_OPEN(file)

		QSqlQuery	query(db);

		query.exec("SELECT COUNT(*), rev_host FROM moz_places GROUP BY rev_host ORDER BY COUNT(*);");

		while (query.next()) {
			QString reversed_host = "";

			Q_FOREACH(QChar c, query.value(1).toString()) {
				reversed_host.insert(0, c);
			}
			reversed_host.remove(0, 1);

			update_map(url_map, reversed_host, query.value(0).toUInt());
		}
		query.clear();
	}
	SQLITE_CLOSE(file)
}

void Firefox_Extractor::extract_cookies() {
	{
		SQLITE_OPEN(file)

		QSqlQuery	query(db);
		QString		insert_query;

		query.exec("SELECT name, value, host, path, expiry, isSecure, isHttpOnly, lastAccessed FROM moz_cookies ORDER BY lastAccessed;");

		while (query.next()) {
			insert_query = "INSERT INTO cookie (name, value, host, path, expiration, secured, http, last_accessed) VALUES (";
			insert_query += "'" % db.driver()->formatValue(query.value(0).toString()) % "',";
			insert_query += "'" % db.driver()->formatValue(query.value(1).toString()) % "',";
			insert_query += "'" % db.driver()->formatValue(query.value(2).toString()) % "',";
			insert_query += "'" % db.driver()->formatValue(query.value(3).toString()) % "',";
			insert_query += "'" % db.driver()->formatValue(query.value(4).toString()) % "',";
			insert_query += "'" % db.driver()->formatValue(query.value(5).toString()) % "',";
			insert_query += "'" % db.driver()->formatValue(query.value(6).toString()) % "',";
			insert_query += "'" % db.driver()->formatValue(query.value(7).toString());
			insert_query += "');";

			send_zmq(insert_query);
		}

		query.clear();
		insert_query.clear();
	}
	SQLITE_CLOSE(file)
}

void Firefox_Extractor::extract_downloads() {
	{
		SQLITE_OPEN(file)

		QSqlQuery	query(db);
		QString		insert_query;

		query.exec("SELECT name, source, mimeType FROM moz_downloads ORDER BY startTime;");

		while (query.next()) {
			insert_query = "INSERT INTO download (name, source, mime) VALUES (";
			insert_query += "'" % db.driver()->formatValue(query.value(0).toString()) % "',";
			insert_query += "'" % db.driver()->formatValue(query.value(1).toString()) % "',";
			insert_query += "'" % db.driver()->formatValue(query.value(2).toString());
			insert_query += "');";

			send_zmq(insert_query);
		}

		query.clear();
		insert_query.clear();
	}
	SQLITE_CLOSE(file)
}

void Firefox_Extractor::extract_forms() {
	{
		SQLITE_OPEN(file)

		QSqlQuery	query(db);
		QString		insert_query;

		query.exec("SELECT hostname, encryptedUsername, encryptedPassword FROM moz_logins ORDER BY hostname;");

		while (query.next()) {
			insert_query = "INSERT INTO form (host, id, password) VALUES (";
			insert_query += "'" % db.driver()->formatValue(query.value(0).toString()) % "',";
			insert_query += "'" % db.driver()->formatValue(query.value(1).toString()) % "',";
			insert_query += "'" % db.driver()->formatValue(query.value(2).toString());
			insert_query += "');";

			send_zmq(insert_query);
		}

		query.clear();
		insert_query.clear();
	}
	SQLITE_CLOSE(file)

}

// TODO: can we really extract these data?
void Firefox_Extractor::extract_search() {
	{
		SQLITE_OPEN(file)

		QSqlQuery	query(db);

		query.exec("SELECT name, value FROM engine_data ORDER BY name;");

		while (query.next()) {
			update_map(keyword_map, query.value(0).toString(), query.value(1).toUInt());
		}

		query.clear();
	}
	SQLITE_CLOSE(file)
}

void Firefox_Extractor::extract_signons() {
	{
		SQLITE_OPEN(file)

		QSqlQuery 	query(db);
		QString		insert_query;

		query.exec("SELECT hostname, encryptedUsername, encryptedPassword FROM moz_logins ORDER BY hostname;");

		while (query.next()) {
			insert_query = "INSERT INTO signon (host, id, password) VALUES (";
			insert_query += "'" % db.driver()->formatValue(query.value(0).toString()) % "',";
			insert_query += "'" % db.driver()->formatValue(query.value(1).toString()) % "',";
			insert_query += "'" % db.driver()->formatValue(query.value(2).toString());
			insert_query += "');";

			send_zmq(insert_query);
		}

		query.clear();
		insert_query.clear();
	}
	SQLITE_CLOSE(file)
}

