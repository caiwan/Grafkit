#pragma once

#include "utils/Command.h"

#include "animation/animation.h"

namespace Idogep
{
    class CurvePointEditor;

	class CurveKeyChangeCommand : public Command 
	{
	public:
		CurveKeyChangeCommand(Grafkit::Animation::ChannelRef & channel, size_t keyId, Grafkit::Animation::Key oldKey, Grafkit::Animation::Key newKey, CurvePointEditor* pointEditor);
        ~CurveKeyChangeCommand();
	    void Do() override;
	    void Undo() override;

	private:
        Grafkit::Animation::ChannelRef m_channel;
		size_t m_id;
		Grafkit::Animation::Key m_newKey;
		Grafkit::Animation::Key m_oldKey;
        CurvePointEditor *m_controller;
	};

	class CurveChangeCommand : public Command
    {
	public:
	    CurveChangeCommand(const Grafkit::Animation::TrackRef& track, size_t id, const Grafkit::Animation::ChannelRef& oldChannel, const Grafkit::Animation::ChannelRef& newChannel, CurvePointEditor* controller);
        ~CurveChangeCommand();
        void Do() override;
        void Undo() override;

    private :
        Grafkit::Animation::TrackRef m_track;
        size_t m_id;
        Grafkit::Animation::ChannelRef m_oldChannel;
        Grafkit::Animation::ChannelRef m_newChannel;

        CurvePointEditor *m_controller;
	};


}
