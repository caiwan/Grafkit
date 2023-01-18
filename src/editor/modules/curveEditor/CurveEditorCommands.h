#pragma once

#include "utils/Command.h"
#include "animation/animation.h"

namespace Idogep
{
    class CurvePointEditor;

	class CurveKeyChangeCommand : public Command 
	{
	public:
		CurveKeyChangeCommand(Ref<Grafkit::Animation::Channel> & channel, size_t keyId, Grafkit::Animation::Key oldKey, Grafkit::Animation::Key newKey, Ref<CurvePointEditor> pointItem);
        ~CurveKeyChangeCommand();
	    void Do() override;
	    void Undo() override;

	private:
		Ref<Grafkit::Animation::Channel> &m_channel;
		size_t m_id;
		Grafkit::Animation::Key m_newKey;
		Grafkit::Animation::Key m_oldKey;
        Ref<CurvePointEditor> m_pointEditor;

	};

	class CurveKeyAddCommand {

	};

	class CurveKeyRemoveCommand {

	};

}
