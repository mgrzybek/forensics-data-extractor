#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QFileDialog>
#include <QDialog>

#include "common.h"
#include "databases/generic_database.h"

namespace Ui {
	class Configuration;
}

class Configuration : public QDialog
{
	Q_OBJECT

	public:
		explicit Configuration(QWidget *parent = 0);
		~Configuration();

	private slots:
		void on_tool_string_daemon_path_clicked();

		void on_check_nsrl_stateChanged(int arg1);

		void on_buttonBox_accepted();

	private:
		Ui::Configuration *ui;
		generic_database_list*	generic_databases;

		void	enable_nsrl_lines(bool enabled);
};

#endif // CONFIGURATION_H

