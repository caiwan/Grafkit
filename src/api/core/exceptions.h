#pragma once

#include <exception>
#include <string>


namespace FWdebug{
/////////////////////////////////////
// exception stub class

#define EXCEPT_MAX_MESSAGE 4096
	
	///@todo karakter kodolas valami kulonleges okbol kifolyolag elszarik
	class Exception : public std::exception{
		private:
			int code, line;
			//char message[EXCEPT_MAX_MESSAGE], file[EXCEPT_MAX_MESSAGE], function[EXCEPT_MAX_MESSAGE];
			//char returnText[EXCEPT_MAX_MESSAGE];

			std::string message, file, function;
			std::string returnText;

			void *object;

		public:
			Exception(int errorCode, const char* message, const char *file, const char *function, int line, void *object = NULL); //basszaj
			virtual ~Exception() {}

			virtual void getError(char *out);
			virtual void getOccurPlace(char *out);

			inline int getCode(){return this->code;}

			const char* getString();
			virtual const char* what(){return getString();}

			template <class T> inline T* getObject() { return static_cast<T*>(this->object); }
	};

/////////////////////////////////////
// custom extrption classes goes here

	class ShaderException : public Exception{
		public:
			enum SHADER_EXCEPT_TYPE_e{
				SH_INVALID = 0,
				SH_vss,
				SH_fss,
				SH_link,
			};
			
			enum SHADER_ERROR_TYPE_e{
				SHE_INVALID = 0,
				SHE_message,
				SHE_warning,
				SHE_error,
			};

			ShaderException(const char* message, const char *file, const char *function, int line, enum SHADER_EXCEPT_TYPE_e type, const char* compiler_error);
			virtual ~ShaderException();

			inline const char* getCompilerError(){return this->compiler_error.c_str();}
			inline const enum SHADER_EXCEPT_TYPE_e getType(){return this->type;}

	private:
		enum SHADER_EXCEPT_TYPE_e type;
		std::string compiler_error;
	};


/////////////////////////////////////
	///@todo az osszes exceptiont ossze kell gyujteni ide
	enum EXCEPT_code_e{
		_EXCEPT_NULL = 0,

		EXCEPT_NullPointer,
		EXCEPT_ObjTypeMismatch,
		EXCEPT_ObjVersionMismatch,
		EXCEPT_StackOverflow,
		EXCEPT_StackUnderflow,
		
		EXCEPT_AssertionFail,

		EXCEPT_NotSupported,
		EXCEPT_FileNotFound,
		
		EXCEPT_InvaludValue,
		
		EXCEPT_ArrayOutOfIndex,
		EXCEPT_OutOfMemory,
		EXCEPT_OutOfBounds,

		EXCEPT_Shader,
		EXCEPT_ShaderVSS,
		EXCEPT_ShaderFSS,
		EXCEPT_ShaderLink,

		EXCEPT_NoPackageManager,
		EXCEPT_CannotOpenPKGFile,
		EXCEPT_CannotClosePKGFile,

		EXCEPT_CannotOpenFile,
		EXCEPT_CannotCloseFile,

		EXCEPT_FWCannotCreateNULLSurface,
		EXCEPT_FWCannotCreateSurface,

		EXCEPT_RENDER_ERROR,

		_EXCEPT_COUNT,
	};

};

/////////////////////////////////////
// override construcor to get the
// current line and file
#ifndef NO_EXCEPTION_OVERRIDE
#define ExceptionEX(code, message) FWdebug::Exception(code, message, __FILE__, __FUNCTION__, __LINE__)
#define ExceptionEX2(code, message, obj) FWdebug::Exception(code, message, __FILE__, __FUNCTION__, __LINE__, obj)
#endif

#define NullPointerException()    FWdebug::Exception((int)FWdebug::EXCEPT_NullPointer, "NULL POINTER EXCEPTION", __FILE__, __FUNCTION__,__LINE__)
#define StackOverflowException()  FWdebug::Exception((int)FWdebug::EXCEPT_StackOverflow, "Stack overflow exception", __FILE__, __FUNCTION__,__LINE__)
#define StackUnderflowException() FWdebug::Exception((int)FWdebug::EXCEPT_StackOverflow, "Stack underflow exception", __FILE__, __FUNCTION__,__LINE__)

#define ObjTypeMismatchException()       FWdebug::Exception((int)FWdebug::EXCEPT_ObjTypeMismatch,    "Object type mismatch", __FILE__, __FUNCTION__,__LINE__)
#define ObjVersionMismatchException()    FWdebug::Exception((int)FWdebug::EXCEPT_ObjVersionMismatch, "Object version mismatch", __FILE__, __FUNCTION__,__LINE__)

#define AssertionFailException(x) FWdebug::Exception((int)FWdebug::EXCEPT_AssertionFail, "Assertion failure", __FILE__, __FUNCTION__,__LINE__, x)

#define NotSupportedException() FWdebug::Exception((int)FWdebug::EXCEPT_NotSupported, "Unsupported or not implemented module, function or method", __FILE__,__FUNCTION__, __LINE__)
#define FileNotFoundException() FWdebug::Exception((int)FWdebug::EXCEPT_FileNotFound, "File not found", __FILE__, __FUNCTION__,__LINE__)

#define InvalidValueException() FWdebug::Exception((int)FWdebug::EXCEPT_InvaludValue, "Invalid input value", __FILE__, __FUNCTION__, __LINE__)


#define ArrayOutOfIndexException() FWdebug::Exception((int)FWdebug::EXCEPT_ArrayOutOfIndex, "Invalid array index", __FILE__, __FUNCTION__, __LINE__)
#define OutOfMemoryException() FWdebug::Exception((int)FWdebug::EXCEPT_ArrayOutOfIndex, "Out of memory or allocation exceeds the address space", __FILE__, __FUNCTION__, __LINE__)
#define OutOfBoundsException()  FWdebug::Exception((int)FWdebug::EXCEPT_OutOfBounds, "Out of addressing bounds", __FILE__, __FUNCTION__, __LINE__)

#if 0
#define ShaderVSSException(x)	FWdebug::Exception((int)FWdebug::EXCEPT_ShaderVSS, "Vertex shader compile", __FILE__, __FUNCTION__, __LINE__, x)
#define ShaderFSSException(x)	FWdebug::Exception((int)FWdebug::EXCEPT_ShaderFSS, "Fragment shader compile", __FILE__, __FUNCTION__, __LINE__, x)
#define ShaderLinkException(x)	FWdebug::Exception((int)FWdebug::EXCEPT_ShaderLink, "Shader link", __FILE__, __FUNCTION__, __LINE__, x)
#else //0
#define ShaderVSSException(x)	FWdebug::ShaderException("Vertex shader compile", __FILE__, __FUNCTION__, __LINE__, FWdebug::ShaderException::SH_vss, x)
#define ShaderFSSException(x)	FWdebug::ShaderException("Fragment shader compile", __FILE__, __FUNCTION__, __LINE__, FWdebug::ShaderException::SH_fss, x)
#define ShaderLinkException(x)	FWdebug::ShaderException("Shader link", __FILE__, __FUNCTION__, __LINE__, FWdebug::ShaderException::SH_link, x)
#endif //0

#define NoPackageManagerException()			FWdebug::Exception((int)FWdebug::EXCEPT_NoPackageManager, "There's no package manager loaded yet.",  __FILE__, __FUNCTION__, __LINE__);
#define CannotOpenPackageFileException()	FWdebug::Exception((int)FWdebug::EXCEPT_CannotOpenPKGFile, "Error occured while opening package file.",  __FILE__, __FUNCTION__, __LINE__);
#define CannotClosePackageFileException()	FWdebug::Exception((int)FWdebug::EXCEPT_CannotClosePKGFile, "Error occured while closing package file.",  __FILE__, __FUNCTION__, __LINE__);

#define CannotOpenFileException(x)	FWdebug::Exception((int)FWdebug::EXCEPT_CannotOpenPKGFile, "Error occured while opening file.",  __FILE__, __FUNCTION__, __LINE__, x);
#define CannotCloseFileException(x)	FWdebug::Exception((int)FWdebug::EXCEPT_CannotClosePKGFile, "Error occured while closing file.",  __FILE__, __FUNCTION__, __LINE__, x);

#define CreateNULLSurfaceException()	FWdebug::Exception((int)FWdebug::EXCEPT_FWCannotCreateNULLSurface, "Creating zero sized rendering surface",  __FILE__, __FUNCTION__, __LINE__);
#define CreateSurfaceException()	FWdebug::Exception((int)FWdebug::EXCEPT_FWCannotCreateSurface, "Cannot create rendering surface",  __FILE__, __FUNCTION__, __LINE__);

#define RenderErrorException(x, file, line, function) FWdebug::Exception((int)FWdebug::EXCEPT_RENDER_ERROR, x,  file, function, line);

	// aliases

///@todo ezket a makrokat kipucolni innen, es az egesz projectbol:
#define EXCEPT_NullPointerException     NullPointerException
//#define EXCEPT_StackOverflowException   StackOverflowException
//#define EXCEPT_StackUnderflowException  StackUnderflowException
//
#define EXCEPT_NotSupportedException    NotSupportedException
//#define EXCEPT_FileNotFoundException    FileNotFoundException
//#define EXCEPT_InvalidValueException    InvalidValueException
#define EXCEPT_OutOfBoundsException     OutOfBoundsException
//
#define EXCEPT_ShaderVSSException(x)	ShaderVSSException(x)
#define EXCEPT_ShaderFSSException(x)	ShaderFSSException(x)
#define EXCEPT_ShaderLinkException(x)	ShaderLinkException(x)
//
#define EXCEPT_ArrayOutOfIndexException ArrayOutOfIndexException
//
//#define EXCEPT_NoPackageManagerException NoPackageManagerException
//
#define EXCEPT_CreateNULLSurfaceException CreateNULLSurfaceException
#define EXCEPT_CreateSurfaceException CreateSurfaceException