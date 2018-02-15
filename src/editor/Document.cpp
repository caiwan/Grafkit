#include "math/fbm.h"

#include "Document.h"

using namespace Idogep;
using namespace Grafkit;

Idogep::EditorDocument::EditorDocument()
{
	//m_testAnimation = new ActorAnimation();
	for (int i = 0; i < 35; i++) {
		m_testAnimation.AddKey(Animation::FloatKey(i, PseudoRandom::Random()));
	}
}
