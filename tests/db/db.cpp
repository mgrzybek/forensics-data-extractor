#include <QDebug>
#include "../include/database.h"
#include "thread.h"

int     main() {
	QString         file = "/tmp/test.db";
	Database        db(file);
	Thread          t1(&db);
	Thread          t2(&db);

	if ( db.exec("INSERT INTO parsed_file (file) VALUES ('/tmp/file')") == true ) {
		qDebug() << "Insertion succeeded";
	} else {
		qDebug() << "Insertion failed";
		return EXIT_FAILURE;
	}

	t1.run();
	t2.run();

	t1.wait();
	t2.wait();

	qDebug() << "End!";
	return EXIT_SUCCESS;
}

