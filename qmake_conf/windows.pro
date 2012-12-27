win32 {
    # On Windows 0MQ's inproc is not supported, we use TCP connections instead
    DEFINES	+= WINDOWS_OS

    # Hack to avoid problems with QDateTime
    # FIXME: http://qt-project.org/forums/viewthread/22133
    DEFINES	+= NOMINMAX

    LIBS	+= -LE:/Users/mathieu.grzybek/Documents/GitHub/libzmq/bin/x64/ -llibzmq \
	E:/Devel/sleuthkit-4.0.1/win32/Release/libtsk.lib \
	E:/Devel/libewf-20121209/msvscpp/Release/*.lib

#libbfio.lib
#libcstring.lib
#liberror.lib
#libewf.lib
#libfvalue.lib
#libmfcache.lib
#libmfdata.lib
#libnotify.lib
#libuna.lib
#zlib.lib


    INCLUDEPATH	+= E:/Users/mathieu.grzybek/Documents/GitHub/libzmq/include \
	E:/Devel/openssl-1.0.1c/include \
	E:/Devel/openssl-1.0.1c/crypto \
	E:/Devel/sleuthkit-4.0.1

}
