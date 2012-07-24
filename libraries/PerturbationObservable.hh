#ifndef PERTURBATIONOBSERVER_HH
#define PERTURBATIONOBSERVER_HH

#include <set>

//namespace beep
//{

  // Forward declarations.
  class PerturbationEvent;
  class PerturbationObserver;

  /**
   * Base class for a holder of "perturbable parameters" which may need
   * to notify dependent objects when a parameter has changed. The listeners,
   * in turn, implement the PerturbationObserver interface and register
   * themselves as subscribers by invoking addPertObserver (passing themselves
   * along). They will then be notified by a call to their update method when a
   * parameter change has occurred.
   *
   * It is up to sub-classes themselves to decide if an internal change should
   * trigger notifications, or if only MCMC classes should do that externally.
   *
   * One has the possibility of turning off all notifications with a flag,
   * meaning that even if the notification method is invoked, no listeners will
   * be notified. Use this e.g. to make multiple updates but only a single
   * notification in the end.
   * Author: Joel Sjöstrand, SBC, the MCMC-club, SBC, all rights reserved.
   */
  class PerturbationObservable
  {
  public:
	
    /**
     * Constructor.
     */
    PerturbationObservable();
	
    /**
     * Destructor.
     */
    virtual ~PerturbationObservable();
	
    /**
     * Adds a listener subscribing to this observable.
     * @param observer the listener to add.
     */
    virtual void addPertObserver(PerturbationObserver* observer) const;
	
    /**
     * Removes a listener subscribing to this observable.
     * @param observer the listener to remove.
     */
    virtual void removePertObserver(PerturbationObserver* observer) const;

    /**
     * Removes all subscribing listeners.
     */
    virtual void clearPertObservers() const;
	
    /**
     * Returns true if the specified listener is among the subscribing objects.
     * @param observer the listener.
     * @return true if it is indeed a registered subscriber.
     */
    virtual bool isPertObserver(PerturbationObserver* observer) const;
	
    /**
     * Returns the flag stating if registered listeners are indeed notified
     * when the notify method is invoked.
     * @return true if listeners are notified.
     */
    virtual bool getPertNotificationStatus() const;
	
    /**
     * Sets the flag stating if registered listeners should indeed be notified when
     * the notify method is invoked. Typically used to temporarily turn the
     * notifications off when multiple changes are made to the object and it
     * suffices with a single notify call in the end. Note that after setting
     * the flag back to true, there is no automatic notification. Returns the
     * previous value.
     * @param doNotify false to turn notification off.
     * @return the old value.
     */
    virtual bool setPertNotificationStatus(bool doNotify) const;

    /**
     * Notifies all registered listeners by calling their corresponding update
     * methods. Info about the changes can be specified in an event object. At the
     * moment, the event object may (and is expected to) be highly volatile, meaning
     * that it might be destroyed right after the call. The object invoking this method
     * keeps ownership of the event.
     * @param event information about the changes. Highly volatile.
     * @return true if the message was indeed sent to the subscribers.
     */
    virtual bool notifyPertObservers(const PerturbationEvent* event) const;
	
  protected:
	
    /** Flag stating if registered listeners should indeed be notified. */
    mutable bool m_notifyPertObservers;

    /** Set of registered listeners. */
    mutable std::set<PerturbationObserver*> m_pertObservers;
  };


  /**
   * Interface for listeners of perturbation events. Classes
   * or structs dependending on perturbable parameters can extend this
   * abstract class, provided that the parameter holder implements
   * PertubationObservable. See also PerturbationEvent.
   */
  class PerturbationObserver
  {
  public:
	
    /**
     * Destructor.
     */
    virtual ~PerturbationObserver() {};
	
    /**
     * Method through which messages of perturbation changes are received.
     * Contains the address of the sender, and potentially more detailed information
     * in an event object. The sender keeps ownership of the event.
     * Note: the event object may be highly volatile, and may thus be
     * destroyed after the method has finished. Use the information wisely.
     * @param sender the address of the sender.
     * @param event info about the changes. Highly volatile.
     */
    virtual void perturbationUpdate(const PerturbationObservable* sender,
				    const PerturbationEvent* event) = 0;


  };


  /**
   * Base class for perturbation events, i.e. the information sent in the
   * callbacks. May be extended to provide more details. See
   * PerturbationObservable and PerturbationObserver for more info.
   */
  class PerturbationEvent
  {
  public:
	
    /** General perturbation types. */
    enum Type
      {
	PERTURBATION,
	RESTORATION
      };
	
  protected:
	
    /** Perturbation type of the event. */
    Type m_type;
	
  public:
	
    /**
     * Constructor.
     * @param type the general perturbation type.
     */
    PerturbationEvent(Type type) :
      m_type(type)
    {
    };
	
    /**
     * Destructor.
     */
    virtual ~PerturbationEvent()
    {
    };
	
    /**
     * Returns the general perturbation type.
     * @return the perturbation type.
     */
    virtual Type getType() const
    {
      return m_type;
    };
	
    /**
     * Returns a string about the event.
     * @return an info string.
     */
    virtual std::string print() const
    {
      return (m_type == PERTURBATION ?
	      "PerturbationEvent: PERTURBATION" : "PerturbationEvent: RESTORATION");
    }
  };

//} // end namespace beep

#endif /*PERTURBATIONOBSERVER_HH*/

