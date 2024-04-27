//
// Created by Junhao Wang (@forkercat) on 4/1/24.
//

#include "core/core.h"

using namespace mapo;

class Test
{
public:
	Test() = default;
};

int main()
{
	MP_PRINT("123123!");

	StdAllocator allocator;

	int* pValue = MP_NEW(int, allocator);
	MP_DELETE(pValue, allocator);

	Test* tValue = MP_NEW(Test, allocator);
	MP_DELETE(tValue, allocator);

	int* p = MP_NEW_ARRAY(int[3], allocator);
	MP_DELETE_ARRAY(p, allocator);

	Test* pt = MP_NEW_ARRAY(Test[3], allocator);
	MP_DELETE_ARRAY(pt, allocator);
}
