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
		Database(const QString& db_file);
		~Database();

		bool			exec(const QString& query);
		bool			exec(const QStringList& queries);
		bool			insert_file(const struct_file& file);
//		bool			burst_insert_file(const struct_file& file);

		QSqlDatabase*	get_db();

		uint			get_row_count(const QString& table_name);

	private:
		bool			init_schema();
		bool			atomic_exec(const QString& query);

		QSqlDatabase	db;
		QMutex			mutex;
};

#endif // DATABASE_H
