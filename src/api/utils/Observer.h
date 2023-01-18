#pragma once

/**
http://python-3-patterns-idioms-test.readthedocs.org/en/latest/Observer.html
*/

#include <set>
#include <mutex>

namespace Grafkit {

	class Observable;
	class Observer;

	class Observer {
		friend class Observable;
	public:
		Observer();
		virtual ~Observer();

	protected:
		virtual void Update(Observable* source, void const * data) = 0;

		void AddObservable(Observable* obj) { m_observables.insert(obj); }
		void RemoveObservable(Observable* obj) { m_observables.erase(obj); }

	private:
		typedef std::set<Observable*>::iterator obvs_it_t;
		std::set<Observable*> m_observables;
	};

	class Observable {
	public:
		Observable();
		virtual ~Observable();

		void AddObserver(Observer* obj);
		void DeleteObserver(Observer* obj);
		void NotifyObservers(void const * data);
		void DeleteObservers();

		void SetChanged() { m_is_changed = 1; }
		void ClearChanged() { m_is_changed = 0; }
		int HasChanged() { return m_is_changed; }

	private:
		typedef std::set<Observer*>::iterator obs_it_t;
		std::set<Observer*> m_observers;

		int m_is_changed;

		std::mutex m_mutex;
	};
	
	
}
