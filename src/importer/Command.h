#pragma once 

#include "utils/reference.h"

#include "json.hpp"
#include "Environment.h"

namespace GKimporter {

	class Command : public Referencable {
	public:
		virtual void Evaluate(Environment*& env, nlohmann::json json) = 0;
		virtual std::string GetCommandString() = 0;

	};

}