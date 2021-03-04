#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <random>
#include "modified_dbscan.h"
#include "historic_data.h"

using namespace db;
using hd::HistoricData;

DataPoint::DataPoint(float val, float xval) {
    this->val = val;
    this->xval = xval;
}


DBSCAN::DBSCAN(float epsilon, int min_points, vector<vector<float>> data, int num_imus) {
    this->epsilon = epsilon;
    this->min_points = min_points;
    this->num_imus = num_imus;

    parse_data(data);
    run();
}

void DBSCAN::parse_data(vector<vector<float>> data) {
    // Iterate through each data array
    for (int i=0; i < data.size(); i++){
        // Iterate through each datum
        for (int j=0; j < data[i].size(); j++){
            DATA.push_back(DataPoint(data[i][j], (float) i));
        }
    }
}

void DBSCAN::init_rand() {
    for (int i=0; i<DATA.size(); i++){
        visit_order.push_back(i);
    }
    // Randomly shuffle the visit order
    auto rng = default_random_engine {};
    shuffle(begin(visit_order), end(visit_order), rng);

    curr_idx = -1;
}

void DBSCAN::run(){
    for (datum _ : DATA){
        init_rand();
        while (curr_idx != DATA.size()-1){
            logic_handler();
        }
        verify_trend();
    }
}

/*
 * Handles main logic of DBSCAN class, as described in documentation
 */
void DBSCAN::logic_handler() {
    // Step 1
    datum data_point = visit_random();
    // If we already know data is a core point, skip (for efficiency)
    if (data_point.type != 3) {
        // Step 2
        data_arr neighbours = init_neighbours(data_point);
        if (!has_min_vertical_neighbours(data_point, neighbours)){
            data_point.type = 1;
            OUTLIER.insert(data_point);
            // Exit method
        }
        else {
            if (neighbours.size() >= min_points){
                data_point.type = 3;
                CORE.insert(data_point);
            }
            else {
                for (datum neighbour : neighbours){
                    if (neighbour.type == 3){
                        data_point.type = 2;
                        BORDER.insert(data_point);
                    }
                }
                data_point.type = 1;
                OUTLIER.insert(data_point);
            }
        }
    }
}

/*
 * If there are no vertical neighbours, then consider outlier
 */
bool DBSCAN::has_min_vertical_neighbours(const datum& data_point, const data_arr& neighbours) {
    for (const datum& neighbour: neighbours){
        if (neighbour.xval == data_point.xval)
            return true;
    }
    return false;
}

/*
 * Visit random data point in the data set
 */
datum DBSCAN::visit_random() {
    curr_idx++;
    return DATA[visit_order[curr_idx]];
}

/*
 * Find all neighbours within epsilon of given data point
 */
data_arr DBSCAN::init_neighbours(datum data_point) {
    data_arr neighbours;
    for (datum data : DATA){
        if (data.val != data_point.val){
            if (euclidean_dist(data, data_point) <= epsilon){
                neighbours.push_back(data_point);
                data_point.neighbours.insert(data);
            }
        }
    }
    return neighbours;
}

bool DBSCAN::require_replacement(int i, int j) {
    //  If point is outlier, and has neighbours => 50-50 split or worse
    //  Assumes the point itself is not considered its own neighbour
    return (DATA[i+j].type == 1) && (DATA[i+j].neighbours.size() > 0);
}

datum DBSCAN::best_approximate(float r, int i, int j, data_arr data) {
    // y = mx + c
    float prediction = r * DATA[i + j].xval + DATA[i].val;
    // Find the closest datum to the predicted point
    int min_diff = 10;
    float diff;
    datum min_diff_datum = DataPoint(0, 0);

    for (const datum& k : data){
        diff = abs(k.val - prediction);
        if (diff < min_diff) {
            min_diff = diff;
            min_diff_datum = k;
        }
    }
    return min_diff_datum;
}

void DBSCAN::adjust_classifier(data_arr data, datum closest_datum) {
    for (datum adjacent : data) {
        auto result = find(closest_datum.neighbours.begin(), closest_datum.neighbours.end(), adjacent);
        // If adjacent is a neighbour
        if (result != end(closest_datum.neighbours)){
            adjacent.type = 3;
            CORE.insert(adjacent);
            auto result = find(OUTLIER.begin(), OUTLIER.end(), adjacent);
            // If adjacent is an outlier
            if (result != end(OUTLIER)){
                OUTLIER.erase(adjacent);
            }
        }
    }
    closest_datum.type = 3;
    CORE.insert(closest_datum);
    OUTLIER.erase(closest_datum);
}

/*
 * Final method to be called after all data is clustered.
 * Verifies the outliers are correctly chosen for each time interval, and acts accordingly.
 * This is discussed in more detail in the paper, under 'Implementation Complications and Design Choices'
 */
void DBSCAN::verify_trend() {
    vector<float> clean_data_x;
    vector<float> clean_data_y;
    for (datum k : DATA) {
        if (k.type != 1) {
            clean_data_x.push_back(k.xval);
            clean_data_y.push_back(k.val);
        }
    }
    float r = correlation_coefficient(clean_data_x, clean_data_y);

    // Iterate through all time intervals.
    for (int i=0; i < DATA.size(); i+=num_imus){
        data_arr data;
        for (int k=i; k < i + num_imus; k++){
            data.push_back(DATA[k]);
        }
        // Check if there is a 50-50 split on outlier decision
        for (int j=0; j < num_imus; j++){
            if (require_replacement(i, j)){
                //  Then, find point closest (and neighbours) to predicted point (using correlation coeff)
                datum closest_datum = best_approximate(r, i, j, data);
                // If best approximate is classed as noise, swap noise and core types within interval
                if (closest_datum.type == 1)
                    adjust_classifier(data, closest_datum);
            }
        }
    }
}

template <typename T>
float sum( T t )
{
    float sum_of_elems = 0;
    for (auto& n : t)
        sum_of_elems += n;

    return sum_of_elems;
}

float DBSCAN::correlation_coefficient(vector<float> x_vals, vector<float> y_vals) {
    float x_mean = sum(x_vals) / x_vals.size();
    float y_mean = sum(y_vals) / y_vals.size();

    vector<float> covar_vector (x_vals.size());
    for (int i=0; i<x_vals.size(); i++){
        covar_vector.push_back((x_vals[i] - x_mean) * (y_vals[i] - y_mean));
    }
    float covar = sum(covar_vector);

    vector<float> x_var_vector (x_vals.size());
    for (float x_val : x_vals){
        x_var_vector.push_back(pow(x_val - x_mean, 2));
    }
    float x_var = sqrt(sum(x_var_vector));

    vector<float> y_var_vector (y_vals.size());
    for (float y_val : y_vals){
        y_var_vector.push_back(pow(y_val - y_mean, 2));
    }
    float y_var = sqrt(sum(y_var_vector));

    return covar / (x_var * y_var);
}

double DBSCAN::euclidean_dist(datum p, datum q) {
    return sqrt(
            pow((p.val - q.val), 2)
            + pow((p.xval - q.xval), 2)
            );
}

set<DataPoint> DBSCAN::get_outliers() {
    return OUTLIER;
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