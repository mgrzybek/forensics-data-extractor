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
#include <QRegExp>
#include <QRunnable>
#include <QThread>
#include <QDebug>
#include <QtSql>

#include <zmq.hpp>

#include "../common.h"

#define SQLITE_OPEN(db_file) \
	QSqlDatabase db; \
	db = QSqlDatabase::addDatabase("QSQLITE",db_file); \
	db.setDatabaseName(db_file); \
\
	if (!db.open()) { \
		qCritical() << "Cannot connect to the database " << db_file; \
		return; \
	}

#define SQLITE_CLOSE(db_file) \
	QSqlDatabase::removeDatabase(db_file);

class Web_Browser_Extractor : public QThread
{
	public:
		Web_Browser_Extractor(void* z_context, web_browser_models* web_models);

		~Web_Browser_Extractor() = 0;

		void	run();

		virtual void	files_filter(const QString& file_path) = 0;

		virtual void	extract_places(const QString& file) = 0;
		virtual void	extract_cookies(const QString& file) = 0;
		virtual void	extract_downloads(const QString& file) = 0;
		virtual void	extract_forms(const QString& file) = 0;
		virtual void	extract_search(const QString& file) = 0;
		virtual void	extract_signons(const QString& file) = 0;

	protected:
		zmq::context_t*		zmq_context;
		QString			dir_path;
		QMap<QString, uint>	url_map;

		web_browser_models*		models;
		web_browser_analysed_files	files;

		void	append_extracted_files_to_model_files();
		void	append_files_to_model_files(const QStringList& f);

		void	update_url_map(const QString& url, const uint& count);
		void	update_model_places();
};

#endif // WEB_BROWSER_EXTRACTOR_H

