#include "demoframework.h"

using namespace GKDemo;
using namespace Grafkit;

GKDemo::DemoApplication::DemoApplication() : 
	ClonableInitializer(), 
	System(), 
	ResourcePreloader(), m_file_loader(nullptr)
{
}

GKDemo::DemoApplication::~DemoApplication()
{
}

int GKDemo::DemoApplication::init()
{
	InitializeLoaderBar(render);
	LoadCache();

	// 1. before preload

	DoPrecalc();

	// 2. after preload 

	// 3. delegate tracks

	// 4. setup maps

	return 0;
}

int GKDemo::DemoApplication::mainloop()
{
	// before render

	// render

	// after render

	return 0;
}

void GKDemo::DemoApplication::release()
{
	// cleanup? shutown? something?
}

// ------------------------------------------------------

void GKDemo::DemoApplication::UpdateLoaderBar(float p)
{
	DrawLoaderBar(render, p);
}
