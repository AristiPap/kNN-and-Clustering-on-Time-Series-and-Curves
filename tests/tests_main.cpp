#include <iostream>
#include <CUnit/Basic.h>
#include "CurveHashing.hpp"
#include "Hashing.hpp"
using namespace std;



int init_suite1(void) {
    cout << "Added Suite 1" << endl;
    return 0;
}

int clean_suite1(void) { cout << "Cleaned Suite 1" << endl; return 0;}

CU_ErrorCode add_test(CU_pSuite pSuite, const char* msg, CU_TestFunc testF) {
    if ((NULL == CU_add_test(pSuite, msg, testF))) {
        CU_cleanup_registry();
        exit(CU_get_error());
    }
}

void test_DLSH(){
    Point point1("1",{1,9.1},L2_norm);
    Point point2("2", {2, 10}, L2_norm);
    Point point3("3", {3, 20}, L2_norm);

    Curve c1("c1", FrechetDistDiscrete, {point1, point2, point3});
    
    Point point4("4",{1,10.1},L2_norm);
    Point point5("5", {2, 9.8}, L2_norm);
    Point point6("6", {3, 19.5}, L2_norm);

    Curve c2("c2", FrechetDistDiscrete, {point4, point5, point6});
    int suc =0;
    for(auto i =0; i<1000; i++){
        DLSHHashingCurve h(1,1,2,1,3);
        Point * gc1 = h(c1);
        Point * gc2 = h(c2);
        auto w = L2_norm(*gc1,*gc2);
        LSHHashing g(3,w,gc1->getDims());
        if(g(*gc1) == g(*gc2))
            suc++;
        delete gc1;
        delete gc2;
    }
    CU_ASSERT((suc*1.0)/(1.0*1000) >= 0.5);
}

int main(void) {
    CU_pSuite pSuite = NULL;

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry()) return CU_get_error();
    
    /* add a suite to the registry */
    pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    //add_test(pSuite, "Discrete Frechet Optimal Path - Test", test_DFOptimalTraversal);
    //add_test(pSuite, "LSH w\\ Constinuous Frechet - Test", test_CLSH);
    add_test(pSuite, "LSH w\\ Discrete Frechet - Test", test_DLSH);
    //add_test(pSuite, "Mean Curve Routine - Test", test_MeanCurve);

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_run_all_tests();
    CU_cleanup_registry();
    CU_get_error();
    return 0;
} 