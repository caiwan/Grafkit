#pragma once

#include "utils/Command.h"
#include "utils/Event.h"

#include "animation/animation.h"

namespace Idogep
{
	class View;

	class CurveKeyChangeCommand : public Command 
	{
	public:
		CurveKeyChangeCommand(Ref<Grafkit::Animation::Channel> & channel, size_t keyId, Grafkit::Animation::Key oldKey, Grafkit::Animation::Key newKey, View * const & enclosingView);
		virtual void Do();
		virtual void Undo();

		//Event<bool> onRefreshView;

	private:
		Ref<Grafkit::Animation::Channel> &m_channel;
		size_t m_id;
		Grafkit::Animation::Key m_newKey;
		Grafkit::Animation::Key m_oldKey;
		View * m_enclosingView;
	};

	class CurveKeyAddCommand {

	};

	class CurveKeyRemoveCommand {

	};

}
