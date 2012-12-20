/**
 * Project: forensics-data-extractor
 * File name: configuration.h
 * Description: describes the configuration dialog
 *
 * @author Mathieu Grzybek on 2012-10-31
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

