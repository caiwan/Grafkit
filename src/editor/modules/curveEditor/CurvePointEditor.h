#pragma once 

#include "utils/Command.h"
#include "utils/ViewModule.h"

#include "models/Curve.h"

namespace Idogep {

    class CurvePointItem;
    class CurveCursor;
    class CurveEditorView;

    class TimelineArea;

    typedef std::vector<CurvePointItem*> CurvePointList;

    class CurvePointEditor : public Controller, public EmitsCommandRole
    {
        friend class CurvePointItem;

    public:
        explicit CurvePointEditor(const Ref<Controller>& parent);

        Grafkit::Animation::ChannelRef GetChannel() const { return m_channel; }
        void SetChannel(Grafkit::Animation::ChannelRef& channel) { m_channel = channel; Rebuild(); }

        void Recalculate(TimelineArea* const area) const;

        void UpdateKey(const Grafkit::Animation::ChannelRef& channel, size_t id, const Grafkit::Animation::Key& key);

        void Initialize() override;

        void Rebuild();

        void HidePoints();
        void ShowPoints();

        // Events
        void StartEditEvent(CurvePointItem *item);
        void CommitEditEvent(CurvePointItem *item);

        void CommitAddPointEvent(float key, float value);
        void CommitRemovePointEvent(float key, float value);

        void EditKeyEvent(CurvePointItem *item);

        bool HasCurvePoints() const { return !m_points.empty(); }
        size_t GetPointCount() const { return m_points.size(); }
        CurvePointItem* GetPoint(size_t id) { return m_points[id]; }

    private:
        Grafkit::Animation::Key EditKey(size_t index, Grafkit::Animation::Key key) const;

        Grafkit::Animation::ChannelRef m_channel;
        CurvePointList m_points;
    };

    class CursorManager
    {
        // ... 
    };

}
