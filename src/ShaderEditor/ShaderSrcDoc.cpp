// #include "../stdafx.h"
#include "stdafx.h"

// #include "utilites/platform/stringutils.h"

#include "ShaderSrcDoc.h"

#include "render/shader.h"
#include "exceptions.h"

using FWdebug::Exception;

using namespace FWdebugExceptions;

// using FWdebug::ShaderException;
// using FWrender::Shader;

//////////////////////////////////////////////////////////////////////
// Shader dokumentum
//////////////////////////////////////////////////////////////////////
/*
CShaderEditorDoc::CShaderEditorDoc(){
	m_shader = NULL;
	m_is_FSS_Only = 0;
	m_is_has_errors = 0;
}
*/

CShaderEditorDoc::CShaderEditorDoc(const char* _shader_source){
	//
	//if (!_vss || !_fss) throw new NullPointerException();
	//m_vss=_vss;
	//m_fss=_fss;
	//m_is_FSS_Only=_is_FSS_Only;
	//m_is_has_errors=0;
	//m_shader = NULL;
	//m_is_has_errors = 0;

}

CShaderEditorDoc::~CShaderEditorDoc(){
	//delete m_shader;
}

int CShaderEditorDoc::compileShader(){
// shader objektum cserevel dolgozik
	//FWrender::Shader *newshader = NULL;
	//try{
	//	this->m_is_has_errors = 1;

	//	newshader = new FWrender::Shader();
	//	newshader->createFromMemory(this->m_vss.c_str(), this->m_fss.c_str());
	//	
	//	delete this->m_shader; this->m_shader = NULL;
	//	this->m_shader = newshader;

	//	this->m_is_has_errors = 0;

	//	return 0; // success
	//} 
	//catch(FWdebug::ShaderException *e){
	//	this->fillErrors(e);

	//	FWcore::writeLog(true, FWcore::MSG_error, "Shader Exception:\n %s", e->what());
	//	/*MessageBox(NULL, e->getCompilerError(), "ShEX", MB_ICONEXCLAMATION);*/
	//	
	//	delete newshader;
	//	delete e;

	//	return 1;
	//}
	//catch(FWdebug::Exception *e){
	//	throw e;	// tovabb 
	//}

	return 0;	// fail
}

void CShaderEditorDoc::flushErrors(){
	//this->m_errors.rewind();	
}

//////////////////////////////////////////////////////////////////////
// Shader hibakezeles
//////////////////////////////////////////////////////////////////////

void CShaderEditorDoc::fillErrors(ShaderException& ex){
	//if (!ex) return;
	//
	//const char* err_str = ex->getCompilerError(); 
	//int len = strlen(err_str); 
	//char* str = (char*) malloc(len+1); strcpy(str, err_str);
	//char **lines = NULL;

	//int line_count = string_split_lines(str, lines);

	//if (!lines || line_count<=0) return;

	//this->flushErrors();

	//enum FWdebug::ShaderException::SHADER_EXCEPT_TYPE_e exceptType = ex->getType();
	//enum FWdebug::ShaderException::SHADER_ERROR_TYPE_e errorType;

	//const char *token = NULL;

	//for(int i=0; i<line_count; i++){
	//	if (!lines[i] || !lines[i][0]) continue;
	//	if(token = strstr(lines[i], "ERROR"))
	//	{
	//		errorType = ShaderException::SHE_error;
	//	} 
	//	else if (token = strstr(lines[i], "WARNING"))
	//	{
	//		errorType = ShaderException::SHE_warning;
	//	} 
	//	else
	//	{
	//		token = lines[i];
	//		errorType = ShaderException::SHE_message;
	//	}

	//	///@todo folyt kov. 

	//	FWcore::writeLog2("%s", lines[i]);
	//}

	//free(str);
}

/*
ShaderError::ShaderError(){
	//line = -1;
	//errorType = ShaderException::SHE_INVALID;
	//exceptType = ShaderException::SH_INVALID;
}
*/

