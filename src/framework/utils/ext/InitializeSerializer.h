#pragma once

/** Force collect of all the clonable classes for serializer due to failures of automatic collection in global namespace */

// Kinda obsolete
#pragma message("Obsolete; General serializer initalization stuff should work fine.")

namespace Grafkit{
	
	class ClonableInitializer {
	protected:
		ClonableInitializer();
	};
	
}
