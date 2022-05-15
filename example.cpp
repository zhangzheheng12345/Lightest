#include "lightest.h" // Just include lightest.h

MAIN // Use default main function to run tests & report

/* ========== Functions to be tested ========== */

double avg(const std::vector<int> li) {
    int sum = 0;
    for(auto item : li) sum += item;
    return double(sum) / li.size();
}
double avg_wrong(const std::vector<int> li) { // a wrong realization of avg calculation
    double res = 0;
    for(auto item : li) res = ( res + item ) / 2;
    return res;
}

/* ========== Tests ========== */

class TestData {
public:
    static double expected;
    static std::vector<int> dataSet;
};
double TestData::expected = 8.5;
std::vector<int> TestData::dataSet{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};

DEFTEST(AvgRight) {
    CHK_OP(TestData::expected, avg(TestData::dataSet), ==);
}
DEFTEST(AvgWrong) {
    CHK_OP(TestData::expected, avg_wrong(TestData::dataSet), ==);
}
DEFTEST(AvgSpeed) {
    std::vector<int> li(0);
    for(int i = 0; i < 5000; i++) li.push_back(i);
    LOG(AVG_TIMER(avg(li), 10000));
}
