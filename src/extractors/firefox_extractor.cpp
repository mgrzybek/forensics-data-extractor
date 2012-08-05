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

#include "firefox_extractor.h"

Firefox_Extractor::Firefox_Extractor(
		void*			z_context,
		web_browser_models*	models
		) : Web_Browser_Extractor(
			z_context,
			models
			)
{
	sorting_db = QSqlDatabase::addDatabase("QSQLITE");
	sorting_db.setDatabaseName(":memory:");

	QSqlQuery	query(sorting_db);

	if ( query.exec("CREATE TABLE host (hits INTEGER NOT NULL, site LONGVARCHAR NOT NULL);") == false )
		qCritical() << "Cannot create host table into memory: " << sorting_db.lastError().NoError;

	query.clear();
}

Firefox_Extractor::~Firefox_Extractor() {
	QSqlQuery		query(sorting_db);
	QList<QStandardItem*>	row;

	query.exec("SELECT SUM(hits), site FROM host GROUP BY site;");

	while (query.next()) {
		row.clear();

		row << new QStandardItem(query.value(0).toString());
		row << new QStandardItem(query.value(1).toString());

		models->places.appendRow(row);
	}
	query.clear();
	row.clear();

	query.exec("DROP TABLE host;");
	query.clear();
	sorting_db.close();
}

void Firefox_Extractor::files_filter(const QString& file_path) {
	QStringList file_path_parts = file_path.split("/", QString::SkipEmptyParts);
	QString file = file_path_parts.at(file_path_parts.size() - 1);

	if ( file.compare("cookies.sqlite") == 0 ) {
		files.cookies << file_path;
		extract_cookies(file_path);
	}
	if ( file.compare("downloads.sqlite") == 0 ) {
		files.downloads << file_path;
		extract_downloads(file_path);
	}
	if ( file.compare("forms.sqlite") == 0 ) {
		files.forms << file_path;
		extract_forms(file_path);
	}
	if ( file.compare("places.sqlite") == 0 ) {
		files.places << file_path;
		extract_places(file_path);
	}
//	if ( file.compare("search.sqlite") == 0 ) {
//		files.searches << file_path;
//		extract_search(file_path);
//	}
	if ( file.compare("signons.sqlite") == 0 ) {
		files.signons << file_path;
		extract_signons(file_path);
	}
}

void Firefox_Extractor::extract_places(const QString& file) {
	{
		SQLITE_OPEN(file)

		QSqlQuery	query(db);
		QSqlQuery	in_mem_query(sorting_db);

		query.exec("SELECT COUNT(*), rev_host FROM moz_places GROUP BY rev_host ORDER BY COUNT(*);");
		in_mem_query.prepare("INSERT INTO host (hits, site) VALUES (:hits, :site);");

		while (query.next()) {
			QString reversed_host;

			Q_FOREACH(QChar c, query.value(1).toString()) {
				reversed_host.insert(0, c);
			}
			reversed_host.remove(0, 1);

			if ( query.value(0).toString().isEmpty() == false and query.value(1).toString().isEmpty() == false ) {

				in_mem_query.bindValue(":hits", query.value(0).toString());
				in_mem_query.bindValue(":site", query.value(1).toString());
				in_mem_query.exec();
			}
		}
		query.clear();
		in_mem_query.clear();
	}
	SQLITE_CLOSE(file)
}

void Firefox_Extractor::extract_cookies(const QString& file) {
	{
		SQLITE_OPEN(file)

		QSqlQuery		query(db);
		QList<QStandardItem*>	row;

		query.exec("SELECT name, value, host, path, expiry, isSecure, isHttpOnly, lastAccessed FROM moz_cookies ORDER BY lastAccessed;");

		while (query.next()) {
			row.clear();

			row << new QStandardItem(query.value(0).toString());
			row << new QStandardItem(query.value(1).toString());
			row << new QStandardItem(query.value(2).toString());
			row << new QStandardItem(query.value(3).toString());
			row << new QStandardItem(query.value(4).toString());
			row << new QStandardItem(query.value(5).toString());
			row << new QStandardItem(query.value(6).toString());

			models->cookies.appendRow(row);
		}
		query.clear();
	}
	SQLITE_CLOSE(file)
}

void Firefox_Extractor::extract_downloads(const QString& file) {
	{
		SQLITE_OPEN(file)

		QSqlQuery   query(db);
		QList<QStandardItem*>	row;

		query.exec("SELECT name, source, mimeType FROM moz_downloads ORDER BY startTime;");

		while (query.next()) {
			row.clear();

			row << new QStandardItem(query.value(0).toString());
			row << new QStandardItem(query.value(1).toString());
			row << new QStandardItem(query.value(2).toString());

			models->downloads.appendRow(row);
		}
		query.clear();
	}
	SQLITE_CLOSE(file)
}

void Firefox_Extractor::extract_forms(const QString& file) {
	{
		SQLITE_OPEN(file)

		QSqlQuery		query(db);
		QList<QStandardItem*>	row;

		query.exec("SELECT hostname, encryptedUsername, encryptedPassword FROM moz_logins ORDER BY hostname;");

		while (query.next()) {
			row.clear();

			row << new QStandardItem(query.value(0).toString());
			row << new QStandardItem(query.value(1).toString());
			row << new QStandardItem(query.value(2).toString());

			models->forms.appendRow(row);
		}
		query.clear();
	}
	SQLITE_CLOSE(file)

}

// TODO: can we really extract these data?
void Firefox_Extractor::extract_search(const QString& file) {
	{
		SQLITE_OPEN(file)

		QSqlQuery		query(db);
		QList<QStandardItem*>	row;

		query.exec("SELECT name, value FROM engine_data ORDER BY name;");

		while (query.next()) {
			row.clear();

			row << new QStandardItem(query.value(0).toString());
			row << new QStandardItem(query.value(1).toString());

			models->searches.appendRow(row);
		}
		query.clear();
	}
	SQLITE_CLOSE(file)
}

void Firefox_Extractor::extract_signons(const QString& file) {
	{
		SQLITE_OPEN(file)

		QSqlQuery   query(db);
		QList<QStandardItem*>	row;

		query.exec("SELECT hostname, encryptedUsername, encryptedPassword FROM moz_logins ORDER BY hostname;");

		while (query.next()) {
			row.clear();

			row << new QStandardItem(query.value(0).toString());
			row << new QStandardItem(query.value(1).toString());
			row << new QStandardItem(query.value(2).toString());

			models->signons.appendRow(row);
		}
		query.clear();
	}
	SQLITE_CLOSE(file)
}
