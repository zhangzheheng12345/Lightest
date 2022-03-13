#include "lightest.h" // Just include lightest.h
// #include <vector> is not needed actually, for lightest.h has already included it.

double avg(std::vector<int>);
double avg_wrong(std::vector<int>);

int main() {
    DEFCASE(AvgCase) {
        double expected = 2;
        DEFTEST(AvgRight) {
            // FIXME: vectors cannot be put out of tests
            std::vector<int> dataSet{1,2,3};
            CHK_LOG(avg(dataSet), avg(dataSet) == expected);
        };
        DEFTEST(AvgWrong) {
            std::vector<int> dataSet{1,2,3};
            CHK_LOG(avg_wrong(dataSet), avg_wrong(dataSet) == expected);
        };
    };
    return 0;
}

double avg(std::vector<int> li) {
    int sum = 0;
    for(auto item : li) sum += item;
    return sum / li.size();
}

double avg_wrong(std::vector<int> li) {
    double res = 0;
    for(auto item : li) res = ( res + item ) / 2;
    return res;
}