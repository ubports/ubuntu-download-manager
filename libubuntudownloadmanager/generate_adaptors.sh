echo "Generating adaptors from DBus xml definitions:"
echo "DownloadManagerAdaptor..."
qdbusxml2cpp -c DownloadManagerAdaptor -a download_manager_adaptor.h:download_manager_adaptor.cpp -i app-downloader-lib_global.h com.canonical.applications.download_manager.xml
echo "generated"
echo "DownloadAdaptor..."
qdbusxml2cpp -c DownloadAdaptor -a download_adaptor.h:download_adaptor.cpp -i app-downloader-lib_global.h com.canonical.applications.download.xml
echo "generated"
