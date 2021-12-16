#include <iostream>
#include <CUnit/CUnit.h>
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

    add_test(pSuite, "Discrete Frechet Optimal Path - Test", test_DFOptimalTraversal);
    add_test(pSuite, "LSH w\\ Constinuous Frechet - Test", test_CLSH);
    add_test(pSuite, "LSH w\\ Discrete Frechet - Test", test_DLSH);
    add_test(pSuite, "Mean Curve Routine - Test", test_MeanCurve);

    /* Run all tests using the CUnit Basic interface */
    CU_run_all_tests();
    CU_cleanup_registry();
    CU_get_error();
    return 0;
} 