macx {
    LIBS += -L/opt/local/lib \
	-L/usr/lib \
	-L/usr/local/lib \
	-lzmq \
	-lssl \
	-lcrypto \
	-lz \
	/opt/local/lib/libtsk3.a
	
    INCLUDEPATH += /usr/include \
	/opt/local/include
}
