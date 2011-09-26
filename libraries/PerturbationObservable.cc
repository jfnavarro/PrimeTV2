#include <iostream>

#include "PerturbationObservable.hh"

// Author: Joel Sjögren, © the MCMC-club, SBC, all rights reserved

namespace beep
{
  using namespace std;

  PerturbationObservable::PerturbationObservable() :
    m_notifyPertObservers(true),
    m_pertObservers()
  {
  }

	
  PerturbationObservable::~PerturbationObservable()
  {
  }


  void PerturbationObservable::addPertObserver(PerturbationObserver* observer) const
  {
    m_pertObservers.insert(observer);
  }


  void PerturbationObservable::removePertObserver(PerturbationObserver* observer) const
  {
    m_pertObservers.erase(observer);
  }


  void PerturbationObservable::clearPertObservers() const
  {
    m_pertObservers.clear();
  }


  bool PerturbationObservable::isPertObserver(PerturbationObserver* observer) const
  {
    return (m_pertObservers.find(observer) != m_pertObservers.end());
  }


  bool PerturbationObservable::getPertNotificationStatus() const
  {
    return m_notifyPertObservers;
  }


  bool PerturbationObservable::setPertNotificationStatus(bool doNotify) const
  {
    bool temp = m_notifyPertObservers;
    m_notifyPertObservers = doNotify;
    return temp;
  }


  bool 
  PerturbationObservable::notifyPertObservers(const PerturbationEvent* event) const
  {
    if(m_notifyPertObservers)
      {
	set<PerturbationObserver*>::iterator it = m_pertObservers.begin();
	for (; it != m_pertObservers.end(); ++it)
	  {
	    (*it)->perturbationUpdate(this, event);
	  }
      }
    return m_notifyPertObservers;
  }

}  // end namespace beep

