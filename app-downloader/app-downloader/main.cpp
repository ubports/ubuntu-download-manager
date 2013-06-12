#include <QCoreApplication>
#include <download_daemon.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // TODO: deal with the fact that we might be already running
    DownloadDaemon* daemon = new DownloadDaemon();
    daemon->start();

    return a.exec();
}
