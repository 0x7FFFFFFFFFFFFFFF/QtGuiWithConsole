#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "consoleutil.h"

#include <QShortcut>
#include <QDateTime>
#include <QCoreApplication>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    QShortcut *consoleShortcut = new QShortcut(QKeySequence("Alt+h"), this);
    connect(consoleShortcut, &QShortcut::activated, this, [this]() {
        ConsoleUtils::ToggleConsole();
    });
}

MainWindow::~MainWindow()
{
    ConsoleUtils::CleanupConsole();
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    puts(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toUtf8().constData());
    puts("小寨赛格");


    // Get full path to executable (including exe name)
    QString exePath = QCoreApplication::applicationFilePath();
    // Get directory containing the executable
    QString exeDir = QCoreApplication::applicationDirPath();

    // When printing, use native separators
    QString exePathNative = QDir::toNativeSeparators(exePath);
    QString exeDirNative = QDir::toNativeSeparators(exeDir);

    puts(exePath.toUtf8().constData());
    puts(exeDir.toUtf8().constData());
    puts(exePathNative.toUtf8().constData());
    puts(exeDirNative.toUtf8().constData());


    // Get just the executable name
    QString exeName = QFileInfo(exePath).fileName();

    // Construct paths relative to the executable directory
    QString configPath2 = QDir(exeDir).filePath("config.ini");

    puts(exeName.toUtf8().constData());
    puts(configPath2.toUtf8().constData());
}












