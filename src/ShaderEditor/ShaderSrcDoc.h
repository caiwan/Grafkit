#pragma once

#include "render/shader.h"

/**
	A shader docot tartalmazza + ujraforditja + shader hibakat kezeli 
*/

class CShaderSrcDoc{
		// -- class interface
	public:
		//CShaderSrcDoc();
		CShaderSrcDoc(const char* _shader_source = "");
		~CShaderSrcDoc();

		int compileShader();
		void flushErrors();

		const int isHasErrors() const {return m_is_has_errors;}	///< @return 1 ha tortent hiba a forditas soran, kulonben 0 

		FWrender::ShaderRef getShader(){return this->m_shader;}

	protected:
		FWrender::ShaderRef m_shader;	///< Shader objektum, amit frissiteni kell.
		int m_is_has_errors;

	private:
		void fillErrors(FWdebugExceptions::ShaderException& ex);	///< exception szovegebol feltolti a hibalistat
};

