
#ifndef XTY_EVENT_H
#define XTY_EVENT_H

/*
 * 2015-06-29
 * interface-consistency for 'xtyEvent' is totally a daydream!
 *
 * cannot merge thread-event & process-event into 'xtyEvent' as
 * one generic base class, because the lock-mechanism is quite
 * different in these 2 situations:
 * 
 * (1) in thread-event situation
 * wait()-ing on a condition variable will unlock 'pthread_mutex_t'
 * first, which will be re-locked on the returning of wait().
 *
 * thread-event situation must go as following:
 * 
 *	lock();                            -----
 *                                           ^
 *	while(something)                     |
 *		keep waiting(&lock);         |
 *	                                     | lock
 *      operating shared data;               | range
 *                                           |
 *	wake_up();                           |
 *                                           v
 *	unlock();                          -----
 *
 * (2) in process-event situation
 * wait()-ing on a semaphore will do nothing on 'spin_lock', which
 * means the lock-range must be only on the shared-data modifying
 * area, otherwise it will cause dead-lock problem.
 *
 * process-event situation must go as following:
 *
 *      while(something)
 *		keep waiting(nothing);
 *	
 *	lock();                            -----
 *                                           ^
 *	operating shared data;               | lock range
 *                                           v 
 *	unlock();                          -----
 *
 *	wake_up();
 */

class xtyEvent
{
public:
	xtyEvent() : m_Events(0)
	{/* does nothing but need a definition */}
	
	virtual ~xtyEvent()
	{/* does nothing but need a definition */}
	
private:
	xtyEvent(const xtyEvent&);
	xtyEvent& operator = (const xtyEvent&);

public:
	virtual int Produce() = 0;
	virtual int Consume() = 0;

protected:
	// > 0: available resource count, = 0: nothing available left, < 0: unknown
	unsigned int m_Events;
};

class xtyThreadEvent : public xtyEvent
{
public:
	xtyThreadEvent()
	{/* does nothing but need a definition */}
	
	virtual ~xtyThreadEvent()
	{/* does nothing but need a definition */}
	
private:
	xtyThreadEvent(const xtyThreadEvent&);
	xtyThreadEvent& operator = (const xtyThreadEvent&);

public:
	virtual int Produce();
	virtual int Consume();

public:
#ifdef DEBUG
	friend class xtyEventTest;
#endif

private:
	// using 'obj' instead of 'ptr' to avoid 'double-free' in concurrency conditions
	xtyMutex m_Lock;
	xtyCond m_Notifier;
};

class xtyProcessEvent : public xtyEvent
{
public:
	xtyProcessEvent()
	{/* does nothing but need a definition */}
	
	virtual ~xtyProcessEvent()
	{/* does nothing but need a definition */}
	
private:
	xtyProcessEvent(const xtyProcessEvent&);
	xtyProcessEvent& operator = (const xtyProcessEvent&);

public:
	virtual int Produce();
	virtual int Consume();

public:
#ifdef DEBUG
	friend class xtyEventTest;
#endif

private:
	// using 'obj' instead of 'ptr' to avoid 'double-free' in concurrency conditions
	xtySpinLock m_Lock;
	xtySemaphore m_Notifier;
};

#endif

