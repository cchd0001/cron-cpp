#include "StringFunc.h"
#include "UnitTest++/UnitTestPP.h"

SUITE(CronTest) {
    template <class T>
        struct VectorSame{
            std::vector<T> data;

            bool operator == (const std::vector<T>& a ) {
                if(data.size() != a.size() ){
                    return false ;
                } else {
                    for(size_t i = 0 ; i < data.size() ; i++ ){
                        if( data[i] != a[i] ) {
                            return false ;
                        }
                    }
                }
                return true;
            }
        };

    typedef VectorSame<std::string> FiledResult;

    struct FiledsTest {
        FiledResult result ;
        std::string str ;
    };

    TEST(TrimLeft){
        CHECK_EQUAL("Hello " , TrimLeft(std::string(" Hello ")));
        CHECK_EQUAL("Hello " , TrimLeft(std::string("Hello ")));
        CHECK_EQUAL("Hello World" , TrimLeft(std::string(" Hello World")));
        CHECK_EQUAL("" , TrimLeft(std::string(" ")));

        CHECK(L"Hello " == TrimLeft(std::wstring(L" Hello ")));
        CHECK(L"Hello " == TrimLeft(std::wstring(L"Hello ")));
        CHECK(L"" == TrimLeft(std::wstring(L"  ")));
        CHECK(L"Hello World" == TrimLeft(std::wstring(L" Hello World")));
    }

    TEST(TrimRight){
        CHECK_EQUAL(" Hello" , TrimRight(std::string(" Hello ")));
        CHECK_EQUAL("Hello" , TrimRight(std::string("Hello ")));
        CHECK_EQUAL("Hello World" , TrimRight(std::string("Hello World      ")));
        CHECK_EQUAL("" , TrimRight(std::string("    ")));

        CHECK(L" Hello" == TrimRight(std::wstring(L" Hello  ")));
        CHECK(L"Hello" == TrimRight(std::wstring(L"Hello ")));
        CHECK(L"" == TrimRight(std::wstring(L"  ")));
        CHECK(L"Hello World" == TrimRight(std::wstring(L"Hello World        ")));
    }

    TEST(Fields){

        FiledResult t{{"Hello" , "World"}};

        FiledsTest test_fields[] = { 
            {
                {{"Hello" , "World"}},
                "Hello World", 
            },
            {
                {{"Hello" , "World"}},
                " Hello World ", 
            },
            {
                {{"Hello" , "World" , "*" , "*"}},
                " Hello World  * *                                                                                                                                                                         ", 
            },
            {
                {},
                "  "
            }
        };

        for ( size_t i=0 ; i< sizeof(test_fields) / sizeof(test_fields[0]) ; i ++ ) {
            CHECK( test_fields[i].result ==  Fields(test_fields[i].str))  ;
        }
    }

    TEST(SplitString){
        FiledsTest test_split[] = { 
            {
                {{"Hello" , "World"}},
                "Hello , World", 
            },
            {
                {{"Hello" , "World"}},
                " Hello , World ", 
            },
            {
                {{"Hello" , "World" , "*" , "*"}},
                " Hello , World,  *, * ", 
            },
            {
                {},
                "   "
            },
            {
                {},
                "  , ,, ,"
            }
        };
        for ( size_t i=0 ; i< sizeof(test_split) / sizeof(test_split[0]) ; i ++ ) {
            CHECK( test_split[i].result ==  SplitString(test_split[i].str, ','))  ;
        }
    }
}
