#include "mainwindow.h"
#include "ModeDialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ModeDialog dlg;
    if(dlg.exec()!=QDialog::Accepted)
        return 0;
    MainWindow w;
    w.setMode(dlg.selectMode());
    w.show();
    return a.exec();
}
