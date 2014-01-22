#-----------------------------------------------------------------------------
# Common configuration for all projects.
#-----------------------------------------------------------------------------

QT             -= gui
CONFIG         += link_pkgconfig

TOP_SRC_DIR     = $$PWD
TOP_BUILD_DIR   = $${TOP_SRC_DIR}/$${BUILD_DIR}
QMAKE_LIBDIR   += $${TOP_BUILD_DIR}
INCLUDEPATH    += $${TOP_SRC_DIR}
QMAKE_CXXFLAGS +=  -std=c++11 -Werror

#-----------------------------------------------------------------------------
# setup the installation prefix
#-----------------------------------------------------------------------------
INSTALL_PREFIX = /usr  # default installation prefix

# default prefix can be overriden by defining PREFIX when running qmake
isEmpty( PREFIX ) {
    message("====")
    message("==== NOTE: To override the installation path run: `qmake PREFIX=/custom/path'")
    message("==== (current installation path is `$${INSTALL_PREFIX}')")
} else {
    INSTALL_PREFIX = $${PREFIX}
    message("====")
    message("==== install prefix set to `$${INSTALL_PREFIX}'")
}

ARCH = $$system(uname -m)
contains( ARCH, x86_64 ) {
    INSTALL_LIBDIR = $${INSTALL_PREFIX}/lib/x86_64-linux-gnu
} else {
    contains( ARCH, i686) {
        INSTALL_LIBDIR = $${INSTALL_PREFIX}/lib
    } else {
        INSTALL_LIBDIR = $${INSTALL_PREFIX}/lib/arm-linux-gnueabihf
    }
}

# default library directory can be overriden by defining LIBDIR when
# running qmake
isEmpty( LIBDIR ) {
    message("====")
    message("==== NOTE: To override the library installation path run: `qmake LIBDIR=/custom/path'")
    message("==== (current installation path is `$${INSTALL_LIBDIR}')")
} else {
    INSTALL_LIBDIR = $${LIBDIR}
    message("====")
    message("==== library install path set to `$${INSTALL_LIBDIR}'")
}

include( coverage.pri )

# End of File

