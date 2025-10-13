#include "sFaultDetectionClassificationRGA.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    sFaultDetectionClassificationRGA w;
    w.showMaximized();
    return a.exec();
}
