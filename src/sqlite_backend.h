/**
 * Project: forensics-data-extractor
 * File name: sqlite_backend.h
 * Description: describes the class used to deal with SQLite databases
 *
 * @author Mathieu Grzybek on 2012-07-20
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

#ifndef SQLITE_BACKEND_H
#define SQLITE_BACKEND_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QString>
#include <QDebug>

// Required by std::exception
#include <iostream>
#include <typeinfo>

class Sqlite_Backend
{
	public:
		Sqlite_Backend(const QString& name);
		~Sqlite_Backend();

		bool	open_db();
		void	close_db();
		bool	delete_db();
		bool	exec(const QString& query);

		QSqlError	last_error();

	private:
		QSqlDatabase	db;
		QString		db_name;
};

#endif // SQLITE_BACKEND_H
