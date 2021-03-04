#include <iostream>
#include <vector>
#include <set>
#include <string>
#include "modified_dbscan.h"
#include "historic_data.h"

using namespace db;
using hd::HistoricData;

DataPoint::DataPoint(float val, float xval, set<DataPoint> neighbours, int type) {
    this->val = val;
    this->xval = xval;
    this->neighbours = neighbours;
    this->type = type;
}

DBSCAN::DBSCAN(float epsilon, int min_points, vector<vector<float>> data, int num_imus) {
    this->epsilon = epsilon;
    this->min_points = min_points;
    this->num_imus = num_imus;

    parse_data(data);
    run();
}

void DBSCAN::parse_data(vector<vector<float>> data) {

}

int main(){
    float EPSILON = 0.5;
    int MIN_POINTS = 2;
    int NUM_IMUS = 4;

    HistoricData buffer(NUM_IMUS);
    buffer.add({1.23, 1.22, 1.21, 1.22});
    buffer.add({1.33, 1.32, 1.31, 1.32});
    buffer.add({3.4, 1.42, 1.41, 1.42});
    buffer.add({1.33, 1.32, 1.31, 1.32});

    DBSCAN dbscan(EPSILON, MIN_POINTS, buffer.get_data(), NUM_IMUS);
    dbscan.get_outliers();

    return 0;
}