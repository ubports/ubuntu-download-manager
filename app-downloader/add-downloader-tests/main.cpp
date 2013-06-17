#include <QCoreApplication>
#include "test_runner.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    return RUN_ALL_QTESTS(argc, argv);
}
