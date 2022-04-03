#include "lightest.h" // Just include lightest.h
// #include <vector> is not needed actually, for lightest.h has already included it.

double avg(std::vector<int>);
double avg_wrong(std::vector<int>); // A wrong realization of average calculation.

int main() {
    DEFTEST(AvgRight) {
        double expected = 8.5;
        std::vector<int> dataSet{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
        CHK_LOG(avg(dataSet), avg(dataSet) == expected);
        LOG(AVG_TIMER(avg(dataSet), 200));
    };
    DEFTEST(AvgWrong) {
        double expected = 8.5;
        std::vector<int> dataSet{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
        CHK_LOG(avg_wrong(dataSet), avg_wrong(dataSet) == expected);
        LOG(AVG_TIMER(avg(dataSet), 200));
    };
    return 0;
}

double avg(const std::vector<int> li) {
    int sum = 0;
    for(auto item : li) sum += item;
    return double(sum) / li.size();
}

double avg_wrong(const std::vector<int> li) {
    double res = 0;
    for(auto item : li) res = ( res + item ) / 2;
    return res;
}