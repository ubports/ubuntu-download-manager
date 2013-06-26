#include "fake.h"


/*
 * METHOD PARAMS
 */

typedef QPair<QList<QObject*>, QList<QObject*> > MethodParamsPair;

MethodParams::MethodParams() : MethodParamsPair()
{
}

MethodParams::MethodParams(QList<QObject*> inParams, QList<QObject*> outParams) :
    MethodParamsPair(inParams, outParams)
{
}

QList<QObject*> MethodParams::inParams()
{
    return first;
}

void MethodParams::setInParams(QList<QObject*> params)
{
    first = params;
}

QList<QObject*> MethodParams::outParams()
{
    return second;
}

void MethodParams::setOutParams(QList<QObject*> params)
{
    second = params;
}

/*
 * METHOD DATA
 */

typedef QPair<QString, MethodParams> MethodDataPair;

MethodData::MethodData()
    : MethodDataPair()
{
}

MethodData::MethodData(const QString& name, const MethodParams& params)
    : MethodDataPair(name, params)
{
}

QString MethodData::methodName()
{
    return first;
}

void MethodData::setMethodName(const QString& name)
{
    first = name;
}

MethodParams MethodData::params()
{
    return second;
}

void MethodData::setParams(const MethodParams& params)
{
    second = params;
}

/*
 * FAKE
 */

Fake::Fake() :
    _recording(false)
{
}


void Fake::record()
{
    _recording = true;
}

void Fake::stopRecording()
{
    _recording = false;
}

void Fake::clear()
{
    _called.clear();
}

QList<MethodData> Fake::calledMethods()
{
    return _called;
}
