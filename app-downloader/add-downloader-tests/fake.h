#ifndef FAKE_H
#define FAKE_H

#include <QList>
#include <QObject>
#include <QPair>


class MethodParams : public QPair<QList<QObject*>, QList<QObject*> >
{

public:
    explicit MethodParams();
    explicit MethodParams(QList<QObject*> inParams, QList<QObject*> outParams);

    QList<QObject*> inParams();
    void setInParams(QList<QObject*> params);

    QList<QObject*> outParams();
    void setOutParams(QList<QObject*> params);

};

class MethodData : public QPair<QString, MethodParams>
{

public:
    explicit MethodData();
    explicit MethodData(const QString& name, const MethodParams& params);

    QString methodName();
    void setMethodName(const QString& name);

    MethodParams params();
    void setParams(const MethodParams& params);
};

class Fake
{
public:
    explicit Fake();
    
    void record();
    void stopRecording();
    void clear();
    QList<MethodData> calledMethods();

protected:
    bool _recording;
    QList<MethodData> _called;
};

#endif // FAKE_H
