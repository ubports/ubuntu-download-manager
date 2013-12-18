/*
 * Copyright 2013 2013 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 3 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "./fake_process_factory.h"
#include "./fake_process.h"

FakeProcessFactory::FakeProcessFactory(QObject *parent)
    : ProcessFactory(parent),
      Fake() {
}

Process*
FakeProcessFactory::createProcess() {
    FakeProcess* process = new FakeProcess(this);

    if (_recording) {
        QList<QObject*> inParams;

        QList<QObject*> outParams;
        outParams.append(process);
        MethodParams params(inParams, outParams);

        MethodData methodData("createProcess", params);
        _called.append(methodData);

        // if we are recording we do set the recording of the returned process
        process->record();
    }
    return process;
}
