#pragma once

#include "utils/Command.h"
#include "utils/ViewModule.h"

#include "models/Curve.h"

namespace Idogep
{
    class CurvePointItem;
    class CurveCursor;
    class CurveEditorView;

    class TimelineArea;

    class PointEditorView;

    typedef std::vector<CurvePointItem*> CurvePointList;

    typedef Event<const size_t&, const Grafkit::Animation::Key &> KeyEditEvent;

    class CurvePointEditor : public Controller, public EmitsCommandRole
    {
        friend class CurvePointItem;

    public:
        explicit CurvePointEditor();

        Grafkit::Animation::ChannelRef GetChannel() const { return m_channel; }

        void SetChannel(Grafkit::Animation::TrackRef& track, const size_t &channelId, Grafkit::Animation::ChannelRef& channel)
        {
            m_track = track;
            m_channelId = channelId;
            m_channel = channel;
            Rebuild(); //... Folyt kov.
        }

        void Initialize(Grafkit::IResourceManager* const& resourceManager) override;

        void Recalculate(TimelineArea* const area) const;

        void UpdateKey(const Grafkit::Animation::ChannelRef& channel, size_t index, const Grafkit::Animation::Key& key);
        void UpdateChannel(const Grafkit::Animation::TrackRef& track, size_t channelId, const Grafkit::Animation::ChannelRef& channel);
        
        void Rebuild();

        void HidePoints();
        void ShowPoints();

        // Events
        void StartEditEvent(const size_t& index, const Grafkit::Animation::Key& key);
        void CommitEditEvent(const size_t& index, const Grafkit::Animation::Key& key);

        void CommitAddPointEvent(const float& key, const float& value);
        void CommitRemovePointEvent(const size_t& index);

        void EditKeyEvent(const size_t& index, const Grafkit::Animation::Key& key);

        bool HasCurvePoints() const { return !m_points.empty(); }
        size_t GetPointCount() const { return m_points.size(); }
        CurvePointItem* GetPoint(size_t index) { return m_points[index]; }

        void PointSelectedEvent(size_t index);
        void PointDeSelectedEvent();

        bool GetAndClearIsCurveChangedFlag();

    private:
        Grafkit::Animation::Key EditKey(size_t index, Grafkit::Animation::Key key) const;

        Grafkit::Animation::ChannelRef m_channel;
        Grafkit::Animation::TrackRef m_track;

        size_t m_channelId;

        CurvePointList m_points;

        bool m_isCurveChangedFlag;

        PointEditorView* m_myView;
    };

    // --- 

    class CursorManager
    {
        // ... 
    };
}
