#ifndef ROUTER_H
#define ROUTER_H

#include <zmq.hpp>

#include "exception.h"
#include "analysis/database.h"
#include "extractors/extractor_select.h"

class Router
{
	public:
		Router(void* z_context, Database* db);

		void process(const struct_file& file_path);

	private:
		zmq::context_t*	zmq_context;
		Extractor_Select	extractor_selector;

		Database*	database;

		Exception	error;
};

#endif // ROUTER_H
