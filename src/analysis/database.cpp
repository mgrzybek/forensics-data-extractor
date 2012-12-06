#include "analysis/database.h"

Database::Database(const QString& analysis_db_file) {
	analysis_db = QSqlDatabase::addDatabase("QSQLITE", analysis_db_file);
	analysis_db.setDatabaseName(analysis_db_file);

	if ( analysis_db.open() == false ) {
		e.msg = "Cannot connect to the database ";
		e.msg += analysis_db_file;
		e.calling_method = "Database";

		throw e;
	}

	if ( init_schema() == false ) {
		e.msg = "Cannot init the analysis database";
		e.calling_method = "Database";

		throw e;
	}
}

Database::~Database() {
	// We never unlock the mutex because the object is destroyed
	analysis_mutex.lock();

	QString connection_name = analysis_db.databaseName();
	analysis_db.close();

	QSqlDatabase::removeDatabase(connection_name);
}

bool	Database::exec(const QString& query) {
	analysis_mutex.lock();

	if ( analysis_db.transaction() == false ) {
		qCritical() << "Transaction begin failed: " << analysis_db.lastError().text();
		analysis_db.rollback();
		qCritical() << "Rollback!";
		analysis_mutex.unlock();
		return false;
	}

	if ( atomic_exec(query) == false ) {
		analysis_db.rollback();
		qCritical() << "Rollback!";
		analysis_mutex.unlock();
		return false;
	}

	if ( analysis_db.commit() == false ) {
		qCritical() << "Commit failed: " << analysis_db.lastError().text();
		analysis_db.rollback();
		qCritical() << "Rollback!";
		analysis_mutex.unlock();
		return  false;
	}

	analysis_mutex.unlock();
	return true;
}

bool	Database::exec(const QStringList& queries) {
	analysis_mutex.lock();

	if ( analysis_db.transaction() == false ) {
		qCritical() << "Transaction begin failed: " << analysis_db.lastError().text();
		analysis_db.rollback();
		analysis_mutex.unlock();
		return false;
	}

	Q_FOREACH(QString query, queries) {
		if ( atomic_exec(query) == false ) {
			analysis_db.rollback();
			qCritical() << "Rollback!";
			analysis_mutex.unlock();
			return false;
		}
	}

	if ( analysis_db.commit() == false ) {
		qCritical() << "Commit failed: " << analysis_db.lastError().text();
		analysis_db.rollback();
		qCritical() << "Rollback!";
		analysis_mutex.unlock();
		return  false;
	}

	analysis_mutex.unlock();
	return true;
}

QSqlDatabase*	Database::get_analysis_db() {
	return &analysis_db;
}

uint	Database::get_row_count(const QString& table_name) {
	QString		query;
	QSqlQuery	q(analysis_db);

	query = "SELECT COUNT(*) FROM " % table_name % " LIMIT 1;";
	q.exec(query);

	if ( q.next() == true )
		return q.value(0).toUInt();

	return 0;
}

bool	Database::init_schema() {
	QStringList	queries;

	queries << "CREATE TABLE IF NOT EXISTS place (site TEXT PRIMARY KEY, hits INTEGER NOT NULL);";
	queries << "CREATE TABLE IF NOT EXISTS cookie (name TEXT PRIMARY KEY, value TEXT NOT NULL, host TEXT NOT NULL, path TEXT NOT NULL, expiration TEXT NOT NULL, secured TEXT NOT NULL, http TEXT NOT NULL, last_accessed TEXT NOT NULL);";
	queries << "CREATE TABLE IF NOT EXISTS download (name TEXT PRIMARY KEY, source TEXT NOT NULL, mime TEXT NOT NULL);";
	queries << "CREATE TABLE IF NOT EXISTS form (host TEXT PRIMARY KEY, id TEXT NOT NULL, password TEXT NOT NULL);";
	//queries << "CREATE TABLE IF NOT EXISTS search (name TEXT PRIMARY KEY, value TEXT UNIQUE NOT NULL, hits INTEGER NOT NULL);";
	queries << "CREATE TABLE IF NOT EXISTS search (name TEXT PRIMARY KEY, hits INTEGER NOT NULL);";
	queries << "CREATE TABLE IF NOT EXISTS signon (host TEXT PRIMARY KEY, id TEXT NOT NULL, password TEXT NOT NULL );";
	queries << "CREATE TABLE IF NOT EXISTS parsed_file (file TEXT PRIMARY KEY, md5 TEXT, sha1 TEXT, analysed INTEGER DEFAULT '0');";

	queries << "CREATE VIEW IF NOT EXISTS analysed_file AS SELECT file FROM parsed_file WHERE analysed = 1";

	return exec(queries);
}

bool	Database::atomic_exec(const QString& query) {
	QSqlQuery	q(analysis_db);

	if ( q.exec(query) == false ) {
		qCritical() << "Atomic exec failed on" << query << " " << q.lastError().text();
		return false;
	}

	qDebug() << "query OK: " << query;
	return true;
}

bool	Database::insert_file(const struct_file& file) {
	QString	query = "INSERT OR IGNORE INTO parsed_file (file, md5, sha1) VALUES ('";

	query += file.full_path % "','";
	query += file.md5;
	query += "','";
	query += file.sha1;
	query += "');";

	qDebug() << "sha1 is: " << file.sha1 << " and md5 is: " << file.md5;

	return exec(query);
}

bool	Database::is_analysed_file(const struct_file& f) {
	QString		query;
	QSqlQuery	q(analysis_db);

	query = "SELECT COUNT(*) FROM analysed_file WHERE file = '";
	query += f.full_path;
	query += "' LIMIT 1;";
	q.exec(query);

	if ( q.next() == true ) {
		if ( q.value(0).toUInt() > 0 )
			return true;
	}

	return false;
}

bool	Database::is_parsed_file(const struct_file& f) {
	QString		query;
	QSqlQuery	q(analysis_db);

	query = "SELECT COUNT(*) FROM parsed_file WHERE file = '";
	query += f.full_path;
	query += "' LIMIT 1;";
	q.exec(query);

	if ( q.next() == true ) {
		if ( q.value(0).toUInt() > 0 )
			return true;
	}

	return false;
}