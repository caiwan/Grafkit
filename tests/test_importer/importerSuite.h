#pragma once 

#include "ImporterTool.h"

/** A quick and dirty way to instantitate and execute importer before running tests with it
*/

class ImporterSuite : public GKimporter::ImporterTool {
public:
	ImporterSuite();
	~ImporterSuite();

	int Execute(int argc, char **argv);
	void Execute(const char * name = nullptr);
};

