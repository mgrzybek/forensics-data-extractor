/**
 * Project: forensics-data-extractor
 * File name: web_browser_extractor.h
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

#ifndef WEB_BROWSER_EXTRACTOR_H
#define WEB_BROWSER_EXTRACTOR_H

#include <QStandardItemModel>
#include <QFileSystemModel>
#include <QRunnable>
#include <QThread>
#include <QDebug>
#include <QtSql>

#include <zmq.hpp>

#define SQLITE_OPEN(db_file) \
	QSqlDatabase db; \
	db = QSqlDatabase::addDatabase("QSQLITE",db_file); \
	db.setDatabaseName(db_file); \
	\
	if (!db.open()) { \
		qCritical() << "Cannoct connect to the database"; \
		return; \
	} \
	\
	qDebug() << "Connected to " << db_file;

#define SQLITE_CLOSE(db_file) \
	QSqlDatabase::removeDatabase(db_file);

class Web_Browser_Extractor : public QThread
{
public:
	Web_Browser_Extractor(
		QStandardItemModel* cookies,
		QStandardItemModel* downloads,
		QStandardItemModel* forms,
		QStandardItemModel* places,
		QStandardItemModel* search,
		QStandardItemModel* signons
	);

	~Web_Browser_Extractor();

	void	run();

	virtual void	files_filter(const QString& file_path) = 0;

	virtual void	extract_places(const QString& file) = 0;
	virtual void	extract_cookies(const QString& file) = 0;
	virtual void	extract_downloads(const QString& file) = 0;
	virtual void	extract_forms(const QString& file) = 0;
	virtual void	extract_search(const QString& file) = 0;
	virtual void	extract_signons(const QString& file) = 0;

protected:
	QString			dir_path;

	// cookies
	QStringList		files_cookies;
	QStandardItemModel*	model_cookies;
	// downloads
	QStringList		files_downloads;
	QStandardItemModel*	model_downloads;
	// forms
	QStringList		files_forms;
	QStandardItemModel*	model_forms;
	// places
	QStringList		files_places;
	QStandardItemModel*	model_places;
	// search
	QStringList		files_search;
	QStandardItemModel*	model_search;
	// signons
	QStringList		files_signons;
	QStandardItemModel*	model_signons;
};

#endif // WEB_BROWSER_EXTRACTOR_H
