/**
 * Project: forensics-data-extractor
 * File name: checksum.cpp
 * Description: describes the checksum hashes used by the program
 *
 * @author Mathieu Grzybek on 2012-10-24
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

#include "analysis/checksum.h"

Checksum::Checksum() {
}

bool	Checksum::process_all(struct_file* file) {
#ifdef WINDOWS_OS
	FILE*	opened_file = NULL;

	if ( fopen_s(&opened_file, file->full_path.toLatin1().constData(), "rb") != 0 ) {
		qCritical() << e.calling_method << ": Cannot open " << file->full_path;
		return false;
	}
#else
	FILE*	opened_file = fopen(file->full_path.toLatin1().constData(), "rb");
#endif
	int	bytes;
	uchar	data[1024];

	if ( init() == false ) {
		fclose(opened_file);
		return false;
	}

	if ( opened_file == NULL ) {
		qCritical() << "Cannot open " << file->full_path << " for sha1";
		return false;
	}

	try {
		 while ( ( bytes = fread(data, 1, 1024, opened_file) ) != 0 ) {
			if ( update(bytes, data) == false) {
				fclose(opened_file);
				return false;
			}
		}
	} catch (const std::exception& ex) {
		qCritical() << ex.what();
		fclose(opened_file);
		return false;
	}

	fclose(opened_file);

	return get_final(file);
}

bool	Checksum::init() {
	if ( SHA1_Init(&sha1_ctx) == 0 || MD5_Init(&md5_ctx) == 0 ) {
		qCritical() << "Cannot init hash engines";
		return false;
	}
	return true;
}

bool	Checksum::update(const int& bytes, const uchar data[1024]) {
	if ( SHA1_Update(&sha1_ctx, data, bytes) == 0 || MD5_Update(&md5_ctx, data, bytes) == 0 ) {
		qCritical() << "Cannot update hash";
		return false;
	}
	return true;
}

bool	Checksum::update(const int& bytes, const char data) {
	if ( SHA1_Update(&sha1_ctx, &data, bytes) == 0 || MD5_Update(&md5_ctx, &data, bytes) == 0 ) {
		qCritical() << "Cannot update hash";
		return false;
	}
	return true;
}

bool	Checksum::get_final(struct_file* file) {
	if ( file == NULL )
		return false;

	if ( SHA1_Final(sha1, &sha1_ctx) == 0 || MD5_Final(md5, &md5_ctx) == 0 ) {
		// error !
		return false;
	}

	file->sha1.clear();
	for ( int i = 0 ; i < SHA_DIGEST_LENGTH ; ++i ) {
		file->sha1.append(QString::number(sha1[i], 16));
	}

	// FIXME: the MD5 sum is wrong:w
	file->md5.clear();
	for ( int i = 0 ; i < MD5_DIGEST_LENGTH ; ++i ) {
		file->md5.append(QString::number(md5[i], 16));
	}
	return true;
}
