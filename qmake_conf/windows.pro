win32 {
    # On Windows 0MQ's inproc is not supported, we use TCP connections instead
    DEFINES	+= WINDOWS_OS

    # Hack to avoid problems with QDateTime
    # FIXME: http://qt-project.org/forums/viewthread/22133
    DEFINES	+= NOMINMAX

    LIBS	+= "E:/Devel/ZeroMQ 3.2.2/lib/libzmq-v110-mt-3_2_2.lib" \
	E:/Devel/libewf-20121209/msvscpp/Release/*.lib \
	E:/Devel/sleuthkit-4.0.1/win32/Release/libtsk.lib

    QMAKE_LFLAGS += /NODEFAULTLIB:library /LTCG

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


    INCLUDEPATH	+= "E:/Devel/ZeroMQ 2.2.0/include" \
	E:/Devel/openssl-1.0.1c/inc32 \
	E:/Devel/openssl-1.0.1c/crypto \
	E:/Devel/sleuthkit-4.0.1

}
