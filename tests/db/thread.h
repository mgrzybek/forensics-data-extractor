#include <QThread>
#include "../../include/database.h"

class Thread : public QThread {
	public:

		Thread(Database* d);
		~Thread();

		void	run();
	private:
		Database*	db;
};

