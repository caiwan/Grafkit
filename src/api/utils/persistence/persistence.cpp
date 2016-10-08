#define USE_STD_ARRAY
#include <vector>

#include "../../stdafx.h"
#include "../exceptions.h"

#include "persistence.h"
#include "dynamics.h"



/**
	Persistent
*/

using namespace FWdebugExceptions;
using namespace Grafkit;
using namespace std;

void Persistent::store(Archive& stream) const 
{
#if 0 // ha nincs RTTI, akkor ez nem jo valasztas
	string className = typeid(*this).name();
	className = className.substr(className.find(' ') + 1);
#else
	string className(this->getClassName());
#endif
	
	//stream << className;
	
	int ver = version();
	//stream << ver;
	
	stream.setDirection(true);
	
	const_cast<Persistent *>(this)->serialize(stream);
	
}

void Grafkit::Persistent::serialize(Archive & stream)
{
}

Persistent* Persistent::load(Archive& stream)
{           
	string className;
	
	//stream >> className;

	Clonable* clone = Clonables::Instance().create(className.c_str());
	if (clone == NULL) {
		throw EX_DETAILS(PersistentCreateObjectExcpetion, className.c_str());
	}
	auto_ptr<Clonable> delitor(clone);

	Persistent * obj = dynamic_cast<Persistent *>(clone);
	if(obj == NULL) {
		/// @todo
		throw EX_DETAILS(PersistentCreateObjectExcpetion, className.c_str());
	}

	int ver = -1;

	//stream >> ver;

	if (ver != obj->version()) {
		throw EX(PersistentVersionMismatch);
	}
	stream.setDirection(false);

	obj->serialize(stream);
	delitor.release();

	return obj;
}

/**
	Archive
*/

Archive::Archive(int isStoring) : 
	_isStoring(isStoring)
{
}

Archive::~Archive()
{

}

size_t Grafkit::Archive::writeString(const char * input)
{
	unsigned int slen = strlen(input);

	this->write(&slen, sizeof(slen));
	this->write(input, slen+1);

	return slen;
}

size_t Grafkit::Archive::readString(char*& output)
{
	unsigned int slen = 0;

	this->read(&slen, sizeof(slen));

	output = new char[slen+1];

	this->read(output, slen + 1);

	return slen;
}

/**
	Persistent helpers
*/


void Grafkit::PersistString::store(Archive & ar)
{
	const char *out = nullptr;
	if (m_pString) {
		out = m_pString->c_str();
	}
	else {
		out = *m_pszString;
	}
	ar.writeString(out);
}

void Grafkit::PersistString::load(Archive & ar)
{
	char *in = nullptr;
	ar.readString(in);
	if (m_pString) {
		m_pString->clear();
		m_pString->assign(in);
	}
	else {
		*m_pszString = in;
	}
}
