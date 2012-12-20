#include "files.h"

int	main(int argc, char* argv[]) {

	if ( argc < 2 ) {
		return EXIT_FAILURE;
	}

	QCoreApplication a(argc, argv);

	QDir		path(argv[1]);
	//QStringList	directories = path.entryList(QDir::AllDirs | QDir::NoDot | QDir::NoDotDot | QDir::Hidden | QDir::NoSymLinks);
	QStringList	files = path.entryList(QDir::Files | QDir::Hidden);

	Q_FOREACH(QString file, files) {
		QString abs_file = path.canonicalPath();
		abs_file.append("/");
		abs_file.append(file);

//		std::cout << "file is: " << file.toAscii().constData() << std::endl << "abs_file is: " << abs_file.toAscii().constData() << std::endl;

		/*
		 * QFile
		 */
		QFile	q_f(abs_file);
		if ( q_f.exists() == false ) {
			std::cerr << abs_file.toAscii().constData() << " does not exist" << std::endl;
			qCritical() << abs_file << " does not exist";
		} else {

			if ( q_f.open(QIODevice::ReadOnly) == false ) {
				std::cerr << "QFile: " << file.toAscii().constData() << " failed, code: " << q_f.error() << std::endl;
			} else {
				std::cout << "QFile : " << file.toAscii().constData() << " success" << std::endl;
				q_f.close();
			}
		}

		/*
		 * Standard fopen
		 */
		FILE*	f = fopen(abs_file.toAscii().constData(), "rb");
		if ( f == NULL )
			std::cerr << "fopen: " << abs_file.toAscii().constData() << " failed" << std::endl;
		else {
			std::cout << "fopen: " << abs_file.toAscii().constData() << " success" << std::endl;
			fclose(f);
		}

		/*
		 * QFile + fopen
		 */
		q_f.open(QIODevice::ReadOnly);
		f = fdopen(q_f.handle(), "rb");
		if ( f == NULL ) {
			std::cerr << "QFile + fopen: " << abs_file.toAscii().constData() << " failed" << std::endl;
			q_f.close();
		} else {
			std::cout << "QFile + fopen: " << abs_file.toAscii().constData() << " success" << std::endl;
			fclose(f);
			q_f.close();
		}
	}

	return EXIT_SUCCESS;
}

