#pragma once 

#include <qobject.h>

#include "animation/animation.h"
#include "utils/Event.h"
#include "utils/Command.h"

namespace Idogep {

	class CurvePointItem;
	class CurveEditorScene;

	class CurveCursor;

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

	class ManageCurveRole : public ManageCurveAudiogramRole, public ManageCursorRole, public EmitsCommandRole
	{
		friend class CurvePointItem;
		friend class CurveEditorScene;

		//Q_OBJECT
	public:
		ManageCurveRole();

#if 0
		QList<CurvePointItem*>* GetCurvePoints() { return m_curve; }
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
		QList<CurvePointItem*>* m_curve;
		Ref<Grafkit::Animation::Channel> m_channel;
#endif
	    Grafkit::Animation::Key m_originalKey;
		Grafkit::Animation::Key m_modifiedKey;
	};

#endif

}