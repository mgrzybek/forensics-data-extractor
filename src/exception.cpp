#include "include/exception.h"

Exception::Exception(QString&	method, QString& message) {
	calling_method = method;
	msg = message;
}

Exception::~Exception() {
}

void	Exception::print() {
	qCritical() << calling_method << ": " << msg;
}
