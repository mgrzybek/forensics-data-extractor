#ifndef CHECKSUM_H
#define CHECKSUM_H

#include <QDebug>
#include <QString>
#include <QByteArray>

#include <stdio.h>

#include "common.h"

class Checksum
{
	public:
		Checksum(struct_file* f);

		bool	process_all();

	private:
		struct_file*	file;
};

#endif // CHECKSUM_H
