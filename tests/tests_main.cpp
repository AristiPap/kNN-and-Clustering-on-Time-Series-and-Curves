#include <CUnit/Basic.h>

#include <iostream>

#include "CurveHashing.hpp"
#include "Hashing.hpp"
#define N_HASHES 100
#define DELTA 7

using namespace std;

int init_suite1(void) { return 0; }

int clean_suite1(void) { return 0; }

void add_test(CU_pSuite pSuite, const char* msg, CU_TestFunc testF) {
    if ((NULL == CU_add_test(pSuite, msg, testF))) {
        CU_cleanup_registry();
        exit(CU_get_error());
    }
}

void appr_curves(void) {
    Point point1("1", {1, 9.1}, L2_norm);
    Point point2("2", {2, 10}, L2_norm);
    Point point3("3", {3, 20}, L2_norm);

    Curve c1("c1", FrechetDistContinuous, {point1, point2, point3});

    Point point1_("1'", {1, 10.1}, L2_norm);
    Point point2_("2'", {2, 8.5}, L2_norm);
    Point point3_("3'", {3, 18.0}, L2_norm);

    Curve c2("c2", FrechetDistContinuous, {point1_, point2_, point3_});

    int succ = 0;
    // Contest a proximity test:
    // Hash with Continuous lsh and make sure the LSH_Vector hasher will assign
    // the same ID to the concatenated grid curves most of the times
    for (auto i = 0; i < N_HASHES; i++) {
        CLSHHashingCurve h(1, 1, 2, DELTA, 3);
        Point* gc1 = h(c1);
        Point* gc2 = h(c2);
        auto w = L2_norm(*gc1, *gc2);
        LSHHashing g(1, w / 10, gc1->getDims());

        if (g(*gc1) == g(*gc2)) succ++;

        delete gc1;
        delete gc2;
    }

    CU_ASSERT((succ * 1.0) / (1.0 * N_HASHES) >= 0.5);
    cout << "\nP[ID(c1) == ID(c2) | c1 appr to c2]="
         << (succ * 1.0) / (1.0 * N_HASHES) << endl;
}

void not_appr_curves() {
    Point point1("1", {1, 9.1}, L2_norm);
    Point point2("2", {2, 10}, L2_norm);
    Point point3("3", {3, 20}, L2_norm);

    Curve c1("c1", FrechetDistContinuous, {point1, point2, point3});

    Point point1_("1'", {1, 100.1}, L2_norm);
    Point point2_("2'", {2, 90.5}, L2_norm);
    Point point3_("3'", {3, 190.5}, L2_norm);

    Curve c2("c2", FrechetDistContinuous, {point1_, point2_, point3_});

    int succ = 0;
    // Contest a proximity test:
    // Hash with Continuous lsh and make sure the LSH_Vector hasher will assign
    // the same ID to the concatenated grid curves most of the times
    for (auto i = 0; i < N_HASHES; i++) {
        CLSHHashingCurve h(1, 1, 2, DELTA, 3);
        Point* gc1 = h(c1);
        Point* gc2 = h(c2);
        auto w = L2_norm(*gc1, *gc2);
        LSHHashing g(1, w / 10, gc1->getDims());

        if (g(*gc1) != g(*gc2)) succ++;

        delete gc1;
        delete gc2;
    }

    CU_ASSERT((succ * 1.0) / (1.0 * N_HASHES) >= 0.5);
    cout << "\nP[ID(c1) == ID(c2) | c1 not appr to c2]="
         << 1 - (succ * 1.0) / (1.0 * N_HASHES) << endl;
}

void test_CLSH(void) {
    appr_curves();
    not_appr_curves();
}


void test_DFOptimalTraversal(void) {
    Frechet::backtrace = true;

    Point point1("1", {1, 1}, L2_norm);
    Point point2("2", {2, 2}, L2_norm);
    Point point3("3", {3, 3}, L2_norm);

    Curve c1("c1", FrechetDistDiscrete, {point1, point2, point3});

    Point point1_("1'", {1, 101}, L2_norm);
    Point point2_("2'", {2, 102}, L2_norm);
    Point point3_("3'", {3, 103}, L2_norm);

    Curve c2("c2", FrechetDistDiscrete, {point1_, point2_, point3_});

    auto dist = c1.dist(c2);

    CU_ASSERT(Frechet::optimal_traversal.size() == 3);
    auto p = Frechet::optimal_traversal.begin();

    CU_ASSERT(p->first->getCoordinates() == point1.getCoordinates() &&
              p->second->getCoordinates() == point1_.getCoordinates());
    p++;
    CU_ASSERT(p->first->getCoordinates() == point2.getCoordinates() &&
              p->second->getCoordinates() == point2_.getCoordinates());
    p++;
    CU_ASSERT(p->first->getCoordinates() == point3.getCoordinates() &&
              p->second->getCoordinates() == point3_.getCoordinates());
    p++;
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

    add_test(pSuite, "Discrete Frechet Optimal Path - Test",
             test_DFOptimalTraversal);
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