// #include "../stdafx.h"
#include "stdafx.h"

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

CShaderSrcDoc::CShaderSrcDoc() : 
	m_shader_source(""),
	m_shader(),
	m_is_has_errors(0)
{
}

CShaderSrcDoc::~CShaderSrcDoc(){
}

int CShaderSrcDoc::CompileShader(FWrender::Renderer &render){
	// shader objektum cserevel dolgozik
	FWrender::ShaderRef newshader;

	try{
		this->m_is_has_errors = 1;

		newshader = new FWrender::Shader();
		//newshader->createFromMemory(this->m_vss.c_str(), this->m_fss.c_str());
		newshader->LoadFromMemory(render, "main", m_shader_source, m_shader_source.GetLength(), FWrender::ST_Pixel);
	
		m_shader = newshader;

		this->m_is_has_errors = 0;
	
	} 
	catch(ShaderException &e){
		this->fillErrors(e);

	//	FWcore::writeLog(true, FWcore::MSG_error, "Shader Exception:\n %s", e->what());
		MessageBox(NULL, e.what(), "ShEX", MB_ICONEXCLAMATION);
	//	
	//	delete newshader;
	//	delete e;

		///@ todo add logger

		return 1;
	}
	//catch(FWdebug::Exception *e){
	//	throw e;	// tovabb 
	//}

	return 0;
}

void CShaderSrcDoc::FlushErrors(){
	//this->m_errors.rewind();	
}

//////////////////////////////////////////////////////////////////////
// Shader hibakezeles
//////////////////////////////////////////////////////////////////////

void CShaderSrcDoc::fillErrors(ShaderException& ex){
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

