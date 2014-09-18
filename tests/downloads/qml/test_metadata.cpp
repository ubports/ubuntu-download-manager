/*
 * Copyright 2014 Canonical Ltd.
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

#include <QScopedPointer>
#include <metadata.h>
#include "test_metadata.h"

using namespace Ubuntu::DownloadManager;

TestMetadata::TestMetadata(QObject *parent)
    : BaseTestCase("TestMetadata", parent) {
}

void
TestMetadata::testGetTittleEmpty() {
    QScopedPointer<Metadata> metadata(new Metadata());
    QVERIFY(metadata->title().isEmpty());
}

void
TestMetadata::testGetTittlePresent_data() {
    QTest::addColumn<QString>("title");

    QTest::newRow("First") << "My title";
    QTest::newRow("Second") << "Test";
}

void
TestMetadata::testGetTittlePresent() {
    QFETCH(QString, title);
    QScopedPointer<Metadata> metadata(new Metadata());
    metadata->setTitle(title);
    QCOMPARE(metadata->title(), title);
}

void
TestMetadata::testGetShowInIndicatorEmpty() {
    QScopedPointer<Metadata> metadata(new Metadata());
    QVERIFY(metadata->showInIndicator());
}

void
TestMetadata::testGetShowInIndicatorPresent_data() {
    QTest::addColumn<bool>("show");

    QTest::newRow("Enabled") << true;
    QTest::newRow("Disabled") << false;
}

void
TestMetadata::testGetShowInIndicatorPresent() {
    QFETCH(bool, show);
    QScopedPointer<Metadata> metadata(new Metadata());
    metadata->setShowInIndicator(show);
    QCOMPARE(metadata->showInIndicator(), show);
}

QTEST_MAIN(TestMetadata)
#include "moc_test_metadata.cpp"
