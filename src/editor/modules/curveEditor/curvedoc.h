#pragma once 

#include <qobject.h>

#include "animation/animation.h"
#include "utils/Event.h"
#include "utils/Command.h"

namespace Idogep {

	class CurvePointItem;
	class CurveCursor;
	class CurveSceneView;

	class TimelineArea;

	namespace Roles
	{
		class EditCurveRole : public EmitsCommandRole
		{

		public:
			void StartEdit(CurvePointItem *item);
			void CommitEdit(CurvePointItem *item);

			void CommitAddPoint(float key, float value);
			void CommitRemovePoint(float key, float value);

		private:
			Grafkit::Animation::Key m_originalKey;
		};
	}

	class CurveManager : public Roles::EditCurveRole
	{
		friend class CurvePointItem;

	public:
		QList<CurvePointItem*> const * GetCurvePoints() const { return m_curvePoints; }
		Ref<Grafkit::Animation::Channel> GetChannel() const { return m_channel; }

		void SetChannel(Ref<Grafkit::Animation::Channel>& channel) { m_channel = channel; Rebuild(); }

		void Recalculate(TimelineArea* drawingSurfaceArea);
		void AddCurveToScene(CurveSceneView* parent);

	protected:
		void Rebuild();

	private:
		QList<CurvePointItem*>* m_curvePoints;
		Ref<Grafkit::Animation::Channel> m_channel;
	};

    class CursorManager
    {
        // ... 
    };

#if 0

	// --- 

	class ManageCursorRole {
		// TODO:
		//CurveCursor *m_crsrPlayback;
		//CurveCursor *m_crsrHover;
		//CurveCursor *m_crsrMarkerStart;
		//CurveCursor *m_crsrMarkerEnd;
	};

	// --- 
	class ManageCurveRole : public EmitsCommandRole
	{
		friend class CurveEditorScene;

	public:
		ManageCurveRole();

#if 0
		QList<CurvePointItem*>* GetCurvePoints() { return m_curvePoints; }
		Ref<Grafkit::Animation::Channel> GetChannel() { return m_channel; }
#endif 
		void SetChannel(Ref<Grafkit::Animation::Channel>& channel);

	protected:
		void Recalculate();
		void AddCurveToScene(CurveEditorScene* parent);

		virtual void RefreshView(bool force = false) = 0;

	protected:
		void MovePoint(CurvePointItem *item);
		void MoveTangent(CurvePointItem *item);
		void StartEdit(CurvePointItem *item);
		void CommitEdit(CurvePointItem *item);

		void CommitAddPoint(float key, float value);
		void CommitRemovePoint(float key, float value);

	private:
#if 0
		QList<CurvePointItem*>* m_curvePoints;
		Ref<Grafkit::Animation::Channel> m_channel;
#endif
		Grafkit::Animation::Key m_originalKey;
		Grafkit::Animation::Key m_modifiedKey;
	};

#endif

}
