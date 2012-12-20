/**
 * Project: forensics-data-extractor
 * File name: chrome_extractor.cpp
 * Description: describes the object used to process Chrome / Chromium data
 *
 * @author Mathieu Grzybek on 2012-05-20
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

#include "extractors/chrome_extractor.h"
/*
Chrome_Extractor::Chrome_Extractor(
		void*			z_context,
		web_browser_models*	models
		) : Web_Browser_Extractor(
			z_context,
			models
			)
{
}
*/
Chrome_Extractor::Chrome_Extractor(
		void*		z_context, const std::string& z_output_uri, const QString& file_path
		) : Web_Browser_Extractor(
			z_context,
			z_output_uri,
			file_path
			)
{
}

Chrome_Extractor::~Chrome_Extractor() {
}

void Chrome_Extractor::files_filter() {
	QStringList file_path_parts = file.split("/", QString::SkipEmptyParts);
	QString file_name = file_path_parts.at(file_path_parts.size() - 1);

	if ( file_name.compare("Cookies") == 0 ) {
		extract_places();
	}
	if ( file_name.compare("History") == 0 ) {
		extract_downloads();
		extract_places();
		extract_search();
	}
	if ( file_name.compare("Login Data") == 0 ) {
		extract_signons();
	}
}

regex_list	Chrome_Extractor::get_regexes() {
	regex_list	result;

	result.append(QRegExp("^Cookies$"));
	result.append(QRegExp("^History$"));
	result.append(QRegExp("^Login Data$"));

	return result;
}

void Chrome_Extractor::extract_places() {
	{
		SQLITE_OPEN(file)
		// TODO: remove last "/" using the regex
		// perl -pe 's/^https*:\/\/(.*?)\/.*?$/$1/'
		QSqlQuery	query(db);
#ifdef WINDOWS_OS
		QRegExp	url_cleaner("^https{0,1}\:\/\/(.+)(\/.*|$)");
		QRegExp slash_cleaner("\/$");
#else
		QRegExp	url_cleaner("^https{0,1}\\:\\/\\/(.+)(\\/.*|\\/|$)");
		QRegExp slash_cleaner("\\/$");
#endif
		url_cleaner.setMinimal(true);

		/*
		 * rebuilding the history:
		 *
		 * select urls.url, visits.visit_time from urls, visits where urls.id = visits.url
		 * select urls.url, urls.title, urls.visit_count, urls.typed_count,
		 *	datetime( urls.last_visit_time/1000000-11644473600, 'unixepoch' ) as lastvisit_date ,
		 *	urls.hidden, visits.visit_time, visits.from_visit, visits.transition
		 *	from urls, visits
		 *	where urls.id = visits.url;
		 *
		 * TODO: add the last_visit_time to the database
		 * Take care of Window's time convertion to UNIX timestamp
		 */
		query.exec("SELECT visit_count, url FROM urls ORDER BY visit_count DESC, url ASC;");

		while (query.next()) {
			QString cleaned_url = "";

			if ( url_cleaner.indexIn(query.value(1).toString()) != -1 ) {
				cleaned_url = url_cleaner.cap(1);
				cleaned_url.replace(slash_cleaner, "");

				update_map(url_map, cleaned_url, query.value(0).toUInt());
			} else
				qDebug() << "Regex failed on " << query.value(1).toString();
		}
		query.clear();
	}
	SQLITE_CLOSE(file)
}

void Chrome_Extractor::extract_cookies() {
	{
		SQLITE_OPEN(file)

		QSqlQuery	query(db);
		QString		insert_query;

		query.exec("SELECT name, value, host_key, path, has_expires, secure, httponly, last_access_utc FROM cookies ORDER BY last_access_utc;");

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

void Chrome_Extractor::extract_downloads() {
	{
		SQLITE_OPEN(file)

		QSqlQuery	query(db);
		QString		insert_query;

		query.exec("SELECT full_path, url FROM downloads ORDER BY start_time;");

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

void Chrome_Extractor::extract_forms() {
	{
		SQLITE_OPEN(file)

		QSqlQuery	query(db);
		QString		insert_query;

		query.exec("SELECT action_url, username_value, password_value FROM logins ORDER BY action_url;");

		while (query.next()) {
			insert_query = "INSERT INTO form (host, id, password) VALUES (";
			insert_query += "'" % db.driver()->formatValue(query.value(0).toString()) % "',";
			insert_query += "'" % db.driver()->formatValue(query.value(1).toString()) % "',";
			insert_query += "'" % db.driver()->formatValue(query.value(2).toString());
			insert_query += "');";

			send_zmq(insert_query);
		}

		query.clear();
	}
	SQLITE_CLOSE(file)
}

void Chrome_Extractor::extract_search() {
	{
		SQLITE_OPEN(file)

		QSqlQuery	query(db);

		query.exec("SELECT term, COUNT(*) FROM keyword_search_terms GROUP BY term;");

		while (query.next()) {
			update_map(keyword_map, query.value(0).toString(), query.value(1).toUInt());
		}

		query.clear();
	}
	SQLITE_CLOSE(file)
}

void Chrome_Extractor::extract_signons() {
	{
		SQLITE_OPEN(file)

		QSqlQuery	query(db);
		QString		insert_query;

		query.exec("SELECT action_url, username_value, password_value FROM logins ORDER BY action_url;");

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
