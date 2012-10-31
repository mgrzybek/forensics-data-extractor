/**
 * Project: forensics-data-extractor
 * File name: configuration.cpp
 * Description: describes the configuration dialog
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

#include "configuration.h"
#include "ui_configuration.h"

Configuration::Configuration(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Configuration)
{
	QSettings	settings;

	ui->setupUi(this);

	// Dealing with pre-defined values
	settings.beginGroup("known_files_databases");
	settings.beginGroup("nsrl");
	ui->check_nsrl->setChecked(settings.value("check_state").toBool());
	qDebug() << settings.value("check_state").toBool();
	ui->nsrl_line_database->setText(settings.value("database").toString());
	qDebug() << settings.value("database").toString();
	ui->nsrl_line_driver->setText(settings.value("driver").toString());
	qDebug() << settings.value("driver").toString();
	ui->nsrl_line_hostname->setText(settings.value("hostname").toString());
	qDebug() << settings.value("hostname").toString();
	ui->nsrl_line_password->setText(settings.value("password").toString());
	qDebug() << settings.value("password").toString();
	ui->nsrl_line_port->setText(settings.value("port").toString());
	qDebug() << settings.value("port").toString();
	ui->nsrl_line_username->setText(settings.value("username").toString());
	qDebug() << settings.value("username").toString();
	settings.endGroup();
	settings.endGroup();

	if ( ui->check_nsrl->isChecked() == true )
		enable_nsrl_lines(true);
	else
		enable_nsrl_lines(false);
}

Configuration::~Configuration()
{
	delete ui;
}

void Configuration::on_tool_string_daemon_path_clicked()
{
	ui->line_strig_daemon_path->setText(QFileDialog::getOpenFileName(this, tr("Open File"), "", "Strigi Daemon"));
}

void Configuration::on_check_nsrl_stateChanged(int arg1)
{
	if ( arg1 == Qt::Unchecked ) {
		enable_nsrl_lines(false);
		return;
	}
	if ( arg1 == Qt::Checked ) {
		enable_nsrl_lines(true);
		return;
	}
}

void	Configuration::enable_nsrl_lines(bool enabled)
{
	ui->nsrl_line_database->setEnabled(enabled);
	ui->nsrl_line_hostname->setEnabled(enabled);
	ui->nsrl_line_password->setEnabled(enabled);
	ui->nsrl_line_port->setEnabled(enabled);
	ui->nsrl_line_username->setEnabled(enabled);
	ui->nsrl_line_driver->setEnabled(enabled);
}

void Configuration::on_buttonBox_accepted()
{
	QSettings	settings;

	settings.beginGroup("known_files_databases");
	settings.beginGroup("nsrl");

	settings.setValue("check_state", ui->check_nsrl->isChecked());
	settings.setValue("database", ui->nsrl_line_database->text());
	settings.setValue("hostname", ui->nsrl_line_hostname->text());
	settings.setValue("password", ui->nsrl_line_password->text());
	settings.setValue("port", ui->nsrl_line_port->text());
	settings.setValue("username", ui->nsrl_line_username->text());
	settings.setValue("driver", ui->nsrl_line_driver->text());

	settings.endGroup();
	settings.endGroup();
}
