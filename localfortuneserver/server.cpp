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

#include <stdlib.h>

#include "server.h"
#include <qlocalserver.h>
#include <qlocalsocket.h>

Server::Server(QWidget *parent)
#ifdef Q_WS_MAEMO_5
    : QWidget(parent), clients_(10)
#else
    : QDialog(parent), clients_(10)
#endif
{
  addr_ = "fortune";
  msg_ = new QLineEdit(tr("send msg"));
  send_msg_ = new QPushButton(tr("send msg"));
  connect(send_msg_, SIGNAL(clicked()), this, SLOT(slot_send_msg()));


    statusLabel = new QLabel;
    statusLabel->setWordWrap(true);
    quitButton = new QPushButton(tr("Quit"));
    quitButton->setAutoDefault(false);


    server = new QLocalServer(this);
    if (!server->listen("fortune")) {
        QMessageBox::critical(this, tr("Fortune Server"),
                              tr("Unable to start the server: %1.")
                              .arg(server->errorString()));
        close();
        return;
    }

    statusLabel->setText(tr("The server is running.\n"
                            "Run the Fortune Client example now."));

    fortunes << tr("You've been leading a dog's life. Stay off the furniture.")
             << tr("You've got to think about tomorrow.")
             << tr("You will be surprised by a loud noise.")
             << tr("You will feel hungry again in another hour.")
             << tr("You might have mail.")
             << tr("You cannot kill time without injuring eternity.")
             << tr("Computers are not intelligent. They only think they are.");

    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(server, SIGNAL(newConnection()), this, SLOT(sendFortune()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(msg_);
    buttonLayout->addWidget(send_msg_);
    buttonLayout->addWidget(quitButton);
    buttonLayout->addStretch(1);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(statusLabel);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("Fortune Server"));
}

void Server::slot_client_disconnect()
{
  qDebug() << "slot_client_disconnect";
  clients_[0]->deleteLater();
}

void Server::sendFortune()
{
  qDebug() << "new connect";
  QLocalSocket *clientConnection = server->nextPendingConnection();
  if (clientConnection == 0)
  {
    qDebug() << "no pending connection";
    return;
  }
  else
  {
    //clients_.push_back(clientConnection);
    clients_[0]=clientConnection;
    connect(clientConnection, SIGNAL(readyRead()), this, SLOT(slot_get_client_msg()));
    connect(clientConnection, SIGNAL(disconnected()), this, SLOT(slot_client_disconnect()));
  }

#if 0
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << fortunes.at(qrand() % fortunes.size());
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    QLocalSocket *clientConnection = server->nextPendingConnection();

    clientConnection->write(block);
    clientConnection->flush();

    while (clientConnection->bytesAvailable() < (int)sizeof(quint32))
    {
      qDebug() << "wait";
      clientConnection->waitForReadyRead();
    }



    QDataStream in(clientConnection);
    in.setVersion(QDataStream::Qt_4_0);
    if (clientConnection->bytesAvailable() < (int)sizeof(quint16)) {
        return;
    }
    QString message;
    in >> message;
    qDebug() << "msg:" << message;
    statusLabel->setText(message);
    qDebug() << "clientConnection->disconnectFromServer();";
    clientConnection->disconnectFromServer();


#if 0
// read data
  QDataStream in(clientConnection);
  in.setVersion(QDataStream::Qt_4_0);
  quint16 blcok_size=0;
  QString read_data;
  if (clientConnection->bytesAvailable() >= (int)sizeof(quint16))
  {
    in >> blcok_size;
    if (!in.atEnd())
      in >> read_data;
    qDebug() << "read data: " << read_data;
  }
#endif


    //QByteArray read_data = clientConnection->readAll();
    //statusLabel->setText(read_data);

    //qDebug() << "xxx: " << read_data.count();
#endif
}

void Server::slot_send_msg()
{
  QLocalSocket *clientConnection = clients_[0];
  {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    #if 0
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << fortunes.at(qrand() % fortunes.size());
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    #endif
    out << msg_->text();
    //out.device()->seek(0);


    clientConnection->write(block);
    clientConnection->flush();
  }
#if 0
    while (clientConnection->bytesAvailable() < (int)sizeof(quint32))
    {
      qDebug() << "wait";
      clientConnection->waitForReadyRead();
    }
    QDataStream in(clientConnection);
    in.setVersion(QDataStream::Qt_4_0);
    if (clientConnection->bytesAvailable() < (int)sizeof(quint16)) {
        return;
    }
    QString message;
    in >> message;
    qDebug() << "msg:" << message;
    statusLabel->setText(message);
    #endif
}

void Server::slot_get_client_msg()
{
  QLocalSocket *clientConnection = clients_[0];

  QDataStream in(clientConnection);
  in.setVersion(QDataStream::Qt_4_0);
  if (clientConnection->bytesAvailable() < (int)sizeof(quint16)) 
  {
    return;
  }
  QString message;
  in >> message;
  qDebug() << "msg:" << message;
  statusLabel->setText(message);
}
