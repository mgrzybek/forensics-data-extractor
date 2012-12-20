macx {
	PKGCONFIG += thrift
	
	LIBS += -lthrift \
		-L/opt/local/lib \
		-L/usr/lib \
		-L/usr/local/lib \
		#-lssl \
		#-lpthread \
		#-lcrypto \
		#-lm \
		#-ldl \
		#-lz \
	
	INCLUDEPATH += /usr/include \
		/opt/local/include \
		/opt/local/include/thrift
}

