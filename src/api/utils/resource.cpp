#include "stdafx.h"

#include <algorithm>
#include <cstdlib>
#include <chrono>

#include "resource.h"

using namespace Grafkit;
using namespace std::chrono;

void IResource::_serialize(Archive& ar) {
	PERSIST_FIELD(ar, m_id);
	PERSIST_STRING(ar, m_name);
}

Grafkit::IResource::IResource()
{
	GenerateID();
}

Grafkit::IResource::~IResource()
{
}

// Quick and dirty crap
// to clamp down values to create seed and random
template <typename T, typename U> inline U crawl(T in) {
	U out = 0;

	const int sIn = sizeof(in), sOut = sizeof(out);	
	
	BYTE* pOut = (BYTE*)&out;
	const BYTE* pIn = (BYTE*)&in;

	for (int i = 0; i < max(sIn, sOut); ++i) {
		pOut[i % sOut] ^= pIn[i % sIn];
	}

	return out;
}

// generates a totallz random tihng
ULONG Grafkit::IResource::GenerateID()
{
	srand(crawl<milliseconds, UINT>(duration_cast<milliseconds>(system_clock::now().time_since_epoch())));
	
	m_id = 0;
	BYTE * const pId = (BYTE*)&m_id;
	for (int i = 0; i < sizeof(m_id); i++) {
		pId[i] = crawl<int, BYTE>(rand());
	}
	return m_id;
}
