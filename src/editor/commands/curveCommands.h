#pragma once

#include "utils/Command.h"
#include "animation/animation.h"

namespace Idogep
{
	class CurvePointItem;

	class CurveKeyChangeCommand : public Command 
	{
	public:
		CurveKeyChangeCommand(Ref<Grafkit::Animation::Channel> & channel, size_t keyId, Grafkit::Animation::Key oldKey, Grafkit::Animation::Key newKey, Ref<CurvePointItem> pointItem);
        ~CurveKeyChangeCommand();
	    void Do() override;
	    void Undo() override;

	private:
		Ref<Grafkit::Animation::Channel> &m_channel;
		size_t m_id;
		Grafkit::Animation::Key m_newKey;
		Grafkit::Animation::Key m_oldKey;
		Ref<CurvePointItem> m_pointItem;
	};

	class CurveKeyAddCommand {

	};

	class CurveKeyRemoveCommand {

	};

}
