#ifndef EXCEPTION_H
#define EXCEPTION_H

class Exception: public std::exception
{
	public:
		//Exception(QString&	method, QString& message);
		Exception();
		~Exception();

		void	print();

		QString	calling_method;
		QString	msg;
};

#endif // EXCEPTION_H
