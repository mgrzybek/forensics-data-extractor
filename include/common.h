/**
 * Project: forensics-data-extractor
 * File name: common.h
 * Description: describes the custom data types
 *
 * @author Mathieu Grzybek on 2012-06-09
 * @copyright 20?? Mathieu Grzybek. All rights reserved.
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

#ifndef COMMON_H
#define COMMON_H

#include <openssl/md5.h>
#include <openssl/sha.h>

#include <QStandardItemModel>
#include <QSqlTableModel>
#include <QStringList>
#include <QString>
#include <QHash>

#include "exception.h"
//#include "databases/generic_database.h"

#define SQLITE_OPEN(db_file) \
	QSqlDatabase db; \
	db = QSqlDatabase::addDatabase("QSQLITE",db_file); \
	db.setDatabaseName(db_file); \
\
	if ( db.open() == false ) { \
		qCritical() << "Cannot connect to the database " << db_file; \
		return; \
	}

#define SQLITE_CLOSE(db_file) \
	QSqlDatabase::removeDatabase(db_file);

#ifdef WINDOWS_OS
#define ZMQ_INPROC_PARSER_PUSH	"tcp://127.0.0.1:5555"
//#define ZMQ_INPROC_WORKER_PUSH	"tcp://127.0.0.1:5556"
#define	ZMQ_INPROC_RECEIVER_PULL	"tcp://127.0.0.1:5557"
#else
#define ZMQ_INPROC_PARSER_PUSH	"inproc://push-parser.inproc"
//#define ZMQ_INPROC_WORKER_PUSH	"inproc://push-worker.inproc"
#define	ZMQ_INPROC_RECEIVER_PULL	"inproc://pull-receiver.inproc"
#endif

#define	ZMQ_QUERIES_SEPARATOR	"|"

typedef	QList<QRegExp>	regex_list;
typedef	QHash<QString, regex_list>	h_dico;

typedef struct {
	QString	source;
	qint64	inode;

	QString	name;
	QString	full_path;
	qint64	size;

#ifdef QT_5
	QMimeType	mime_type;
#endif

	QString	sha1;
	QString	md5;

	QString	extractor;
	QString	node;
} struct_file;

typedef struct {
	QSqlTableModel*	cookies;
	QSqlTableModel*	downloads;
	QSqlTableModel*	forms;
	QSqlTableModel*	places;
	QSqlTableModel*	searches;
	QSqlTableModel*	signons;
	QSqlTableModel*	extracted_files;
} web_browser_models;

typedef QHash<QString, QString>			h_known_db_config;
typedef QHash<QString, h_known_db_config>	hh_known_db_config;

typedef enum {
	DIRECTORY,
	IMAGE
} t_source_type;

#endif // COMMON_H

