/**
 * Project: forensics-data-extractor
 * File name: sqlite_backend.cpp
 * Description: describes the class used to deal with SQLite databases
 *
 * @author Mathieu Grzybek on 2012-07-20
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

#include "sqlite_backend.h"

Sqlite_Backend::Sqlite_Backend(const QString& name)
{
	db_name = name;
}

Sqlite_Backend::~Sqlite_Backend()
{
	close_db();
}

bool Sqlite_Backend::open_db()
{
//	if ( db.isOpen() == true )
//		return true;

	// Find QSLite driver
	db = QSqlDatabase::addDatabase("QSQLITE", db_name);
	db.setDatabaseName(db_name);

	// Open databasee
	return db.open();
}

void Sqlite_Backend::close_db()
{
	db.close();
}

bool Sqlite_Backend::delete_db()
{
	close_db();

	if ( db_name.compare(":memory:", Qt::CaseInsensitive) != 0 )
		return QFile::remove(db_name);

	return true;
}

bool Sqlite_Backend::exec(const QString& query) {
	db.exec(query);

	if ( db.lastError().isValid() == true ) {
		qCritical() << db.lastError().text() << " : " << query;
		return false;
	}

	return true;
}

QSqlError Sqlite_Backend::last_error()
{
	// If opening database has failed user can ask
	// error description by QSqlError::text()
	return db.lastError();
}
