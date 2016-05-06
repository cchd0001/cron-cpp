#include "UnitTest++/UnitTestPP.h"
#include <iostream>
int main(int, char const *[])
{
   UnitTest::RunAllTests();
#ifdef MSC_VER // Hi , hold on until I let you go .
   char c;
   std::cin >> c;
#endif
}

                    
