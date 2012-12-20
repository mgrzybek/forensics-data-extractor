#include "parser.h"

void	usage() {
	std::cout << "Usage: parser directory1 [directory2 ... directoryN]" << std::endl;
}

int	main(int argc, char* argv[]) {
	if ( argc < 2 ) {
		usage();
		return EXIT_FAILURE;
	}

	const QString	db_file("/tmp/parser.db");
	zmq::context_t	zmq_context(1);
	Database	db(db_file);
	Fake_Worker	worker(&zmq_context, ZMQ_INPROC_PARSER_PUSH); // TODO: use a centralized URI

	for (int index = 1 ; index < argc ; index++) {
		Parsing_Engine	p(&zmq_context, QString(argv[index]), &db, NULL);

		try {
			worker.start();
			p.start();
			p.wait();
		} catch (const Exception& e) {
			qCritical() << e.calling_method << ": " << e.msg;
//			worker.wait();
			return EXIT_FAILURE;
		}
	}

//	worker.wait();
	return EXIT_SUCCESS;
}

