#ifndef DOWNLOAD_ERROR_H
#define DOWNLOAD_ERROR_H

#include <QObject>
#include <QString>

namespace Ubuntu {

namespace DownloadManager {

class DownloadError : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString type READ type NOTIFY typeChanged)
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)

public:
    explicit DownloadError(QObject *parent = 0);

    QString type() const { return m_type; }
    QString message() const { return m_message; }

    void setType(QString type) { m_type = type; emit typeChanged(); }
    void setMessage(QString message) { m_message = message; emit messageChanged(); }

signals:
    void typeChanged();
    void messageChanged();

private:
    QString m_message;
    QString m_type;

};

}
}

#endif // DOWNLOAD_ERROR_H
