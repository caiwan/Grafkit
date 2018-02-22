#include "math/fbm.h"

#include "Document.h"

#include "ui/curve/curvedoc.h"

using namespace Idogep;
using namespace Grafkit;

Idogep::EditorDocument::EditorDocument()
{
	m_testAnimation = new Animation::FloatTrack();
	for (int i = 0; i < 35; i++) {
		m_testAnimation->AddKey(Animation::FloatKey(i, PseudoRandom::Random()));
	}

	m_testCurveDocument = new CurveDocument(m_testAnimation);
}
