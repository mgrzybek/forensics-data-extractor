/**
 * Project: forensics-data-extractor
 * File name: nsrl.cpp
 * Description: describes the NSRL interface
 *
 * @author Mathieu Grzybek on 2012-10-29
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

#include "databases/nsrl.h"

NSRL::NSRL(const h_known_db_config& config) {
	error.calling_method = "NSRL::NSRL";

	if ( config.contains("hostname") == false ) {
		error.msg = "Cannot find the hostname";
		throw error;
	}
	db.setHostName(config.value("hostname"));

	if ( config.contains("port") == true )
		db.setPort(config.value("port").toInt());

	if ( config.contains("database") == false ) {
		error.msg = "Cannot find the database";
		throw error;
	}
	db.setDatabaseName(config.value("database"));

	if ( config.contains("username") == true )
		db.setUserName(config.value("username"));

	if ( config.contains("password") == true )
		db.setPassword(config.value("password"));

	if ( config.contains("driver") == false ) {
		error.msg = "Cannot fint the driver to use";
		throw error;
	}

	db = QSqlDatabase::addDatabase(config.value("driver"), "nsrl");
}

NSRL::~NSRL() {
	db.close();
	QSqlDatabase::removeDatabase("nsrl");
}

bool	NSRL::is_known(const struct_file& file) {
	QSqlQuery	query(db);
	QString		sql = "SELECT COUNT(*) FROM hash WHERE sha1 = '";

	sql += QString((char*)file.sha1);
	sql += "' LIMIT 1;";

	error.calling_method = "NSRL::is_known";

	if ( query.exec(sql) == false ) {
		error.msg = "Cannot exec the query: ";
		error.msg += query.lastError().text();
		throw error;
	}

	if ( query.next() == true) {
		if ( query.value(0).toInt() > 0 )
			return true;
		else
			return false;
	} else {
		error.msg = "No returned data from the query: ";
		error.msg += sql;
		throw error;
	}

	// We should never arrive here
	return false;
}
