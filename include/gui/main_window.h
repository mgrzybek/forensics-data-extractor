/**
 * Project: forensics-data-extractor
 * File name: main_window.h
 * Description: describes the main Qt window
 *
 * @author Mathieu Grzybek on 2012-05-20
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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QDir>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QErrorMessage>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>

#include "common.h"

#include "gui/configuration.h"

#include "analysis/database.h"
#include "analysis/indexing_engine.h"
#include "analysis/parsing_engine.h"
#include "analysis/receiver.h"

#include "extractors/worker.h"

#include "databases/generic_database.h"
#include "databases/nsrl.h"

namespace Ui {
	class Main_Window;
}

class Main_Window : public QMainWindow
{
	Q_OBJECT

	public:
		explicit	Main_Window(void* z_context, QWidget *parent = 0);
		~Main_Window();

		zmq::context_t*	zmq_context;

	signals:
		void	stop();

	private slots:
		void	update_info();

		void	on_action_New_Analysis_triggered();
		void	on_action_Quit_triggered();
		void	on_action_Open_Analysis_triggered();
		void	on_action_Close_Analysis_triggered();
		void	on_action_Preferences_triggered();

		void	on_index_button_clicked();

		void	on_action_Save_Analysis_triggered();

		void	on_push_add_image_clicked();
		void	on_push_add_directory_clicked();

		void	on_button_acquire_sources_clicked();

		void	on_check_analysis_auto_start_stateChanged(int arg1);

		void	on_push_start_analysis_clicked();

public slots:
		void	refresh_models();

	private:
		/*
		 * GUI Stuff
		 */
		Ui::Main_Window*	ui;
		Configuration*		conf_dialog;

		/*
		 * Models
		 */
		Database*		db;
		web_browser_models	web_models;

		// sources to process
		QSqlTableModel*		model_sources;
		// scanned files
		QSqlTableModel*		model_indexed_files;
		// extracted files
		QSqlTableModel*		model_analysed_files;
		// known files
		QSqlTableModel*		model_known_files;

		/*
		 * Working files
		 */
		QString		working_directory;
		QString		strigi_daemon_path;

		/*
		 * Multi threading
		 */
		QThreadPool	thread_pool;

		/*
		 * Processing classes
		 */
		generic_database_list	known_files_databases;
		Worker*			worker;
		Receiver*		receiver;

		QList<Generic_Database*>	g_db;

		bool	scan_in_progress;
		bool	index_in_progress;

		void	process_analysis();
		void	process_index();
		void	process_acquisition();
		void	init_models(QSqlDatabase& db);
		void	clean_models();

		bool	init_analysis_db(QSqlQuery& query);
		bool	create_analysis_db(const QString& db_file);
		bool	open_analysis_db(const QString& db_file);
		bool	save_analysis_db(QSqlQuery& query);

		void	load_settings();
		void	save_settings();
};

#endif // MAIN_WINDOW_H
