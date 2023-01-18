#pragma once 

#include <qobject.h>

#include "animation/animation.h"
#include "utils/Command.h"
#include "utils/ViewModule.h"

namespace Idogep {

	class CurvePointItem;
	class CurveCursor;
	class CurveSceneView;

	class TimelineArea;

	typedef std::vector<Ref<CurvePointItem>> CurvePointList;
	typedef CurvePointList::const_iterator CurvePointIterator;

	namespace Roles
	{
		class EditCurveRole : public EmitsCommandRole
		{

		public:
			virtual void StartEdit(CurvePointItem *item);
			virtual void CommitEdit(CurvePointItem *item);

			virtual void CommitAddPoint(float key, float value);
			virtual void CommitRemovePoint(float key, float value);

			virtual void EditKey(CurvePointItem *item);

			Ref<Grafkit::Animation::Channel> GetChannel() const { return m_channel; }
			void SetChannel(Ref<Grafkit::Animation::Channel>& channel) { m_channel = channel; Rebuild(); }

		protected:
			Ref<Grafkit::Animation::Channel> m_channel;
			virtual void Rebuild() =0;

		};
	}

	class CurveManager : public Roles::EditCurveRole, public Module
	{
		friend class CurvePointItem;

	public:
	    explicit CurveManager(const Ref<Module>& parent);

		std::pair<CurvePointIterator, CurvePointIterator> GetCurvePointIterators() const { return std::make_pair(m_curvePoints.cbegin(), m_curvePoints.cend()); }

		void AddCurveToScene(CurveSceneView* parent) const;
		void Recalculate(TimelineArea* const area) const;

	    void Initialize() override;

	protected:
		void Rebuild() override;

	private:
		std::vector<Ref<CurvePointItem>> m_curvePoints;
	};

	class CursorManager
	{
		// ... 
	};

}
