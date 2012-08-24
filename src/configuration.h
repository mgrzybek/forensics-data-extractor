#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QFileDialog>
#include <QDialog>

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

	private:
		Ui::Configuration *ui;
};

#endif // CONFIGURATION_H

