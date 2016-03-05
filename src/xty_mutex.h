
#ifndef XTY_MUTEX_H
#define XTY_MUTEX_H

class xtyMutex : public xtyLock
{
public:
	// create a new "pthread_mutex_t", need to delete	
	xtyMutex();
	
	// inherit "pthread_mutex_t*" from outside, no need to delete
	explicit xtyMutex(pthread_mutex_t*);
	
	// deconstructor
	virtual ~xtyMutex();
	
private:
	// does not allow copying
	xtyMutex(const xtyMutex&);
	xtyMutex& operator = (const xtyMutex&);

public:
	// get raw ptr 'pthread_mutex_t*'
	inline pthread_mutex_t* GetRawMutexPtr() const { return m_pMutex; }
	
	virtual int Lock();
	virtual int TryLock();
	virtual void UnLock(); 

private:
	pthread_mutex_t* m_pMutex;
	const bool m_NeedToDelete;
};

#endif

