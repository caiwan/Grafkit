#pragma once

/** Force collect of all the clonable classes for serializer due to failures of automatic collection in global namespace */

namespace Grafkit{
	
	class ClonableInitializer {
	protected:
		ClonableInitializer();
	};
	
}
