#include "analysis/router.h"

Router::Router(void* z_context, Database* db) {
	error.calling_method = "Parsing_Engine";

	if ( z_context == NULL ) {
		error.msg = "Databse is NULL";

		throw error;
	}

	if ( db == NULL ) {
		error.msg = "Databse is NULL";

		throw error;
	}

	zmq_context = (zmq::context_t*) z_context;
	database = db;
}
/*
void	Router::process(const struct_file& file) {
	error.calling_method = "Router::process";

	if ( file.name.isEmpty() == true ) {
		error.msg = "file_name is empty";
		throw error;
	}

	if ( file.full_path.isEmpty() == true ) {
		error.msg = "file_full_path is empty";
		throw error;
	}

	QString extractor = extractor_selector.select(file.name);

	if ( extractor.isEmpty() == true )
		return;
}
*/
