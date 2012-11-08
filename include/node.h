/**
 * Project: forensics-data-extractor
 * File name: node.h
 * Description: the header of the headless worker
 *
 * @author Mathieu Grzybek on 2012-11-05
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

#ifndef NODE_H
#define NODE_H

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <csignal>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>

#include <QString>

#include "extractors/worker.h"
#include "exception.h"

int		main(int argc, char *argv[]);
void	usage();


class	Node : public QObject {
	Q_OBJECT

	public:
		Node(void* z_context, const QString& config_file, bool daemonize);
		~Node();

		void	run();

		// Unix signal handlers.
		static void	hupSignalHandler(int unused);
		static void	termSignalHandler(int unused);

	signals:
		void	stop_worker();

	public slots:
		// Qt signal handlers.
		void	handleSigHup();
		void	handleSigTerm();

	private:
		static int	sighupFd[2];
		static int	sigtermFd[2];

		QSocketNotifier	*snHup;
		QSocketNotifier	*snTerm;

		void	daemonize();

		std::string		lock_file;
		bool			daemon;

		zmq::context_t*	zmq_context;
		Worker*			worker;
		Exception		error;
};

#endif // NODE_H
