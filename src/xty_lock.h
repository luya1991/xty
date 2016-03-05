
#ifndef XTY_LOCK_H
#define XTY_LOCK_H

class xtyLock
{
public:
	xtyLock()
	{/* does nothing but need a definition */}
	
	virtual ~xtyLock()
	{/* does nothing but need a definition */}

private:
	// does not allow copying
	xtyLock(const xtyLock&);
	xtyLock& operator = (const xtyLock&);
	
public:
	// pure virtual as a interface-class
	virtual int Lock() = 0;
	virtual int TryLock() = 0;
	virtual void UnLock() = 0;
};

#endif

