
#ifndef XTY_SPINLOCK_H
#define XTY_SPINLOCK_H

class xtySpinLock : public xtyLock
{
public:
	xtySpinLock();
	virtual ~xtySpinLock();
	
private:
	// does not allow copying
	xtySpinLock(const xtySpinLock&);
	xtySpinLock& operator = (const xtySpinLock&);

public:
	virtual int Lock();
	virtual int TryLock();
	virtual void UnLock();

private:
	volatile unsigned int m_Atomic;
};

#endif

