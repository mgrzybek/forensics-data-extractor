/**
 * Project: forensics-data-extractor
 * File name: database.cpp
 * Description: describes the object creating the analysis database
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

#include "analysis/database.h"

Database::Database(const QString& analysis_db_file) {
	analysis_db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", analysis_db_file));
	analysis_db->setDatabaseName(analysis_db_file);

	if ( analysis_db->open() == false ) {
		e.msg = "Cannot connect to the database ";
		e.msg += analysis_db_file;
		e.calling_method = "Database";

		delete analysis_db;
		throw e;
	}

	if ( init_schema() == false ) {
		e.msg = "Cannot init the analysis database";
		e.calling_method = "Database";

		analysis_db->close();
		delete analysis_db;
		throw e;
	}
}

Database::~Database() {
	// We never unlock the mutex because the object is destroyed
	analysis_mutex.lock();

	QString connection_name = analysis_db->databaseName();
	analysis_db->close();
	delete analysis_db;

	QSqlDatabase::removeDatabase(connection_name);
}

bool	Database::exec(const QString& query) {
	analysis_mutex.lock();

	if ( analysis_db->transaction() == false ) {
		qCritical() << "Transaction begin failed: " << analysis_db->lastError().text();
		analysis_db->rollback();
		qCritical() << "Rollback!";
		analysis_mutex.unlock();
		return false;
	}

	if ( atomic_exec(query) == false ) {
		analysis_db->rollback();
		qCritical() << "Rollback!";
		analysis_mutex.unlock();
		return false;
	}

	if ( analysis_db->commit() == false ) {
		qCritical() << "Commit failed: " << analysis_db->lastError().text();
		analysis_db->rollback();
		qCritical() << "Rollback!";
		analysis_mutex.unlock();
		return false;
	}

	analysis_mutex.unlock();
	return true;
}

bool	Database::exec(const QStringList& queries) {
	analysis_mutex.lock();

	if ( analysis_db->transaction() == false ) {
		qCritical() << "Transaction begin failed: " << analysis_db->lastError().text();
		analysis_db->rollback();
		analysis_mutex.unlock();
		return false;
	}

	Q_FOREACH(QString query, queries) {
		if ( atomic_exec(query) == false ) {
			analysis_db->rollback();
			qCritical() << "Rollback!";
			analysis_mutex.unlock();
			return false;
		}
	}

	if ( analysis_db->commit() == false ) {
		qCritical() << "Commit failed: " << analysis_db->lastError().text();
		analysis_db->rollback();
		qCritical() << "Rollback!";
		analysis_mutex.unlock();
		return  false;
	}

	analysis_mutex.unlock();
	return true;
}

QSqlDatabase*	Database::get_analysis_db() {
	return analysis_db;
}

uint	Database::get_row_count(const QString& table_name) {
	analysis_mutex.lock();

	QString		query;
	QSqlQuery	q(*analysis_db);
	uint		result = 0;

	query = "SELECT COUNT(*) FROM " % table_name % " LIMIT 1;";
	q.exec(query);

	if ( q.next() == true )
		result = q.value(0).toUInt();

	analysis_mutex.unlock();

	return result;
}

bool	Database::init_schema() {
	QStringList	queries;

	// Web browsing
	queries << "CREATE TABLE IF NOT EXISTS place (site TEXT PRIMARY KEY, hits INTEGER NOT NULL);";
	queries << "CREATE TABLE IF NOT EXISTS cookie (name TEXT PRIMARY KEY, value TEXT NOT NULL, host TEXT NOT NULL, path TEXT NOT NULL, expiration TEXT NOT NULL, secured TEXT NOT NULL, http TEXT NOT NULL, last_accessed TEXT NOT NULL);";
	queries << "CREATE TABLE IF NOT EXISTS download (name TEXT PRIMARY KEY, source TEXT NOT NULL, mime TEXT NOT NULL);";
	queries << "CREATE TABLE IF NOT EXISTS form (host TEXT PRIMARY KEY, id TEXT NOT NULL, password TEXT NOT NULL);";
	//queries << "CREATE TABLE IF NOT EXISTS search (name TEXT PRIMARY KEY, value TEXT UNIQUE NOT NULL, hits INTEGER NOT NULL);";
	queries << "CREATE TABLE IF NOT EXISTS search (name TEXT PRIMARY KEY, hits INTEGER NOT NULL);";
	queries << "CREATE TABLE IF NOT EXISTS signon (host TEXT PRIMARY KEY, id TEXT NOT NULL, password TEXT NOT NULL );";

	// Sources
	queries << "CREATE TABLE IF NOT EXISTS source (source TEXT, type TEXT, PRIMARY KEY (source));";
	// Files
#ifdef QT_5
	queries << "CREATE TABLE IF NOT EXISTS parsed_file (source TEXT, file TEXT, inode INTEGER DEFAULT NULL, size INTEGER NOT NULL, md5 TEXT, sha1 TEXT, analysed INTEGER DEFAULT '0', known TEXT default NULL, mime_type TEXT NOT NULL, PRIMARY KEY(source, inode));";
#else
	queries << "CREATE TABLE IF NOT EXISTS parsed_file (source TEXT, file TEXT, inode INTEGER DEFAULT NULL, size INTEGER NOT NULL, md5 TEXT, sha1 TEXT, analysed INTEGER DEFAULT '0', known TEXT default NULL, PRIMARY KEY(source, inode));";
#endif

	queries << "CREATE VIEW IF NOT EXISTS analysed_file AS SELECT file FROM parsed_file WHERE analysed = 1";
	queries << "CREATE VIEW IF NOT EXISTS not_analysed_file AS SELECT file FROM parsed_file WHERE analysed = 0";

	queries << "CREATE VIEW IF NOT EXISTS known_file AS SELECT file,known FROM parsed_file WHERE known NOT NULL ORDER BY known,file ASC";

	return exec(queries);
}

bool	Database::atomic_exec(const QString& query) {
	QSqlQuery	q(*analysis_db);

	qDebug() << query;

	if ( q.exec(query) == false ) {
		qCritical() << "Atomic exec failed on" << query << " " << q.lastError().text();
		return false;
	}

	return true;
}

bool	Database::insert_source(const QString& source, const t_source_type& type) {
	QString	query;

	if ( source.isEmpty() == true ) {
		qCritical() << "Database::insert_source: the source is empty!";
		return false;
	}

	query = "INSERT INTO source (source, type) VALUES ('";
	query += source % "','";

	if ( type == IMAGE )
		query += "image');";
	else
		query += "directory');";

	return exec(query);
}


void	Database::get_sources(QStringList& sources) {
	QString		query = "SELECT source FROM source;";
	QSqlQuery	q(*analysis_db);

	q.exec(query);

	while ( q.next() == true ) {
		sources << q.value(0).toString();
	}
}

bool	Database::insert_file(const struct_file& file) {
	QString	query;

#ifndef QT_5
	if ( file.inode > -1 )
		query = "INSERT INTO parsed_file (inode, source, file, size, md5, sha1) VALUES ('" % QString::number(file.inode) % "','";
	else
		query = "INSERT INTO parsed_file (source, file, size, md5, sha1) VALUES ('";
#else
	if ( file.inode > -1 )
		query = "INSERT INTO parsed_file (mime_type, inode, source, file, size, md5, sha1, mime_type) VALUES ('" % QString::number(file.inode) % "','";
	else
		query = "INSERT INTO parsed_file (mime_type, source, file, size, md5, sha1) VALUES ('";

	query += file.mime_type.name() % "','";
#endif
	// TODO: understand why formatValue does not work
	//query += analysis_db->driver()->formatValue(file.full_path) % "','";
	query += file.source % "','";
	query += file.full_path % "','";
	query += QString::number(file.size) % "','";
	query += file.md5 % "','";
	query += file.sha1 % "');";

	return exec(query);
}

bool	Database::is_analysed_file(const struct_file& f) {
	QString	query;
	QSqlQuery	q(*analysis_db);

	query = "SELECT COUNT(*) FROM analysed_file WHERE file = '";
	query += f.full_path;
	query += "' LIMIT 1;";
	q.exec(query);

	if ( q.next() == true ) {
		if ( q.value(0).toUInt() > 0 ) {
			analysis_mutex.unlock();
			return true;
		}
	}

	return false;
}

bool	Database::is_parsed_file(const struct_file& f) {
	QString	query;
	QSqlQuery	q(*analysis_db);

	query = "SELECT COUNT(*) FROM parsed_file WHERE file = '";
	query += f.full_path;
	query += " AND source = '";
	query += f.source;
	query += "' LIMIT 1;";
	q.exec(query);

	if ( q.next() == true ) {
		if ( q.value(0).toUInt() > 0 )
			return true;
	}

	return false;
}

bool	Database::walk_files_to_extract(struct_file& result) {
	static QSqlQuery	q(*analysis_db);
	QString			query;

#ifdef QT_5
	query = "SELECT source,inode,file,size,sha1,md5,mime_type FROM parsed_file;";
#else
	query = "SELECT source,inode,file,size,sha1,md5 FROM parsed_file;";
#endif

	if ( q.isValid() == false )
		q.exec(query);

	if ( q.next() == true ) {
		result.source = q.value(0).toString();
		result.inode  = q.value(1).toUInt();
		result.full_path = q.value(2).toString();
		result.size = q.value(3).toUInt();
		result.sha1 = q.value(4).toString();
		result.md5 = q.value(5).toString();
#ifdef QT_5
		result.mime_type = q.value(6).toString();
#endif
		return true;
	}

	q.clear();
	return false;
}

