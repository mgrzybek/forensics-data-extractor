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
	QStandardItemModel* cookies,
	QStandardItemModel* downloads,
	QStandardItemModel* forms,
	QStandardItemModel* places,
	QStandardItemModel* search,
	QStandardItemModel* signons
	) : Web_Browser_Extractor(
		cookies,
		downloads,
		forms,
		places,
		search,
		signons
)
{
}

Firefox_Extractor::~Firefox_Extractor() {
}

void Firefox_Extractor::files_filter(const QString& file_path) {
	QStringList file_path_parts = file_path.split("/", QString::SkipEmptyParts);
	QString file = file_path_parts.at(file_path_parts.size() - 1);

	if ( file.compare("cookies.sqlite") == 0 ) {
		files_cookies << file_path;
		qDebug() << "files_filter: Cookies: " << file;
		extract_places(file_path);
	}
	if ( file.compare("downloads.sqlite") == 0 ) {
		files_downloads << file_path;
		qDebug() << "files_filter: Downloads: " << file;
		extract_places(file_path);
	}
	if ( file.compare("forms.sqlite") == 0 ) {
		files_forms << file_path;
		qDebug() << "files_filter: Forms: " << file;
		extract_forms(file_path);
	}
	if ( file.compare("places.sqlite") == 0 ) {
		files_places << file_path;
		qDebug() << "files_filter: Places: " << file;
		extract_places(file_path);
	}
	if ( file.compare("search.sqlite") == 0 ) {
		files_search << file_path;
		qDebug() << "files_filter: Files: " << file;
		extract_search(file_path);
	}
	if ( file.compare("signons.sqlite") == 0 ) {
		files_signons << file_path;
		qDebug() << "files_filter: Login: " << file;
		extract_signons(file_path);
	}
}

void Firefox_Extractor::extract_places(const QString& file) {
	{
		SQLITE_OPEN(file)

		qDebug() << "extract_places: " << file;

		QSqlQuery	query(db);
		QList<QStandardItem*>	row;

		query.exec("SELECT visit_count, url FROM moz_places ORDER BY visit_count, url;");

		while (query.next()) {
			row.clear();

			row << new QStandardItem(query.value(0).toString());
			row << new QStandardItem(query.value(1).toString());

			model_places->appendRow(row);
		}
		query.clear();
	}
	SQLITE_CLOSE(file)
}

void Firefox_Extractor::extract_cookies(const QString& file) {
	{
		SQLITE_OPEN(file)

		qDebug() << "extract_cookies: " << file;

		QSqlQuery   query(db);
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

			model_cookies->appendRow(row);
		}
		query.clear();
	}
	SQLITE_CLOSE(file)
}

void Firefox_Extractor::extract_downloads(const QString& file) {
	{
		SQLITE_OPEN(file)

		qDebug() << "extract_downloads: " << file;

		QSqlQuery   query(db);
		QList<QStandardItem*>	row;

		query.exec("SELECT name, source, mimeType FROM moz_downloads ORDER BY startTime;");

		while (query.next()) {
			row.clear();

			row << new QStandardItem(query.value(0).toString());
			row << new QStandardItem(query.value(1).toString());
			row << new QStandardItem(query.value(2).toString());

			model_downloads->appendRow(row);
		}
		query.clear();
	}
	SQLITE_CLOSE(file)
}

void Firefox_Extractor::extract_forms(const QString& file) {
	{
		SQLITE_OPEN(file)

		qDebug() << "extract_forms: " << file;

		QSqlQuery   query(db);
		QList<QStandardItem*>	row;

		query.exec("SELECT hostname, encryptedUsername, encryptedPassword FROM moz_logins ORDER BY hostname;");

		while (query.next()) {
			row.clear();

			row << new QStandardItem(query.value(0).toString());
			row << new QStandardItem(query.value(1).toString());
			row << new QStandardItem(query.value(2).toString());

			model_forms->appendRow(row);
		}
		query.clear();
	}
	SQLITE_CLOSE(file)

}

void Firefox_Extractor::extract_search(const QString& file) {
	{
		SQLITE_OPEN(file)

		qDebug() << "extract_search: " << file;

		QSqlQuery   query(db);
		QList<QStandardItem*>	row;

		query.exec("SELECT name, value FROM engine_data ORDER BY name;");

		while (query.next()) {
			row.clear();

			row << new QStandardItem(query.value(0).toString());
			row << new QStandardItem(query.value(1).toString());

			model_search->appendRow(row);
		}
		query.clear();
	}
	SQLITE_CLOSE(file)
}

void Firefox_Extractor::extract_signons(const QString& file) {
	{
		SQLITE_OPEN(file)

		qDebug() << "extract_signons: " << file;

		QSqlQuery   query(db);
		QList<QStandardItem*>	row;

		query.exec("SELECT hostname, encryptedUsername, encryptedPassword FROM moz_logins ORDER BY hostname;");

		while (query.next()) {
			row.clear();

			row << new QStandardItem(query.value(0).toString());
			row << new QStandardItem(query.value(1).toString());
			row << new QStandardItem(query.value(2).toString());

			model_signons->appendRow(row);
		}
		query.clear();
	}
	SQLITE_CLOSE(file)
}
