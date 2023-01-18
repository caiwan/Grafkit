#include "observer.h"

using namespace Grafkit;

// ======================================================================================================

Grafkit::Observer::Observer()
{
}

Grafkit::Observer::~Observer()
{
	for (obvs_it_t it = m_observables.begin(); it != m_observables.end(); it++) {
		(*it)->DeleteObserver(this);
	}
}

// ======================================================================================================


Grafkit::Observable::Observable() : m_mutex(), m_is_changed(0), m_observers()
{
}

Grafkit::Observable::~Observable()
{
	DeleteObservers();
}

void Grafkit::Observable::AddObserver(Observer * obj)
{
	m_mutex.lock();
	m_observers.insert(obj);
	obj->AddObservable(this);
	m_mutex.unlock();
}

void Grafkit::Observable::DeleteObserver(Observer * obj)
{
	m_mutex.lock();
	m_observers.erase(obj);
	obj->RemoveObservable(this);
	m_mutex.unlock();
}

void Grafkit::Observable::NotifyObservers(void const* data)
{
	if (!this->HasChanged())
		return;
	m_mutex.lock();
	for (obs_it_t it = m_observers.begin(); it != m_observers.end(); it++) {
		(*it)->Update(this, data);
	}
	m_mutex.unlock();
}

void Grafkit::Observable::DeleteObservers()
{
	m_mutex.lock();
	for (obs_it_t it = m_observers.begin(); it != m_observers.end(); it++) {
		(*it)->RemoveObservable(this);
	}
	m_observers.clear();
	m_mutex.unlock();
}

// ------------------------------------------------------------------------------------------------------

