echo "Generating adaptors from DBus xml definitions:"
echo "DownloadManagerAdaptor..."
qdbusxml2cpp -c DownloadManagerAdaptor -a download_manager_adaptor.h:download_manager_adaptor.cpp com.canonical.applications.downloader.xml
echo "generated"
echo "DownloadAdaptor..."
qdbusxml2cpp -c DownloadAdaptor -a download_adaptor.h:download_adaptor.cpp com.canonical.applications.application_download.xml
echo "generated"
