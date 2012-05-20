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

#include "chrome_extractor.h"

Chrome_Extractor::Chrome_Extractor(
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

Chrome_Extractor::~Chrome_Extractor() {
}

void Chrome_Extractor::files_filter(const QString& file_path) {
	QStringList file_path_parts = file_path.split("/", QString::SkipEmptyParts);
	QString file = file_path_parts.at(file_path_parts.size() - 1);

	if ( file.compare("Cookies") == 0 ) {
		files_cookies << file_path;
		qDebug() << "files_filter: Cookies: " << file;
		extract_places(file_path);
	}
	if ( file.compare("History") == 0 ) {
		files_downloads << file_path;
		qDebug() << "files_filter: Downloads: " << file;
		extract_downloads(file_path);

		files_places << file_path;
		qDebug() << "files_filter: Places: " << file;
		extract_places(file_path);

		files_search << file_path;
		qDebug() << "files_filter: Files: " << file;
		extract_search(file_path);
	}
	if ( file.compare("Login Data") == 0 ) {
		files_signons << file_path;
		qDebug() << "files_filter: Login: " << file;
		extract_signons(file_path);
	}
}

void Chrome_Extractor::extract_places(const QString& file) {
	{
		SQLITE_OPEN(file)

		qDebug() << "extract_places: " << file;

		QSqlQuery   query(db);
		QList<QStandardItem*>	row;

		query.exec("SELECT visit_count, url FROM urls ORDER BY visit_count, url;");

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

void Chrome_Extractor::extract_cookies(const QString& file) {
	{
		SQLITE_OPEN(file)

		qDebug() << "extract_cookies: " << file;

		QSqlQuery   query(db);
		QList<QStandardItem*>	row;

		query.exec("SELECT name, value, host_key, path, has_expires, secure, httponly, last_access_utc FROM cookies ORDER BY last_access_utc;");

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

void Chrome_Extractor::extract_downloads(const QString& file) {
	{
		SQLITE_OPEN(file)

		qDebug() << "extract_downloads: " << file;

		QSqlQuery   query(db);
		QList<QStandardItem*>	row;

		query.exec("SELECT full_path, url FROM downloads ORDER BY start_time;");

		while (query.next()) {
			row.clear();

			row << new QStandardItem(query.value(0).toString());
			row << new QStandardItem(query.value(1).toString());

			model_downloads->appendRow(row);
		}
		query.clear();
	}
	SQLITE_CLOSE(file)
}

void Chrome_Extractor::extract_forms(const QString& file) {
	{
		SQLITE_OPEN(file)

		qDebug() << "extract_places: " << file;

		QSqlQuery   query(db);
		QList<QStandardItem*>	row;

		query.exec("SELECT action_url, username_value, password_value FROM logins ORDER BY action_url;");

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

void Chrome_Extractor::extract_search(const QString& file) {
	{
		SQLITE_OPEN(file)

		qDebug() << "extract_search: " << file;

		QSqlQuery   query(db);
		QList<QStandardItem*>	row;

		query.exec("SELECT k.term, u.url FROM keyword_search_terms k, urls u WHERE k.url_id = u.id ORDER BY k.term;");

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

void Chrome_Extractor::extract_signons(const QString& file) {
	{
		SQLITE_OPEN(file)

		qDebug() << "extract_signons: " << file;

		QSqlQuery   query(db);
		QList<QStandardItem*>	row;

		query.exec("SELECT action_url, username_value, password_value FROM logins ORDER BY action_url;");

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
