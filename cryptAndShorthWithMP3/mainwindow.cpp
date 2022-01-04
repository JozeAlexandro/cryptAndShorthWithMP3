#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) noexcept
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow() noexcept
{
    delete ui;
}

void MainWindow::printToStatusBar(const QString &message) noexcept
{
    static const quint32 TIMEOUT = 2000;

    ui->statusbar->showMessage(message, TIMEOUT);
}

void MainWindow::updateUI() noexcept
{
    /// Обновление доступности кнопки "Старт"
    ui->btnStart->setEnabled(fileName.isEmpty() ? false : true);

    /// Обновление области сообщения
    ui->textWindow->clear();
    ui->textWindow->setEnabled(currentModeWork == WRITE ? true : false);
}

void MainWindow::readMessage()
{
    ID3v2TagWorker worker(fileName.toStdString());

    std::string buffer = worker.readMessage();

    cryptor myCryptor;

    buffer = myCryptor.DeCode(buffer,
                              ui->leCryptoKey->text().toStdString(),
                              roundsCount);

    message.clear();
    message = QString::fromStdString(buffer);

    ui->textWindow->setPlainText(message);
    printToStatusBar("Сообщение прочитано");
}

void MainWindow::writeMessage()
{
    ID3v2TagWorker worker(fileName.toStdString());

    message = ui->textWindow->toPlainText();

    cryptor myCryptor;

    worker.writeMessage(myCryptor.Code(message.toStdString(),
                                       ui->leCryptoKey->text().toStdString(),
                                       roundsCount));

    printToStatusBar("Сообщение записано");
}

void MainWindow::on_cbProgrammMode_currentIndexChanged(int index)
{
    currentModeWork = static_cast<eModeWork>(index);
    updateUI();

    QString messageToStatusBar("Изменен режим работы программы на ");

    static const QString S_WRITE_MODE("запись сообщения");
    static const QString S_READ_MODE ("чтение сообщения");

    messageToStatusBar.append(currentModeWork == WRITE ? S_WRITE_MODE
                                                       : S_READ_MODE);
    printToStatusBar(messageToStatusBar);
}

void MainWindow::on_btnChoseFile_clicked()
{
    fileName = QFileDialog::getOpenFileName(this, "Выбор файла формата MP3",
                                            "/home", "*.mp3");

    ui->leFilePath->setText(fileName);
    updateUI();
    printToStatusBar("Выбран файл: " + fileName);
}

void MainWindow::on_btnStart_clicked()
{
    currentModeWork == WRITE ? writeMessage()
                             : readMessage();
}
