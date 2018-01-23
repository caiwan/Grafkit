#include "stdafx.h"

#include <algorithm>
#include <cstdlib>
#include <chrono>

#include "resource.h"

using namespace Grafkit;
using namespace std::chrono;

Grafkit::IResource::IResource()
{
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

