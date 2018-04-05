

#pragma once 

#include <vector>
#include <qstring.h>
#include <qvariant.h>

#include "common.h"

namespace Idogep{

	class Property {

	public:

	private:
		QString m_time;
		QVariant m_value;

	};

	class Properties : virtual public Referencable {

	public :

		void AddProperty(Property*) {

		}

		Property* GetProperty() {

		}

		size_t GetPropertyCount() {

		}

	private:

		std::vector<Property*> m_properties;

	};

}
