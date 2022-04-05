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
DEFTEST(AvgRight) {
    double expected = 8.5;
    std::vector<int> dataSet{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    CHK_LOG(avg(dataSet), avg(dataSet) == expected);
    LOG(AVG_TIMER(avg(dataSet), 200));
}
DEFTEST(AvgWrong) {
    double expected = 8.5;
    std::vector<int> dataSet{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    CHK_LOG(avg_wrong(dataSet), avg_wrong(dataSet) == expected);
    LOG(AVG_TIMER(avg(dataSet), 200));
}
