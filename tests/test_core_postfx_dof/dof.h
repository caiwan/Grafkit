
#pragma once 

#include "render/effect.h"

namespace Grafkit{
	class DofEffect {

	public:
		DofEffect();
		~DofEffect();

		void Preload();

		void Initialize();

	private:
		EffectComposerRef fxPrecalc;
		EffectComposerRef fxPass[3];
		EffectComposerRef fxCombine;

		ShaderResRef fsPrecalc;
		ShaderResRef fsBlur;
		ShaderResRef fsCombine;
	};

}


