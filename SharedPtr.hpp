#ifndef SHARED_PTR
#define SHARED_PTR

#include<iostream>
#include <mutex> 

namespace cs540 {

std::mutex myMutex;

class ReferenceCount{
public:
	int refCount; 
	ReferenceCount() : refCount(0) {}
	virtual ~ReferenceCount() {}
};

template <typename T>
class CriticalSection : public ReferenceCount
{
	public:
	T *obj;
	CriticalSection(T* tempObj) : obj(tempObj){}
	~CriticalSection(){
		delete obj;
		obj=nullptr;
	}
};

template <typename T> 
class SharedPtr{
	public:
	T* myPtr;
	ReferenceCount* objShared;

	SharedPtr() : objShared(nullptr),myPtr(nullptr)  {}

	template <typename U> 
	explicit SharedPtr(U *obj) : myPtr(obj),objShared(new CriticalSection<U>(obj)) {
		if(objShared!=nullptr){
			myMutex.lock();
			objShared->refCount=1;
			myMutex.unlock();
		}
	}

	SharedPtr(const SharedPtr &p) : objShared(p.objShared),myPtr(p.myPtr) {
		if(objShared!=nullptr){
			myMutex.lock();
			objShared->refCount++;
			myMutex.unlock();
		}
	}
	template <typename U>
	SharedPtr(const SharedPtr<U>& p) : objShared(p.objShared), myPtr(p.myPtr) {
		if(objShared!=nullptr){
			myMutex.lock();
			objShared->refCount++;
			myMutex.unlock();
		}
    }
	
	SharedPtr(SharedPtr &&p) : objShared(p.objShared), myPtr(p.myPtr) {
		if(p!=nullptr) {
			//delete p.objShared; 
			p.objShared=nullptr;
			p.myPtr=nullptr;
		}
    }
    template <typename U> 
    SharedPtr(SharedPtr<U> &&p) : objShared(p.objShared), myPtr(p.myPtr) {
		if(p!=nullptr) {
			//delete p.objShared;
			p.objShared=nullptr;
			p.myPtr=nullptr;
		}
	}
	
	SharedPtr &operator=(const SharedPtr &p) {
		if(p!=nullptr) {
			if (objShared == p.objShared) {
				return *this;
			}else{
				if(objShared!=nullptr){
					myMutex.lock();
					int tempCount=--objShared->refCount;
					myMutex.unlock();
					if( tempCount == 0) {   
						if(objShared!=nullptr)
							delete objShared;
					}
				}
				objShared = p.objShared;
				myMutex.lock();
				objShared->refCount++;
				myMutex.unlock();
			}
		}
		return *this;
    }
	template <typename U> 
	SharedPtr<T> &operator=(const SharedPtr<U> &p) {
		if(p!=nullptr) {
			if (objShared == p.objShared) {
				return *this;
			}else{
				if(objShared!=nullptr) {
					myMutex.lock();
					int tempCount=--objShared->refCount;
					myMutex.unlock();
					if( tempCount == 0) {   
						if(objShared!=nullptr)
							delete objShared;
					}
				}
				objShared = p.objShared;
				myMutex.lock();
				objShared->refCount++;
				myMutex.unlock();
			}
		}
		return *this;
    }
	
	SharedPtr &operator=(SharedPtr &&p) {
		if(p!=nullptr) {
			if (objShared == p.objShared) {
				return *this;
			}else{
				if(objShared!=nullptr) {
					myMutex.lock();
					int tempCount=--objShared->refCount;
					myMutex.unlock();
					if( tempCount == 0) {   
						if(objShared!=nullptr)
							delete objShared;
					}
				} 
				objShared = p.objShared;
			}
			p.objShared=nullptr;
			p.myPtr=nullptr;
		}
		return *this;
    }
    template <typename U> 
    SharedPtr &operator=(SharedPtr<U> &&p) {
        if(p!=nullptr) {
			if (objShared == p.objShared) {
				return *this;
			}else{
				if(objShared!=nullptr) {
					myMutex.lock();
					int tempCount=--objShared->refCount;
					myMutex.unlock();
					if( tempCount == 0) {   
						if(objShared!=nullptr)
							delete objShared;
					}
				}
				objShared = p.objShared;
			}
			p.objShared=nullptr;
			p.myPtr=nullptr;
		}
        return *this;
    }
	
	~SharedPtr() {
		if(objShared!=nullptr) {
			myMutex.lock();
			int tempCount=--objShared->refCount;
			myMutex.unlock();
			if( tempCount == 0) {
				delete objShared;
				objShared=nullptr;
				myPtr=nullptr;
			}
		}
	}
	
	void reset() {
		if(objShared!=nullptr) {
			myMutex.lock();
			int tempCount=--objShared->refCount;
			myMutex.unlock();
			if(tempCount == 0) {
				delete objShared;
			}
			objShared=nullptr;
			myPtr=nullptr;
		}
	}

	template <typename U> 
	void reset(U *p) {
		if(objShared!=nullptr) {
			myMutex.lock();
			int tempCount=--objShared->refCount;
			myMutex.unlock();
			if(tempCount==0) {
				delete objShared;
			}
			objShared=nullptr;
		}
		myPtr=p;
		objShared= new CriticalSection<U>(p);
		myMutex.lock();
		objShared->refCount++;
		myMutex.unlock();
	}
	
	T *get() const {
		return myPtr;
    }
	
	T &operator*() const {
		return *myPtr;
    }

    T *operator->() const {
		return myPtr;
    }
	
	explicit operator bool() const {
        if(myPtr!=nullptr) {
            return true;
        }
		return false;
    }
	
	template <typename T1, typename T2> 
	friend bool operator==(const SharedPtr<T1> &, const SharedPtr<T2> &);
	template <typename T1> 
	friend bool operator==(const SharedPtr<T1> &, std::nullptr_t); 
	template <typename T1> 
	friend bool operator==(std::nullptr_t, const SharedPtr<T1> &);
	
	template <typename T1, typename T2> 
	friend bool operator!=(const SharedPtr<T1>&, const SharedPtr<T2> &);
	template <typename T1> 
	friend bool operator!=(const SharedPtr<T1> &, std::nullptr_t); 
	template <typename T1> 
	friend bool operator!=(std::nullptr_t, const SharedPtr<T1> &);
	
	template <typename T1, typename U1> 
	friend SharedPtr<T1> static_pointer_cast(const SharedPtr<U1> &sp);
	template <typename T1, typename U1> 
	friend SharedPtr<T1> dynamic_pointer_cast(const SharedPtr<U1> &sp);
	
};

template <typename T1, typename T2>
bool operator==(const SharedPtr<T1>& p1, const SharedPtr<T2>& p2){
	if(p1.myPtr==p2.myPtr){
		return true;
	}
	if(p1==nullptr && p2==nullptr){
		return true;
	}
	return false;
}
template <typename T1>
bool operator==(const SharedPtr<T1>& p1, std::nullptr_t p2){
	if(p1.objShared == p2){
		return true;
	}
	return false;
}
template <typename T1>
bool operator==(std::nullptr_t p1, const SharedPtr<T1>& p2){
	if(p1 == p2){
		return true;
	}
	return false;
}

template <typename T1, typename T2>
bool operator!=(const SharedPtr<T1>& p1, const SharedPtr<T2>& p2){
	if (p1 == p2) {
		return false;
	}
	return true;
}
template <typename T1>
bool operator!=(const SharedPtr<T1>& p1, std::nullptr_t p2){
	if (p1 == p2) {
		return false;
	}
	return true;
}
template <typename T1>
bool operator!=(std::nullptr_t p1, const SharedPtr<T1>& p2){
	if (p1 == p2) {
		return false;
	}
	return true;
}
 
template <typename T1, typename U1>
SharedPtr<T1> static_pointer_cast(const SharedPtr<U1>& sp){
	SharedPtr<T1> castPtr;
	castPtr.objShared=sp.objShared;
	myMutex.lock();
	++castPtr.objShared->refCount;
	myMutex.unlock();

	castPtr.myPtr=static_cast<T1 *>(sp.myPtr);
	return castPtr;
}
template <typename T1, typename U1>
SharedPtr<T1> dynamic_pointer_cast(const SharedPtr<U1> &sp){
	SharedPtr<T1> castPtr;
	castPtr.objShared=sp.objShared;
	myMutex.lock();
	++castPtr.objShared->refCount;
	myMutex.unlock();

	castPtr.myPtr=dynamic_cast<T1 *>(sp.myPtr);
	return castPtr;
}
}//namespace
#endif