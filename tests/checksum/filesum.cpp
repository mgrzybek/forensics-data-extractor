#include "filesum.h"

void	usage() {
	std::cout << "Usage: filesum file1 [file2 ... fileN]" << std::endl;
}

int	main(int argc, char* argv[]) {
	if ( argc < 2 ) {
		usage();
		return EXIT_FAILURE;
	}

	for (int index = 1 ; index < argc ; index++) {
		struct_file	file;
		file.full_path = argv[index];

		try {
			Checksum	checksum(&file);
			checksum.process_all();

			std::cout << file.sha1.toStdString() << " " << file.md5.toStdString() << " " << file.full_path.toStdString() << std::endl;
		} catch (const Exception& e) {
			qCritical() << e.calling_method << ": " << e.msg;
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

