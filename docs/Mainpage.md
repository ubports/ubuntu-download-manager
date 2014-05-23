Ubuntu Download Manager {#mainpage}
=======================

The application lifecycle model of found in Ubuntu Touch does not
ensure that application will be allowed to perform a long running
connection to a server in order to perform a download. The Ubuntu
Download Manager is a system daemon that leverages the long connections
required to perform downloads from the click applications and provides
a set of APIs for the application to interact with the downloads.


The download manager is a centralized daemon and therefore must ensure
that only those applications

Security Model
--------------

The download manager is a centralized daemon that can be used by all
those applications that are allows to use the networking apparmor
profile, due to this fact the downloading daemon ensures that the
interaction with a download can only be performed either by the 
application that originally created the download or by an application
that is not confined and that can therefore interact with all the 
daemons of the application.

Once a download has been created the download manager will ensure that
the data being downloaded is stored in a location that can only be
accessed by the creating application to ensure that the app confinement
is kept.
