#include "checksum.h"

Checksum::Checksum(struct_file* f)
{
	if ( f == NULL )
		throw "Input is NULL!";

	file = f;
}

bool	Checksum::process_all() {
	SHA_CTX	sha1_ctx;
	MD5_CTX	md5_ctx;
	uchar	data[1024];
	FILE*	opened_file = fopen(file->full_path.toAscii().constData(), "rb");
	int		bytes;

	if ( SHA1_Init(&sha1_ctx) == 0 or MD5_Init(&md5_ctx) == 0 ) {
		qCritical() << "Cannot init hash engines";
		return false;
	}

	if ( opened_file == NULL ) {
		qCritical() << "Cannot open " << file->full_path << " for sha1";
		return false;
	}

	try {
		 while ( ( bytes = fread(data, 1, 1024, opened_file) ) != 0 ) {
			if ( SHA1_Update(&sha1_ctx, data, bytes) == 0 or MD5_Update(&md5_ctx, data, bytes) == 0 ) {
				qCritical() << "Cannot compute hash for " << file->full_path;
				return false;
			}
		}
	} catch (const std::exception& e) {
	}

	if ( SHA1_Final(file->sha1, &sha1_ctx) == 0 or MD5_Final(file->md5, &md5_ctx) == 0 ) {
		// error !
		return false;
	}

	qDebug() << "sha1: " << file->sha1;
	return true;
}
