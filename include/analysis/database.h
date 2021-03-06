/**
 * Project: forensics-data-extractor
 * File name: database.h
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

#ifndef DATABASE_H
#define DATABASE_H

#include <QThread>
#include <QMutex>
#include <QDebug>
#include <QtSql>

#include "common.h"

class Database : public QObject
{
	Q_OBJECT

	public:
		/**
		 * Constructor
		 */
		Database(const QString& analysis_db_file);

		/**
		 * Destructor
		 */
		~Database();

		/**
		 * exec
		 *
		 * Executes SQL queries to the analysis's database
		 *
		 * @param	query		the SQL query to exec
		 * @param	queries		the SQL queries to exec within a transaction
		 *
		 * @return	true (success)
		 */
		bool	exec(const QString& query);
		bool	exec(const QStringList& queries);


		/**
		 * insert_source
		 *
		 * Insert a source file or directory into the analysis database
		 *
		 * @param	source	the source to add
		 * @param	type	its type (file or image)
		 *
		 * @return	true (success)
		 */
		bool	insert_source(const QString& source, const t_source_type& type);


		/**
		 * get_sources
		 *
		 * Gets the sources from the analysis database
		 *
		 * @param	the list to update
		 */
		void	get_sources(QStringList& sources);

		/**
		 * insert_file
		 *
		 * Executes SQL queries to insert a file
		 *
		 * @param	file	the file to insert
		 *
		 * @return	true (success)
		 */
		bool	insert_file(const struct_file& file);

		/**
		 * burst_insert_file
		 *
		 * Executes SQL queries to insert a file
		 * The transaction is committed according to an internal counter
		 *
		 * @param	file	the file to insert
		 *
		 * @return	true (success)
		 */
//		bool	burst_insert_file(const struct_file& file);

		/**
		 * is_known_file
		 *
		 * Queries the konwn files databases using sha1
		 *
		 * @param	f	the file to check
		 *
		 * @return	true (yes)
		 */
		bool	is_known_file(const struct_file& f);

		/**
		 * is_analysed_file
		 *
		 * Queries the analysed_file view to answer the question:
		 * - has this file already been analysed?
		 *
		 * @param	f	the file to check
		 *
		 * @return	true (yes)
		 */
		bool	is_analysed_file(const struct_file& f);

		/**
		 * is_parsed_file
		 *
		 * Queries the file table to answer the question:
		 * - has this file already been parsed?
		 *
		 * @param	f	the file to check
		 *
		 * @return	true (yes)
		 */
		bool	is_parsed_file(const struct_file& f);

		/**
		 * get_analysis_db
		 */
		QSqlDatabase*	get_analysis_db();

		/**
		 * get_row_count
		 *
		 * Gets the number of rows
		 *
		 * @param	table_name	 the table to check
		 * @return	the number of rows
		 *
		 */
		uint	get_row_count(const QString& table_name);

		/**
		 * walk_files_to_extract
		 *
		 * Provides a way to walk the SQL table
		 *
		 * @param	result	the struct_file to get
		 *
		 * @return	true if there is a file to get
		 */
		bool	walk_files_to_extract(struct_file& result);

	private:
		/**
		 * init_schema
		 *
		 * Creates the tables to use the analysis database
		 */
		bool	init_schema();

		/**
		 * atomic_exec
		 *
		 * This method is not protected by a mutex
		 *
		 * @param	query	the query to exec
		 */
		bool	atomic_exec(const QString& query);

		/**
		 * analysis_db
		 *
		 * The database is used to store the results
		 */
		QSqlDatabase*	analysis_db;

		/**
		 * analysis_mutex
		 *
		 * This mutex is used to be thread-safe
		 */
		QMutex		analysis_mutex;

		/**
		 * e
		 *
		 * This exception is used to report problems
		 */
		Exception	e;
};

#endif // DATABASE_H
