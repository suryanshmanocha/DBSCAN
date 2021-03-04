#ifndef DBSCAN_MODIFIED_DBSCAN_H
#define DBSCAN_MODIFIED_DBSCAN_H

using namespace std;

namespace db {
    class DataPoint {
    public:
        DataPoint(float val, float xval);

        float val;
        float xval;
        set<DataPoint> neighbours;
        int type = 0;
    };

    typedef DataPoint datum;
    typedef vector<datum> data_arr;

    class DBSCAN {
    public:
        DBSCAN(float epsilon, int min_points, vector<vector<float>> data, int num_imus);

        set<DataPoint> get_outliers();

    private:
        string const DATA_TYPES[4] = {"Unknown", "Noise", "Border", "Core"};
        data_arr DATA;
        vector<int> visit_order;
        int curr_idx = 0;
        float epsilon;
        int min_points;
        int num_imus;
        set<DataPoint> CORE;
        set<DataPoint> BORDER;
        set<DataPoint> OUTLIER;

        void run();
        void parse_data(vector<vector<float>> data);
        void init_rand();
        void logic_handler();
        bool static has_min_vertical_neighbours(const datum& data_point, const data_arr& neighbours);
        datum visit_random();
        data_arr init_neighbours(datum data_point);
        bool require_replacement(int i, int j);
        datum best_approximate(float r, int i, int j, data_arr data);
        void adjust_classifier(data_arr data, datum closest_datum);
        void verify_trend();
        float static correlation_coefficient(vector<float> x_vals, vector<float> y_vals);
        double static euclidean_dist(datum p, datum q);

    };
}


#endif //DBSCAN_MODIFIED_DBSCAN_H
