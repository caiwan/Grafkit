#pragma once 

namespace GKimporter {

	// importer tool

	class ImporterTool {

	public:
	// ... 

	protected:
		BlenderThread *m_blenderThread;
		ServerThread *m_serverThread;
		ParserThread *m_parserThread;

	};
}