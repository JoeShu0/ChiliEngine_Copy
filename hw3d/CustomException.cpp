#include "CustomException.h"
#include<sstream>

CustomException::CustomException(int line, const char * file) noexcept
	:
	line(line),
	file(file)
{}

const char * CustomException::what() const noexcept
{
	std::ostringstream oss;  //here the oss is used to format the string to be clean.
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();  //Since the oss.str() will be deleted after this call, and the return ptr, we have to have a buffer to store the string.
	return whatBuffer.c_str();
}

const char * CustomException::GetType() const noexcept
{
	return "Custom Exception";
}

int CustomException::GetLine() const noexcept
{
	return line;
}

const std::string & CustomException::GetFile() const noexcept
{
	return file;
}

std::string CustomException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File]" << file << std::endl
		<< "[line]" << line;
	return oss.str();
}



