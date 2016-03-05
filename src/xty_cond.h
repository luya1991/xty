
#ifndef XTY_COND_H
#define XTY_COND_H

class xtyCond
{
public:
	// create a new "pthread_cond_t", need to delete
	xtyCond();

	// inherit "pthread_cond_t*" from outside, no need to delete
	explicit xtyCond(pthread_cond_t*);

	// deconstructor
	~xtyCond();

private:
	// does not allow copying
	xtyCond(const xtyCond&);
	xtyCond& operator = (const xtyCond&);

public:
	int Wait(xtyMutex* pMutex);
	int WakeUp();
	int WakeUpAll();

public:
#ifdef DEBUG
	friend class xtyCondTest;
#endif

private:
	pthread_cond_t* m_pCond;
	const bool m_NeedToDelete;
};

#endif

