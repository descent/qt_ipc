/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include <QtNetwork>

#include "client.h"

Client::Client(QWidget *parent)
#ifdef Q_WS_MAEMO_5
    : QWidget(parent)
#else
    : QDialog(parent)
#endif
{
    hostLabel = new QLabel(tr("&Server name:"));
    hostLineEdit = new QLineEdit("fortune");

    hostLabel->setBuddy(hostLineEdit);

    statusLabel = new QLabel(tr("This examples requires that you run the "
                                "Fortune Server example as well."));
    statusLabel->setWordWrap(true);

    getFortuneButton = new QPushButton(tr("Get Fortune"));
    getFortuneButton->setDefault(true);

    quitButton = new QPushButton(tr("Quit"));

    send_button_ = new QPushButton(tr("Send message to server")); 
    send_msg_ = new QLineEdit("msg to server");

    buttonBox = new QDialogButtonBox;
    buttonBox->addButton(getFortuneButton, QDialogButtonBox::ActionRole);
    //buttonBox->addButton(send_button_, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    socket = new QLocalSocket(this);

    connect(hostLineEdit, SIGNAL(textChanged(QString)), this, SLOT(enableGetFortuneButton()));
    connect(getFortuneButton, SIGNAL(clicked()), this, SLOT(requestNewFortune()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

    connect(socket, SIGNAL(readyRead()), this, SLOT(readFortune()));
    connect(socket, SIGNAL(error(QLocalSocket::LocalSocketError)), this, SLOT(displayError(QLocalSocket::LocalSocketError)));
    connect(socket, SIGNAL(connected()), this, SLOT(slot_socket_connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(slot_socket_disconnected()));

    connect(send_button_, SIGNAL(clicked()), this, SLOT(slot_send_msg()));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(hostLabel, 0, 0);
    mainLayout->addWidget(hostLineEdit, 0, 1);
    mainLayout->addWidget(statusLabel, 2, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 5, 0, 1, 2);

    mainLayout->addWidget(send_msg_, 4, 0);
    mainLayout->addWidget(send_button_, 4, 1);
    setLayout(mainLayout);

    setWindowTitle(tr("Fortune Client"));
    hostLineEdit->setFocus();
}

void Client::slot_socket_disconnected()
{
  qDebug() << "disconnect";
}

void Client::slot_socket_connected()
{
  qDebug() << "connect to server";
  //slot_send_msg();
}

void Client::slot_send_msg()
{
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_0);
  qDebug() << "send msg: " << send_msg_->text();
  out << send_msg_->text();
  out.device()->seek(0);
  socket->write(block);
  socket->flush();
}

void Client::requestNewFortune()
{
    getFortuneButton->setEnabled(false);
    blockSize = 0;
    socket->abort();
    socket->connectToServer(hostLineEdit->text());
}

void Client::readFortune()
{
  QDataStream in(socket);
  in.setVersion(QDataStream::Qt_4_0);

  quint16 avail_byte = socket->bytesAvailable();
  qDebug() << "avail_byte: " << avail_byte;

  QString read_data;
  in >> read_data;



  qDebug() << "read_data: " << read_data;
  avail_byte = socket->bytesAvailable();
  qDebug() << "avail_byte: " << avail_byte;
  //qDebug() << "currentFortune: " << currentFortune;
  getFortuneButton->setEnabled(true);
}

void Client::displayError(QLocalSocket::LocalSocketError socketError)
{
    switch (socketError) {
    case QLocalSocket::ServerNotFoundError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QLocalSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    case QLocalSocket::PeerClosedError:
        break;
    default:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(socket->errorString()));
    }

    getFortuneButton->setEnabled(true);
}

void Client::enableGetFortuneButton()
{
    getFortuneButton->setEnabled(!hostLineEdit->text().isEmpty());
}
