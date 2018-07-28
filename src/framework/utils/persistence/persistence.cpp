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

void Persistent::Store(Archive& ar)
{
	string className = this->GetClazzName();
	UCHAR ver = this->GetVersion();

	Log::Logger().Info("Storing object %s %d", className.c_str(), ver);

	PERSIST_STRING(ar, className);
	PERSIST_FIELD(ar, ver);

	this->Serialize(ar);
}

Persistent* Persistent::Load(Archive& ar)
{
	string className;
	UCHAR ver = 0;
	PERSIST_STRING(ar, className);

	Clonable* clone = Clonables::Instance().Create(className.c_str());
	if (clone == nullptr) {
#ifdef DEBUG
		Clonables::Instance().dumpClonables();
#endif
		THROW_EX_DETAILS(PersistentCreateObjectExcpetion, className.c_str());
	}

	Persistent * obj = dynamic_cast<Persistent *>(clone);
	if (obj == nullptr) {
		THROW_EX_DETAILS(PersistentCreateObjectExcpetion, className.c_str());
	}

	PERSIST_FIELD(ar, ver);

	Log::Logger().Info("Loading object %s %d", className.c_str(), ver);

	//
	if (ver != obj->GetVersion()) {
		THROW_EX(PersistentVersionMismatch);
	}

	obj->Serialize(ar);

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

size_t Archive::WriteString(const char * input)
{
	USHORT slen = strlen(input);
	this->Write(&slen, sizeof(slen));
	this->Write(input, slen + 1);
	return slen;
}

size_t Archive::ReadString(char*& output)
{
	USHORT slen = 0;
	this->Read(&slen, sizeof(slen));
	output = new char[slen + 1];
	this->Read(output, slen + 1);
	return slen;
}

void Archive::PersistString(const char *& str)
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

void Archive::PersistString(string &str)
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

void Archive::StoreObject(Persistent * object)
{
	UCHAR isNotNull = object != nullptr;
	PersistField(isNotNull);

	if (isNotNull)
		object->Store(*this);
}

Persistent * Archive::LoadObject()
{
	UCHAR isNotNull = 0;
	PersistField(isNotNull);

	if (isNotNull)
		return Persistent::Load(*this);
    return nullptr;
}

