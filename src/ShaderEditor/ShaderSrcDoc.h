#pragma once

#include "render/renderer.h"
#include "render/shader.h"

/**
	A shader docot tartalmazza + ujraforditja + shader hibakat kezeli 
*/

class CShaderSrcDoc{
		// -- class interface
	public:
		CShaderSrcDoc();
		//CShaderSrcDoc(const char* _shader_source = "");
		~CShaderSrcDoc();

		int CompileShader(FWrender::Renderer &render);
		void FlushErrors();

		const int IsHasErrors() const {return m_is_has_errors;}	///< @return 1 ha tortent hiba a forditas soran, kulonben 0 

		FWrender::ShaderRef GetShader(){return this->m_shader;}
		
		void SetShaderSource(TCHAR *in_src) {this->m_shader_source = in_src; }
		CString &GetShaderSource() { return this->m_shader_source;}

	protected:
		CString m_shader_source;

		FWrender::ShaderRef m_shader;	///< Shader objektum, amit frissiteni kell.
		int m_is_has_errors;

		typedef CMap<LPCTSTR, LPCTSTR, size_t, size_t> mapCbufferVariables_t;
		typedef CMap<LPCTSTR, LPCTSTR, mapCbufferVariables_t, mapCbufferVariables_t> mapCbuffers_t;

		//mapCbuffers_t m_mapCbuffers;
		mapCbufferVariables_t m_mapCbufferVairables;


	private:
		void fillErrors(FWdebugExceptions::ShaderException& ex);	///< exception szovegebol feltolti a hibalistat
};

// #include "compareOperators.h"

