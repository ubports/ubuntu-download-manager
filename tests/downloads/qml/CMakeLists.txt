set(QML_TESTS
)

foreach(test ${DAEMON_TESTS})
        # set the sources per test
        set(${test}_QML_SOURCES
                ${test}.cpp
        )

        set(${test}_QML_HEADERS
                ${test}.h
        )

endforeach(test)
