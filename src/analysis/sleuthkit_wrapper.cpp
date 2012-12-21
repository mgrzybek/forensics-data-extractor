/**
 * Project:
 * File name:
 * Description:
 *
 * @author Mathieu Grzybek on 20??-??-??
 * @copyright 20?? Mathieu Grzybek. All rights reserved.
 * @version $Id: code-gpl-license.txt,v 1.2 2004/05/04 13:19:30 garry Exp $
 *
 * @see The GNU Public License (GPL) version 3 or higher
 *
 *
 * ? is free software; you can redistribute it and/or modify
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

#include "analysis/sleuthkit_wrapper.h"

Sleuthkit_Wrapper::Sleuthkit_Wrapper(zmq::socket_t* z_socket, Database* db) {
	if ( z_socket == NULL ) {
		e.calling_method = "Sleuthkit_Wrapper";
		e.msg = "z_socket is NULL";
	}

	if ( db == NULL ) {
		e.calling_method = "Sleuthkit_Wrapper";
		e.msg = "db is NULL";
	}

	socket	= z_socket;
	database = db;
	hdb_info = NULL;
}

Sleuthkit_Wrapper::Sleuthkit_Wrapper(Database* db) {
	if ( db == NULL ) {
		e.calling_method = "Sleuthkit_Wrapper";
		e.msg = "db is NULL";
	}

	socket	= NULL;
	database = db;
	hdb_info = NULL;
}

void	Sleuthkit_Wrapper::image_process(const QString& image_path) {
	TskImgInfo *img_info = new TskImgInfo();

#if DO_HASHLOOKUP
	/* Setup hash infrastructure */
	if ((hdb_info = tsk_hdb_open(_TSK_T("/XXX/NSRLFile.txt"), TSK_HDB_OPEN_NONE)) == NULL) {
		delete img_info;
		tsk_error_print(stderr);
	}

	if (tsk_hdb_hasindex(hdb_info, TSK_HDB_HTYPE_MD5_ID) == 0) {
		delete img_info;
		fprintf(stderr, "Hash database does not have an index (create one using hfind -i nsrl-md5 HASHFILE\n");
	}
#endif

	if (img_info->open(image_path.toStdString().c_str(), TSK_IMG_TYPE_DETECT, 0) == 1) {
		delete img_info;
		fprintf(stderr, "Error opening file\n");
		tsk_error_print(stderr);
	}

	if (procVs(img_info, 0)) {
		delete img_info;
		tsk_error_print(stderr);
	}

	delete img_info;
}

uint8_t	Sleuthkit_Wrapper::procDir(TskFsInfo * fs_info, TSK_STACK * stack, TSK_INUM_T dir_inum, const char *path) {
	TskFsDir	*fs_dir = new TskFsDir();
	size_t		i;
	char*		path2 = NULL;
	char*		buf = NULL;

	// open the directory
	if ((fs_dir->open(fs_info, dir_inum)) == 1) {
		fprintf(stderr, "Error opening directory: %" PRIuINUM "\n", dir_inum);
		tsk_error_print(stderr);
		return 1;
	}

	/* These should be dynamic lengths, but this is just a sample program.
	 * Allocate heap space instead of stack to prevent overflow for deep
	 * directories. */
	if ((path2 = (char *) malloc(4096)) == NULL) {
		return 1;
	}

	if ((buf = (char *) malloc(2048)) == NULL) {
		return 1;
	}

	// cycle through each entry
	for (i = 0; i < fs_dir->getSize(); i++) {
		TskFsFile *fs_file;
		TSK_OFF_T off = 0;
		size_t len = 0;

		// get the entry
		if ((fs_file = fs_dir->getFile(i)) == NULL) {
			fprintf(stderr, "Error getting directory entry %" PRIuSIZE " in directory %" PRIuINUM "\n", i, dir_inum);
			tsk_error_print(stderr);

			free(path2);
			free(buf);
			return 1;
		}

		/* Ignore NTFS System files */
		if ((TSK_FS_TYPE_ISNTFS(fs_file->getFsInfo()->getFsType())) &&
			(const_cast<TskFsName *>(fs_file->getName())->getName()[0] == '$')) {
			fs_file->close();
			continue;
		}

		//printf("Processing %s/%s\n", path, fs_file->name->name);

		// make sure it's got metadata and not only a name
		if (fs_file->getMeta()) {
			ssize_t cnt;

			/*
			 * Note that we could also cycle through all of the attributes in the
			 * file by using one of the tsk_fs_attr_get() functions and reading it
			 * with tsk_fs_attr_read().  See the File Systems section of the Library
			 * User's Guide for more details:
			 * http://www.sleuthkit.org/sleuthkit/docs/api-docs/
			 */

			// read file contents
			if (fs_file->getMeta()->getType() == TSK_FS_META_TYPE_REG) {
				struct_file	s_file;
				TSK_OFF_T	fSize = fs_file->getMeta()->getSize();
				Checksum	checksum_calculator;
				int		myflags = 0;

				s_file.full_path = path;
				s_file.full_path += "/";
				s_file.full_path += fs_file->getName()->getName();
				s_file.md5 = "";
				s_file.sha1 = "";

				if ( database->is_parsed_file(s_file) == true ) {
					qDebug() << "already parsed:" << s_file.full_path;
					continue;
				}

				checksum_calculator.init();

				for ( off = 0 ; off < fSize ; off += len ) {
					if (fSize - off < 2048)
						len = (size_t) (fSize - off);
					else
						len = 2048;

					cnt = fs_file->read(off, buf, len, (TSK_FS_FILE_READ_FLAG_ENUM) myflags);
					if (cnt == -1) {
						// could check tsk_errno here for a recovery error (TSK_ERR_FS_RECOVER)
						fprintf(stderr, "Error reading %s file: %s\n", ((fs_file->getName()->getFlags() & TSK_FS_NAME_FLAG_UNALLOC) || (fs_file->getMeta()->getFlags() & TSK_FS_META_FLAG_UNALLOC)) ? "unallocated" : "allocated", fs_file->getName()->getName());
						tsk_error_print(stderr);
						break;
					}
					else if (cnt != (ssize_t) len) {
						fprintf(stderr, "Warning: %" PRIuSIZE " of %" PRIuSIZE " bytes read from %s file %s\n", cnt, len, ((fs_file->getName()->getFlags() & TSK_FS_NAME_FLAG_UNALLOC) || (fs_file->getMeta()->getFlags() & TSK_FS_META_FLAG_UNALLOC)) ? "unallocated" : "allocated", fs_file->getName()->getName());
					}

					// do something with the data...
					checksum_calculator.update(cnt, (uchar)*buf);
				}
				checksum_calculator.get_final(&s_file);

				// TODO: add known files databases support (NSRL) to prevent the ZMQ message to be sent
				if ( socket != NULL )
					send_zmq(s_file.full_path.toAscii().constData());
				if ( database->insert_file(s_file) == false )
					qCritical() << "Cannot insert " << s_file.full_path;
			}

			// recurse into another directory (unless it is a '.' or '..')
			else if (fs_file->getMeta()->getType() == TSK_FS_META_TYPE_DIR) {
				if (TSK_FS_ISDOT(fs_file->getName()->getName()) == 0) {

					// only go in if it is not on our stack
					if (tsk_stack_find(stack, fs_file->getMeta()->getAddr()) == 0) {
						// add the address to the top of the stack
						tsk_stack_push(stack, fs_file->getMeta()->getAddr() );

						snprintf(path2, 4096, "%s/%s", path, fs_file->getName()->getName());
						if (procDir(fs_info, stack, fs_file->getMeta()->getAddr(), path2)) {
							fs_file->close();
							fs_dir->close();
							free(path2);
							free(buf);
							return 1;
						}

						// pop the address
						tsk_stack_pop(stack);
					}
				}
			}
		}
		fs_file->close();
	}
	fs_dir->close();

	free(path2);
	free(buf);
	return 0;
}

uint8_t	Sleuthkit_Wrapper::procFs(TskImgInfo * img_info, TSK_OFF_T start) {
	TskFsInfo *fs_info = new TskFsInfo();
	TSK_STACK *stack;

	/* Try it as a file system */
	if ((fs_info->open(img_info, start, TSK_FS_TYPE_DETECT)) == 1)
	{
		fprintf(stderr,
			"Error opening file system in partition at offset %" PRIuOFF
			"\n", start);
		tsk_error_print(stderr);

		/* We could do some carving on the volume data at this point */

		return 1;
	}

	// create a stack to prevent infinite loops
	stack = tsk_stack_create();

	// Process the directories
	if (procDir(fs_info, stack, fs_info->getRootINum(), "")) {
		fprintf(stderr, "Error processing file system in partition at offset %" PRIuOFF "\n", start);
		delete fs_info;
		return 1;
	}

	tsk_stack_free(stack);

	/* We could do some analysis of unallocated blocks at this point...  */


	delete fs_info;
	return 0;
}

uint8_t	Sleuthkit_Wrapper::procVs(TskImgInfo * img_info, TSK_OFF_T start) {
	TskVsInfo *vs_info = new TskVsInfo();

	// Open the volume system
	if ((vs_info->open(img_info, start, TSK_VS_TYPE_DETECT)) == 1) {
		if (tsk_verbose)
			fprintf(stderr, "Error determining volume system -- trying file systems\n");

		/* There was no volume system, but there could be a file system */
		tsk_error_reset();
		if (procFs(img_info, start)) {
			return 1;
		}
	}
	else {
		// cycle through the partitions
		for (TSK_PNUM_T i = 0; i < vs_info->getPartCount(); i++) {
			const TskVsPartInfo *vs_part;

			if ((vs_part = vs_info->getPart(i)) == NULL) {
				fprintf(stderr, "Error getting volume %" PRIuPNUM "\n", i);
				continue;
			}

			// ignore the metadata partitions
			if (const_cast<TskVsPartInfo *>(vs_part)->getFlags() & TSK_VS_PART_FLAG_META)
				continue;

			// could do something with unallocated volumes
			else if (const_cast<TskVsPartInfo *>(vs_part)->getFlags() & TSK_VS_PART_FLAG_UNALLOC) {

			}
			else {
				if (procFs(img_info,
					const_cast<TskVsPartInfo *>(vs_part)->getStart() * vs_info->getBlockSize())) {
					// We could do more fancy error checking here to see the cause
					// of the error or consider the allocation status of the volume...
					tsk_error_reset();
				}
			}
		}
		vs_info->close();
	}
	return 0;
}
void	Sleuthkit_Wrapper::send_zmq(const std::string& message) {
	zmq::message_t	z_msg(message.size() + 1);
	snprintf((char*)z_msg.data(), message.size() + 1, "%s", message.c_str());
	socket->send(z_msg);
}
