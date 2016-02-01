// #include "../stdafx.h"
#include "stdafx.h"

#include "ShaderSrcDoc.h"

#include "render/shader.h"
#include "exceptions.h"

using FWdebug::Exception;
using namespace FWdebugExceptions;
using FWrender::Shader;

// aliases for nested classes 
using BufferRecord = CShaderSrcDoc::BufferRecord;
//using VariableElement = CShaderSrcDoc::BufferRecord::VariableElement;
using VariableElement = CShaderSrcDoc::VariableElement;
//using VariableElementRef = CShaderSrcDoc::BufferRecord::VariableElementRef;
using VariableElementRef = CShaderSrcDoc::VariableElementRef;

//#include "compareOperators.h"

//////////////////////////////////////////////////////////////////////
// Shader dokumentum
//////////////////////////////////////////////////////////////////////

CShaderSrcDoc::CShaderSrcDoc() : 
	m_shader_source(),
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
		newshader->LoadFromMemory(render, "main", m_shader_source, m_shader_source.GetLength(), FWrender::ST_Pixel);
	
		m_shader = newshader;

		// mark every existing cbuffer as invalid
		for (mapBufferRecord_t::iterator record_it = m_lBuffers.begin(); record_it != m_lBuffers.end(); record_it++) 
		{
			BufferRecordRef &record = record_it->second;
			record->m_is_valid = 0;
				
			for(BufferRecord::mapVariables_t::iterator var_it = record->m_lVariables.begin(); var_it != record->m_lVariables.end(); var_it++) 
			{
				VariableElementRef &variable = var_it->second;
				variable->m_is_valid = 0;
			}
		}
		
		
		// reflect buffers, and validate exisitng ones
		size_t buffer_count = this->m_shader->GetConstantBufferCount();
		for (size_t i = 0; i < buffer_count; i++){
			BufferRecord *record = new BufferRecord();
			size_t var_count = m_shader[i].GetElementCount();
			
			record->m_id = i;
			record->m_desc = m_shader[i].GetBufferDesc();
			record->m_name = m_shader[i].GetBufferDesc().Name;
			record->m_is_valid = 1;

			for (size_t j = 0; j < var_count; j++) {
				VariableElement *variable = new VariableElement();
				variable->m_id = j;
				variable->m_varDesc = m_shader[i][j].GetVarDesc();
				variable->m_typeDesc = m_shader[i][j].GetTypeDesc();
				variable->m_name = variable->m_varDesc.Name;
				//variable->parent = record;
				variable->m_is_valid = 1;

				record->m_lVariables[variable->m_name] = (variable);

			}
			this->m_lBuffers[record->m_name] = (record);
		}

		this->m_is_has_errors = 0;
	
	} 
	catch(ShaderException &e){
		this->FillErrors(e);

		MessageBox(NULL, e.what(), "ShEX", MB_ICONEXCLAMATION);

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


void CShaderSrcDoc::operator() (CPropertyView & wndPropList)
{
	wndPropList.SetHandler(this);
	wndPropList.RemoveAll();

	for (mapBufferRecord_t::iterator record_it = m_lBuffers.begin(); record_it != m_lBuffers.end(); record_it++)
	{
		CShaderSrcDoc::BufferRecordRef &record = record_it->second;
		CMFCPropertyGridProperty* recPropGrp = new CMFCPropertyGridProperty(record->m_name.c_str());

		for (BufferRecord::mapVariables_t::iterator var_it = record->m_lVariables.begin(); var_it != record->m_lVariables.end(); var_it++)
		{
			VariableElementRef &variable = var_it->second;
			CMFCPropertyGridProperty* varPropGrp = new CMFCPropertyGridProperty(variable->m_name.c_str());

			ParseVars(variable, varPropGrp);

			recPropGrp->AddSubItem(varPropGrp);
		}

		// ...
		// ha egy adott elem eppen nem erheto el, akkor (...) 

		wndPropList.AddProperty(recPropGrp);
	}
}

void CShaderSrcDoc::ParseVars(VariableElementRef & variable, CMFCPropertyGridProperty * parentProperty)
{
	variable->m_propGroup = parentProperty;

	switch (variable->m_typeDesc.Class) {
		case D3D_SVC_SCALAR:
		{
			CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Value"), (_variant_t)0l, _T(""));
			pProp->EnableSpinControl(TRUE, 0, 0);
			parentProperty->AddSubItem(pProp);

			variable->m_lProps.push_back(pProp);
		}
			break;

		case D3D_SVC_VECTOR:
		{
			variable->m_propType = new CMFCPropertyGridProperty(_T("Type"), _T("Vector"), _T(""));
			variable->m_propType->AddOption(_T("Vector"));
			variable->m_propType->AddOption(_T("Color"));
			variable->m_propType->AllowEdit(FALSE);

			parentProperty->AddSubItem(variable->m_propType);

			const char *fields_names = "XYZW";
			for (size_t i= 0;  i<variable->m_typeDesc.Columns; ++i) {
				CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(CString(fields_names[i]), (_variant_t)0l, _T(""));
				pProp->EnableSpinControl(TRUE, 0, 0);
				parentProperty->AddSubItem(pProp);

				variable->m_lProps.push_back(pProp);
			}
		}
			break;
		case D3D_SVC_MATRIX_ROWS:
		case D3D_SVC_MATRIX_COLUMNS:
			break;

		case D3D_SVC_OBJECT:
		case D3D_SVC_STRUCT:

			break;
	}

	switch (variable->m_typeDesc.Class) {
		case D3D_SVT_INT:
		case D3D_SVT_FLOAT:
		case D3D_SVT_UINT:
		case D3D_SVT_UINT8:
		case D3D_SVT_DOUBLE:
			break;
		case D3D_SVT_TEXTURE2D:
		case D3D_SVT_SAMPLER2D:
			break;
	}


	variable->m_propSource = new CMFCPropertyGridProperty(_T("Source"), _T("Manual"), _T(""));
	variable->m_propSource->AddOption(_T("Manual"));
	variable->m_propSource->AddOption(_T("Scenegraph"));
	variable->m_propSource->AllowEdit(FALSE);

	parentProperty->AddSubItem(variable->m_propSource);
}

void CShaderSrcDoc::PropertyChangedEvent(TreeNode * item)
{
	// hello
	throw EX(NotImplementedMethodException);
}


//////////////////////////////////////////////////////////////////////
// Shader hibakezeles
//////////////////////////////////////////////////////////////////////

void CShaderSrcDoc::FillErrors(ShaderException& ex){
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

// ===================================================================================================

BufferRecord::BufferRecord():
	m_is_valid(0),
	m_id(0),
	m_lVariables(),
	m_name(),
	m_desc()
{
}

BufferRecord::~BufferRecord()
{
}

// ===================================================================================================
VariableElement::VariableElement() :
	m_is_valid(0),
	m_id(0),
	m_name(),
	m_varDesc(),
	m_typeDesc()
{
}