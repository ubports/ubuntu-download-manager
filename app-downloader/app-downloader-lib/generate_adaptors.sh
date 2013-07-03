echo "Generating adaptors from DBus xml definitions:"
echo "DownloadManagerAdaptor..."
qdbusxml2cpp -c DownloaderAdaptor -a downloader_adaptor.h:downloader_adaptor.cpp com.canonical.applications.downloader.xml
echo "generated"
echo "DownloadAdaptor..."
qdbusxml2cpp -c ApplicationDownloadAdaptor -a application_download_adaptor.h:application_download_adaptor.cpp com.canonical.applications.application_download.xml
echo "generated"
