#pragma once

#include "windows.h"

//	Clas to handle long-lived memory allocations, principally intended
//	for instances of entity classes like CvUnit, CvCity, etc.
class CvAllocator
{
public:
	CvAllocator();
	virtual ~CvAllocator();

	void clearAll();

	void*	allocate(size_t size);
	void	deAllocate(void* ptr);

private:
	HANDLE	m_heap;
};
