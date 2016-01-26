/**
A shader docot tartalmazza + ujraforditja + shader hibakat kezeli
*/


#pragma once

#include "render/renderer.h"
#include "render/shader.h"
#include "reference.h"

#include "PropertyView.h"

#define USE_STD_VECTOR


#ifdef USE_STD_VECTOR
#include <vector>
#endif //USE_STD_VECTOR

class CShaderSrcDoc : public CPropertyView::IBuilder, public CPropertyView::IProperyItemHandler
{
	/**
	A szerkeszto oldalon kezeli a shader forrast + ujraforditja + reflektalja a shader parameterket a szerkeszto fele 
	*/
public :
	class BufferRecord;

	typedef Ref<BufferRecord> BufferRecordRef;

#ifndef USE_STD_VECTOR
	typedef CList<BufferRecordRef, BufferRecordRef> listBufferRecord_t;
#else //USE_STD_VECTOR
	typedef std::vector<BufferRecordRef> listBufferRecord_t;
#endif USE_STD_VECTOR

public:
	CShaderSrcDoc();
	~CShaderSrcDoc();

	int CompileShader(FWrender::Renderer &render);
	void FlushErrors();

	const int IsHasErrors() const {return m_is_has_errors;}	///< @return 1 ha tortent hiba a forditas soran, kulonben 0 

	FWrender::ShaderRef GetShader(){return this->m_shader;}
		
	void SetShaderSource(TCHAR *in_src) {this->m_shader_source = in_src; }
	CString &GetShaderSource() { return this->m_shader_source;}

	listBufferRecord_t& GetBuffers() { return m_lBuffers; }

	// --- 
public:
	virtual void operator() (CPropertyView &props);
protected:
	virtual void PropertyChangedEvent(NodeIterator* item);

protected:
	CString m_shader_source;

	FWrender::ShaderRef m_shader;	///< Shader objektum, amit frissiteni kell.
	int m_is_has_errors;

	listBufferRecord_t m_lBuffers;

private:
	void FillErrors(FWdebugExceptions::ShaderException& ex);	///< exception szovegebol feltolti a hibalistat


	// ---- 
public:
	class BufferRecord : public Referencable{
		/**
		A constant/texture buffereket reflektalja at a shaderbol a szerkeszto oldalra
		illetve kezeli az MFC elemeket, amiket a szerkeszto fog rajzolgani kifele a property viewban
		*/
	public:
		class VariableElement;

		typedef Ref<VariableElement> VariableElementRef;

#ifndef USE_STD_VECTOR
		typedef CList<VariableElementRef, VariableElementRef> listVariables_t;
#else //USE_STD_VECTOR
		typedef std::vector<VariableElementRef> listVariables_t;
#endif // USE_STD_VECTOR

		friend class CShaderSrcDoc;
		friend class CShaderSrcDoc::BufferRecord::VariableElement;
	
	public:
		BufferRecord();
		~BufferRecord();

		listVariables_t& GetVariables() { return m_lVariables; }

		// ezeknek majd idovel privatenak kellene lennie
	public:
		int m_is_valid;
		size_t m_id; 
		listVariables_t m_lVariables;
		CString m_name;

		D3D11_SHADER_BUFFER_DESC m_desc;

		// ---- 
	public:
		class VariableElement : public Referencable{
			/**
			A constant/texture buffer mezoit reflektalja at a shaderbol a szerkeszto oldalra
			illetve kezeli az MFC elemeket, amiket a szerkeszto fog rajzolgani kifele a property viewban
			*/

			friend class CShaderSrcDoc;
			friend class CShaderSrcDoc::BufferRecord;
		public:
			VariableElement();
			

			// ezeknek majd idovel privatenak kellene lennie
		public:
			int m_is_valid;
			size_t m_id;
			CString m_name;

			D3D11_SHADER_VARIABLE_DESC m_varDesc;
			D3D11_SHADER_TYPE_DESC m_typeDesc;
		};
	};
};

// #include "compareOperators.h"

