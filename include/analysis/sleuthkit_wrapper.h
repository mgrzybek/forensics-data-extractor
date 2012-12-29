/**
 * Project: forensics-data-extractor
 * File name: sleuthkit_wrapper.h
 * Description: describes the usage of Sleuthkit to process forensic images
 *
 * @author Mathieu Grzybek on 2012-12-21
 * @copyright 20?? Mathieu Grzybek. All rights reserved.
 * @version $Id: code-gpl-license.txt,v 1.2 2004/05/04 13:19:30 garry Exp $
 *
 * @see The GNU Public License (GPL) version 3 or higher
 *
 *
 * forensics-data-extractor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
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

#ifndef SLEUTHKIT_WRAPPER_H
#define SLEUTHKIT_WRAPPER_H

#include <tsk3/libtsk.h>
#include <zmq.hpp>

#include "common.h"
#include "database.h"
#include "checksum.h"

class Sleuthkit_Wrapper
{
	public:
		/**
		 * Sleuthkit_Wrapper
		 *
		 * The constructor of the class
		 * It will send messages using ZMQ
		 *
		 * @param	socket	the ZMQ socket to use
		 * @param	db	the database used to store the results
		 */
		Sleuthkit_Wrapper(zmq::socket_t* z_socket, Database* db, const QString& source_path);

		/**
		 * Sleuthkit_Wrapper
		 *
		 * The constructor of the class
		 * It will not send messages using ZMQ
		 *
		 * @param	db	the database used to store the results
		 */
		Sleuthkit_Wrapper(Database* db, const QString& source_path);

		/**
		 * image_process
		 *
		 */
		void	image_process();

	private:
		/**
		 * image_process
		 *
		 * @param	image_path	the forensics image to process
		 */
		void	image_process(const QString& image_path);

		/**
		 * Open a directory and cycle through its contents.  Read each file and recurse
		 * into each directory.
		 *
		 * @param fs_info File system to process
		 * @param stack Stack to prevent infinite recursion loops
		 * @param dir_inum Metadata address of directory to open
		 *
		 * @param path Path of directory being opened
		 * @returns 1 on error
		 */
		uint8_t procDir(TskFsInfo * fs_info, TSK_STACK * stack, TSK_INUM_T dir_inum, const char *path);

		/**
		* Process the data as a volume system to find the partitions
		 * and volumes.
		 * File system analysis will be performed on each partition.
		 *
		 * @param img Image file information structure for data to analyze
		 * @param start Byte offset to start analyzing from.
		 *
		 * @return 1 on error and 0 on success
		 */
		uint8_t procVs(TskImgInfo * img_info, TSK_OFF_T start);

		/**
		* Analyze the volume starting at byte offset 'start' and look
		* for a file system.  When found, the files will be analyzed.
		*
		* @param img Disk image to be analyzed.
		* @param start Byte offset of volume starting location.
		*
		* @return 1 on error and 0 on success
		*/
		uint8_t procFs(TskImgInfo * img_info, TSK_OFF_T start);

		/**
		 * socket
		 *
		 * The ZMQ socket used to send messages
		 */
		void	send_zmq(const std::string& message);

		/**
		 * e
		 *
		 * This exception is used to report problems
		 */
		Exception	e;

		/**
		 * socket
		 *
		 * The ZMQ socket used to send messages
		 */
		zmq::socket_t*	socket;

		/**
		 * database
		 *
		 * The database object to use to store the results
		 */
		Database*	database;

		/**
		 * hdb_info
		 *
		 * Used to process hash files
		 * TODO: use another object
		 */
		//TskHdbInfo	*hdb_info;

		/**
		 * source_image_path
		 *
		 * The image file of process
		 */
		QString		source_image_path;

#ifdef QT_5
		/**
		 * mime_type_database
		 *
		 * This object is used to determine files' MIMI-Types
		 */
		QMimeDatabase	mime_type_database;
#endif
};

#endif // SLEUTHKIT_WRAPPER_H
