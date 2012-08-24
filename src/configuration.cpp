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

#include "src/configuration.h"
#include "ui_configuration.h"

Configuration::Configuration(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Configuration)
{
	ui->setupUi(this);
}

Configuration::~Configuration()
{
	delete ui;
}

void Configuration::on_tool_string_daemon_path_clicked()
{
	ui->line_strig_daemon_path->setText(QFileDialog::getOpenFileName(this, tr("Open File"), "", "Strigi Daemon"));
}

