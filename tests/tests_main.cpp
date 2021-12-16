#include <iostream>
#include <CUnit/Basic.h>
#include "CurveHashing.hpp"
#include "Hashing.hpp"
#define N_HASHES 1000

using namespace std;

int init_suite1(void) {
    cout << "Added Suite 1" << endl;
    return 0;
}

int clean_suite1(void) { cout << "Cleaned Suite 1" << endl; return 0;}

void add_test(CU_pSuite pSuite, const char* msg, CU_TestFunc testF) {
    if ((NULL == CU_add_test(pSuite, msg, testF))) {
        CU_cleanup_registry();
        exit(CU_get_error());
    }
}

void test_CLSH(void) {
    Point point1("1", {1, 9.1}, L2_norm);
    Point point2("2", {2, 10}, L2_norm);
    Point point3("3", {3, 20}, L2_norm);

    Curve c1("c1", FrechetDistContinuous, {point1, point2, point3});

    Point point1_("1'", {1, 10.1}, L2_norm);
    Point point2_("2'", {2, 9.5}, L2_norm);
    Point point3_("3'", {3, 14}, L2_norm);

    Curve c2("c2", FrechetDistContinuous, {point1_, point2_, point3_});

    int succ = 0;
    // Contest a proximity test:
    // Hash with Continuous lsh and make sure the LSH_Vector hasher will assign the same ID to the concatenated grid curves most of the times
    for (auto i = 0; i < N_HASHES; i++) {
        CLSHHashingCurve h(1, 1, 2, 1, 3);
        Point* gc1 = h(c1);
        Point* gc2 = h(c1);
        auto w = L2_norm(*gc1, *gc2);
        LSHHashing g(3, w, gc1->getDims());

        if (g(*gc1) == g(*gc2)) succ++;

        delete gc1;
        delete gc2;
    }

    CU_ASSERT((succ*1.0)/(1.0*N_HASHES) >= 0.5);
    CU_PASS("Succeed with rate of " + string((succ * 1.0) / (1.0 * N_HASHES) * 100) + "%");
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

    // add_test(pSuite, "Discrete Frechet Optimal Path - Test", test_DFOptimalTraversal);
    add_test(pSuite, "LSH w\\ Constinuous Frechet - Test", test_CLSH);
    // add_test(pSuite, "LSH w\\ Discrete Frechet - Test", test_DLSH);
    // add_test(pSuite, "Mean Curve Routine - Test", test_MeanCurve);

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    CU_get_error();
    return 0;
} 