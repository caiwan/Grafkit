#pragma once

#include "Command.h"
#include "Event.h"

#include "animation/animation.h"

namespace Idogep
{
	class CurveKeyChangeCommand : public Command 
	{
	public:
		CurveKeyChangeCommand(Ref<Grafkit::Animation::Channel> & channel, size_t keyId, Grafkit::Animation::Key oldKey, Grafkit::Animation::Key newKey);
		virtual void Do();
		virtual void Undo();

		Event<bool> onRefreshView;

	private:
		Ref<Grafkit::Animation::Channel> &m_channel;
		size_t m_id;
		Grafkit::Animation::Key m_newKey;
		Grafkit::Animation::Key m_oldKey;

	};

	class CurveKeyAddCommand {

	};

	class CurveKeyRemoveCommand {

	};

}
