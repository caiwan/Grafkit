#include <vector>

#include "stdafx.h"

#include "persistence.h"
#include "dynamics.h"

/**
	Persistent
*/

using namespace FWdebugExceptions;
using namespace Grafkit;
using namespace std;

void Persistent::store(Archive& ar)
{
	string className = this->getClassName();
	UCHAR ver = this->version();

	Log::Logger().Info("Storing object %s %d", className.c_str(), ver);

	PERSIST_STRING(ar, className);
	PERSIST_FIELD(ar, ver);

	this->serialize(ar);
}

Persistent* Persistent::load(Archive& ar)
{
	string className;
	UCHAR ver = 0;
	PERSIST_STRING(ar, className);

	Clonable* clone = Clonables::Instance().create(className.c_str());
	if (clone == NULL) {
#ifdef DEBUG
		Clonables::Instance().dumpClonables();
#endif
		throw new EX_DETAILS(PersistentCreateObjectExcpetion, className.c_str());
	}

	Persistent * obj = dynamic_cast<Persistent *>(clone);
	if (obj == NULL) {
		throw new EX_DETAILS(PersistentCreateObjectExcpetion, className.c_str());
	}

	PERSIST_FIELD(ar, ver);

	Log::Logger().Info("Loading object %s %d", className.c_str(), ver);

	//
	if (ver != obj->version()) {
		throw new EX(PersistentVersionMismatch);
	}

	obj->serialize(ar);

	return obj;
}

/**
	Archive
*/

Archive::Archive(int IsStoring) :
	m_isStoring(IsStoring)
{
}

Archive::~Archive()
{

}

size_t Grafkit::Archive::WriteString(const char * input)
{
	USHORT slen = strlen(input);
	this->Write(&slen, sizeof(slen));
	this->Write(input, slen + 1);
	return slen;
}

size_t Grafkit::Archive::ReadString(char*& output)
{
	USHORT slen = 0;
	this->Read(&slen, sizeof(slen));
	output = new char[slen + 1];
	this->Read(output, slen + 1);
	return slen;
}

void Grafkit::Archive::PersistString(const char *& str)
{
	if (m_isStoring) {
		WriteString(str);
	}
	else {
		char *in = nullptr;
		ReadString(in);
		str = in;
	}
}

void Grafkit::Archive::PersistString(std::string &str)
{
	if (m_isStoring) {
		WriteString(str.c_str());
	}
	else {
		char *in = nullptr;
		ReadString(in);
		str = in;
		delete[] in;
	}
}

void Grafkit::Archive::StoreObject(Persistent * object)
{
	UCHAR isNotNull = object != nullptr;
	PersistField(isNotNull);

	if (isNotNull)
		object->store(*this);
}

Persistent * Grafkit::Archive::LoadObject()
{
	UCHAR isNotNull = 0;
	PersistField(isNotNull);

	if (isNotNull)
		return Persistent::load(*this);
}

