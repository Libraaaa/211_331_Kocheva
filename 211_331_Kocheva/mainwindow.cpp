#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFileDialog>
#include <QCryptographicHash>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    loadJsonFromFile("transactions_valid.json");
    connect(ui->buttonOpenFile, &QPushButton::clicked, this, &MainWindow::on_openButton_clicked);

}

// Обработка нажатия кнопки открытия файла
void MainWindow::on_openButton_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, "Открыть JSON-файл", "", "JSON Files (*.json)");
    if (!fileName.isEmpty()) {
        loadJsonFromFile(fileName);
    }
}

// Загрузка и проверка JSON-файла
void MainWindow::loadJsonFromFile(const QString &filePath) {
    ui->listWidgetTransactions->clear();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Не удалось открыть файл:" << filePath;
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        qDebug() << "Неверный формат JSON";
        return;
    }

    QJsonArray array = doc.array();

    QString prevHash = "0";
    bool errorDetected = false;

    for (int i = 0; i < array.size(); ++i) {
        QJsonObject obj = array[i].toObject();
        QString lastName = obj["LastName"].toString();
        QString firstName = obj["FirstName"].toString();
        QString passport = obj["Passport"].toString();
        QString hash = obj["Hash"].toString();

        QString concatenated = lastName + firstName + passport + prevHash;
        QString calculatedHash = calculateHash(concatenated);

        QString itemText = QString("Фамилия: %1\nИмя: %2\nПаспорт: %3\nХэш: %4")
                               .arg(lastName)
                               .arg(firstName)
                               .arg(passport)
                               .arg(hash);

        // qDebug() << "Concat string: " << concatenated;
        // qDebug() << "Calculated: " << calculatedHash;
        // qDebug() << "From file: " << hash;

        QListWidgetItem *item = new QListWidgetItem(itemText);
        if (errorDetected || calculatedHash != hash) {
            item->setBackground(Qt::red);
            errorDetected = true;
        }

        ui->listWidgetTransactions->addItem(item);
        prevHash = hash;
    }
}

// Функция для вычисления хэша
QString MainWindow::calculateHash(const QString &data) {
    QByteArray hashBytes = QCryptographicHash::hash(data.toUtf8(), QCryptographicHash::Sha256);
    return hashBytes.toBase64();
}

MainWindow::~MainWindow()
{
    delete ui;
}
