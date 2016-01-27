/**
A shader docot tartalmazza + ujraforditja + shader hibakat kezeli
*/

#pragma once

//#include <vector>
#include <list>
#include <map>
#include <string>

#include "render/renderer.h"
#include "render/shader.h"
#include "reference.h"
#include "tree.h"

#include "PropertyView.h"



class CShaderSrcDoc : public CPropertyView::IBuilder, public CPropertyView::IProperyItemHandler
{
	/**
	A szerkeszto oldalon kezeli a shader forrast + ujraforditja + reflektalja a shader parameterket a szerkeszto fele 
	*/
public :
	class BufferRecord;

	typedef Ref<BufferRecord> BufferRecordRef;
	typedef std::map<std::string, BufferRecordRef> mapBufferRecord_t;

	class VariableElement;

	typedef Ref<VariableElement> VariableElementRef;

public:
	CShaderSrcDoc();
	~CShaderSrcDoc();

	int CompileShader(FWrender::Renderer &render);
	void FlushErrors();

	const int IsHasErrors() const {return m_is_has_errors;}	///< @return 1 ha tortent hiba a forditas soran, kulonben 0 

	FWrender::ShaderRef GetShader(){return this->m_shader;}
		
	void SetShaderSource(TCHAR *in_src) {this->m_shader_source = in_src; }
	CString &GetShaderSource() { return this->m_shader_source;}

	//mapBufferRecord_t& GetBuffers() { return m_lBuffers; }

	// --- 
public:
	virtual void operator() (CPropertyView &props);

protected:
	void ParseVars(VariableElementRef& variable, CMFCPropertyGridProperty *parentProperty);
	virtual void PropertyChangedEvent(NodeIterator* item);

protected:
	CString m_shader_source;

	FWrender::ShaderRef m_shader;	///< Shader objektum, amit frissiteni kell.
	int m_is_has_errors;

	mapBufferRecord_t m_lBuffers;

private:
	void FillErrors(FWdebugExceptions::ShaderException& ex);	///< exception szovegebol feltolti a hibalistat


	// ---- 
public:
	class BufferRecord : virtual public Referencable, virtual public NodeIterator 
	{
		/**
		A constant/texture buffereket reflektalja at a shaderbol a szerkeszto oldalra
		illetve kezeli az MFC elemeket, amiket a szerkeszto fog rajzolgani kifele a property viewban
		*/
	public:
		class VariableElement;

		typedef std::map<std::string, VariableElementRef> mapVariables_t;

		friend class CShaderSrcDoc;
		friend class CShaderSrcDoc::BufferRecord::VariableElement;
	
	public:
		BufferRecord();
		~BufferRecord();

		// ezeknek majd idovel privatenak kellene lennie
	public:
		int m_is_valid;
		size_t m_id; 
		mapVariables_t m_lVariables;
		std::string m_name;

		D3D11_SHADER_BUFFER_DESC m_desc;

	};

	class VariableElement : virtual public Referencable, virtual public NodeIterator {
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
		std::string m_name;

		D3D11_SHADER_VARIABLE_DESC m_varDesc;
		D3D11_SHADER_TYPE_DESC m_typeDesc;

		CMFCPropertyGridProperty* m_propGroup;
		CMFCPropertyGridProperty* m_propSource;
		CMFCPropertyGridProperty* m_propType;
		std::list<CMFCPropertyGridProperty*> m_lProps;
	};
};

// #include "compareOperators.h"

