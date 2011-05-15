/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "uploaderdialog.h"
#include "ui_uploaderdialog.h"
#include "core/core.h"

// #include <QtGui/QClipboard>
#include <QDebug>

UploaderDialog::UploaderDialog(Uploader* uploader, QWidget* parent)
    :QDialog(parent), ui(new Ui::UploaderDialog), loader(uploader)
{
    ui->setupUi(this);
    
    ui->shotLabel->setFixedWidth(150);
    ui->shotLabel->setFixedHeight(128);

    ui->shotLabel->setPixmap(Core::instance()->getPixmap().scaled(ui->shotLabel->size(),                              Qt::KeepAspectRatio, Qt::SmoothTransformation));
    
    // set suze to tooltip for preview pixmap
    int width = Core::instance()->getPixmap().width();
    int height = Core::instance()->getPixmap().height();
    QString pixmapSize = QString::number(width) + "x" + QString::number(height) + tr(" pixel");
    ui->shotLabel->setToolTip(pixmapSize);
    
    QString warningTitle = tr("Warning!");
    QString warningText =  tr("Resize makes on servers imageshack.us");
    
    ui->labResizeWarning->setText("<font color='red'><b>" + warningTitle + "</b></font><br />" + warningText);

    connect(ui->butClose, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->butUpload, SIGNAL(clicked(bool)), loader, SLOT(uploadScreen()));
    connect(ui->butUpload, SIGNAL(clicked(bool)), this, SLOT(uploadStart()));
    
    connect(loader, SIGNAL(uploadDone(QVector<QByteArray>)), this, SLOT(uploadDone(QVector<QByteArray>)));
    connect(loader, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(updateProgerssbar(qint64,qint64)));
    
    connect(ui->butCopyLink, SIGNAL(clicked(bool)), this, SLOT(copyDirectLink()));
    connect(ui->butCopyExtCode, SIGNAL(clicked(bool)), this, SLOT(copyExtCode()));
    connect(ui->cbxExtCode, SIGNAL(currentIndexChanged(int)), this, SLOT(changeExtCode(int)));
    connect(ui->cbxResize, SIGNAL(currentIndexChanged(int)), loader, SLOT(selectResizeMode(int)));
    
    qDebug() << "Core::instance()->getPixmap().width() " << Core::instance()->getPixmap().width();
    
    ui->progressBar->setFormat(tr("Uploaded ") + "%p%" + " (" + "%v" + " of " + "%m bytes");
    ui->progressBar->setVisible(false);
    ui->labStatus->setVisible(false);
    ui->stackedWidget->setCurrentIndex(0);
}

UploaderDialog::~UploaderDialog()
{

}

void UploaderDialog::closeEvent(QCloseEvent* e)
{
    qDebug() << "loader ;" <<  loader;
    delete loader;

    QDialog::closeEvent(e);
}

void UploaderDialog::updateProgerssbar(qint64 bytesSent, qint64 bytesTotal)
{
    ui->progressBar->setMaximum(bytesTotal);
    ui->progressBar->setValue(bytesSent);
    
    if (bytesSent == bytesTotal)
    {
        qDebug() << "all is send!!!!";
        ui->progressBar->setFormat(tr("Upload completed!"));
        ui->labStatus->setText(tr("Receiving a response from the server"));
    }
}

void UploaderDialog::uploadStart()
{
    ui->butClose->setEnabled(false);
    ui->butUpload->setEnabled(false);
    ui->cbxResize->setEnabled(false);
    ui->progressBar->setVisible(true);
    ui->labStatus->setVisible(true);
    ui->labStatus->setText(tr("Sending screenshot on the server"));
}

void UploaderDialog::uploadDone(const QVector< QByteArray >& resultStrings)
{
    ui->labSuccessfully->setText(tr("Screenshot uploaded successfully!"));
    ui->butClose->setEnabled(true);
    ui->butUpload->setVisible(false);
    
    ui->editDirectLink->setText(resultStrings.at(0));
    
    extCodes.resize(resultStrings.count() - 1);
    for (int i = 1; i < resultStrings.count(); i++)
    {
        extCodes[i-1] = resultStrings[i];        
    }
        
    ui->editExtCode->setText(extCodes[ui->cbxExtCode->currentIndex()]);

    ui->stackedWidget->setCurrentIndex(1);
}

inline void UploaderDialog::copyLink(const QString& link)
{
    qApp->clipboard()->setText(link);
}

void UploaderDialog::copyDirectLink()
{
    copyLink(ui->editDirectLink->text());
}

void UploaderDialog::copyExtCode()
{
    copyLink(ui->editExtCode->text());
}


void UploaderDialog::changeExtCode(int code)
{
    ui->editExtCode->setText(extCodes[code]);
}