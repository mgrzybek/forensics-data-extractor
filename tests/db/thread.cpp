#include "thread.h"

Thread::Thread(Database* d) {
	db = d;
}

Thread::~Thread() {
	db = NULL;
}

void	Thread::run() {
	QString	name;
	QString	query;

	for ( int  i = 0 ; i < 50 ; i++ ) {
		name = "/tmp/file" + QString::number(i);
		query = "INSERT INTO parsed_file (file) VALUES ('" + name + "');";
		if ( db->exec(query) == true)
			qDebug() << "insert succedded";
		else
			qDebug() << "insert failed";
	}

}
