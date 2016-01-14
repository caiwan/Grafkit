#pragma once

// #include "render/renderer.h"
#include "render/shader.h"

/*
struct ShaderError{
	enum FWdebug::ShaderException::SHADER_EXCEPT_TYPE_e exceptType;
	enum FWdebug::ShaderException::SHADER_ERROR_TYPE_e errorType;
	int line;
	//CString what;
	std::string what;

	ShaderError();
};
*/

/**
	A shader docot tartalmazza + ujraforditja + shader hibakat kezeli 
*/

class CShaderEditorDoc{
		// -- class interface
	public:
		//CShaderEditorDoc();
		CShaderEditorDoc(const char* _shader_source = "");
		~CShaderEditorDoc();

		int compileShader();
		void flushErrors();

		const int isHasErrors() const {return m_is_has_errors;}	///< @return 1 ha tortent hiba a forditas soran, kulonben 0 
		// const std::vector<ShaderError> &getErrorList() const {return this->m_errors;}

		FWrender::Shader*& getShader(){return this->m_shader;}

		//std::string& getVSS(){return this->m_vss;}	
		//std::string& getFSS(){return this->m_fss;}	

		//const int isFSSOnly() const {return m_is_FSS_Only;}

	protected:
		FWrender::Shader *m_shader;	///< Shader objektum, amit frissiteni kell.
		// int m_is_FSS_Only, m_is_has_errors;
		int m_is_has_errors;
		//std::string m_vss, m_fss;

	private:
		void fillErrors(FWdebugExceptions::ShaderException& ex);	///< exception szovegebol feltolti a hibalistat

		// std::vector<ShaderError> m_errors;
};

