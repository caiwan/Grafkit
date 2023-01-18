#pragma once 

#include "animation/animation.h"

#include "utils/Command.h"
#include "utils/ViewModule.h"

namespace Idogep {

    class CurvePointItem;
    class CurveCursor;
    class CurveEditorView;

    class TimelineArea;

    typedef Ref<Grafkit::Animation::Channel> ChannelRef;
    typedef Grafkit::Animation::Key AnimationKey;

    typedef std::vector<Ref<CurvePointItem>> CurvePointList;
    typedef CurvePointList::const_iterator CurvePointIterator;

    class CurvePointEditor : public Controller, public EmitsCommandRole
    {
        friend class CurvePointItem;

    public:
        explicit CurvePointEditor(const Ref<Controller>& parent);

        ChannelRef GetChannel() const { return m_channel; }
        void SetChannel(ChannelRef& channel) { m_channel = channel; Rebuild(); }

        std::pair<CurvePointIterator, CurvePointIterator> GetCurvePointIterators() const { return std::make_pair(m_curvePoints.cbegin(), m_curvePoints.cend()); }

        void AddCurveToScene(CurveEditorView* parent) const;
        void Recalculate(TimelineArea* const area) const;

        //void RefreshPoint(ChannelRef channel, size_t id, AnimationKey key);

        void Initialize() override;

        void Rebuild();

        // Events
        void StartEditEvent(CurvePointItem *item);
        void CommitEditEvent(CurvePointItem *item);

        void CommitAddPointEvent(float key, float value);
        void CommitRemovePointEvent(float key, float value);

        void EditKeyEvent(CurvePointItem *item);

    private:
        CurvePointList m_curvePoints;
        ChannelRef m_channel;
    };

    class CursorManager
    {
        // ... 
    };

}
