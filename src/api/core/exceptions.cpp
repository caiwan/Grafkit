#include "exceptions.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <windows.h>

#include "../utilites/platform/stringutils.h"

using namespace std;

FWdebug::Exception::Exception(int errorCode, const char *_message, const char *_file, const char *_function, int sor, void* _object) : std::exception(){
	this->code = errorCode;
	this->line = sor;

	this->object = _object;

	this->message = _message; 
	this->file =_file;
	this->function = _function;

	//this->returnText[0] = 0;
	//sprintf(returnText, "%#x: %s \nin function %s\nin file %s, at line %d", this->code, this->message, this->function, this->file, this->line);
	this->returnText = string_format("%#x: %s \nin function %s\nin file %s, at line %d", this->code, this->message.c_str(), this->function.c_str(), this->file.c_str(), this->line);
}

void FWdebug::Exception::getError(char *out){
	sprintf(out, "%#x: %s", this->code, this->message.c_str());
}

void FWdebug::Exception::getOccurPlace(char *out){
	sprintf(out, "in function %s in file %s at line %d", this->function.c_str(), this->file.c_str(), this->line);
}

const char *FWdebug::Exception::getString(){
	return this->returnText.c_str();
}

/////////////////////////////////////
using FWdebug::ShaderException;

FWdebug::ShaderException::ShaderException(const char* message, const char *file, const char *function, int line, enum SHADER_EXCEPT_TYPE_e _type, const char* _compiler_error) :
FWdebug::Exception(EXCEPT_Shader, message, file, function, line)
{
	this->compiler_error = _compiler_error;
	this->type = _type;
}

FWdebug::ShaderException::~ShaderException(){
	// ... 
}
