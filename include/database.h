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
		/*
		 * Constructor
		 */
		Database(const QString& analysis_db_file);

		/*
		 * Destructor
		 */
		~Database();

		/*
		 * exec
		 *
		 * Executes SQL queries to the analysis's database
		 *
		 * @arg	query	:	the SQL query to exec
		 * @arg	queries	:	the SQL queries to exec within a transaction
		 *
		 * @return	true (success)
		 */
		bool			exec(const QString& query);
		bool			exec(const QStringList& queries);

		/*
		 * insert_file
		 *
		 * Executes SQL queries to insert a file
		 *
		 * @arg	file	:	the file to insert
		 *
		 * @return	true (success)
		 */
		bool			insert_file(const struct_file& file);

		/*
		 * burst_insert_file
		 *
		 * Executes SQL queries to insert a file
		 * The transaction is committed according to an internal counter
		 *
		 * @arg	file	:	the file to insert
		 *
		 * @return	true (success)
		 */
//		bool			burst_insert_file(const struct_file& file);

		/*
		 * is_known_file
		 *
		 * Queries the konwn files databases using sha1
		 *
		 * @arg	f	:	the file to check
		 *
		 * @return	true (yes)
		 */
		bool			is_known_file(const struct_file& f);

		/*
		 * get_analysis_db
		 */
		QSqlDatabase*	get_analysis_db();

		/*
		 * get_row_count
		 *
		 * Gets the number of rows
		 *
		 * @arg	table_name	:	 the table to check
		 * @return	: the number of rows
		 *
		 */
		uint			get_row_count(const QString& table_name);

	private:
		/*
		 * init_schema
		 *
		 * Creates the tables to use the analysis database
		 */
		bool			init_schema();

		/*
		 * atomic_exec
		 *
		 * This method is not protected by a mutex
		 *
		 * @arg	query	: the query to exec
		 *
		 */
		bool			atomic_exec(const QString& query);

		QSqlDatabase	analysis_db;
		QMutex			analysis_mutex;

		Exception		e;
};

#endif // DATABASE_H
