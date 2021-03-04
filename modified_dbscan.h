#ifndef DBSCAN_MODIFIED_DBSCAN_H
#define DBSCAN_MODIFIED_DBSCAN_H

using namespace std;

namespace db {
    class DataPoint {
    public:
        DataPoint(float val, float xval, set<DataPoint> neighbours, int type);

        float val;
        float xval;
        set<DataPoint> neighbours;
        int type;
    };

    typedef DataPoint datum;
    typedef vector<datum> data_arr;
    typedef vector<data_arr> data_arrs;

    class DBSCAN {
    public:
        DBSCAN(float epsilon, int min_points, vector<vector<float>> data, int num_imus);

        set<DataPoint> get_outliers();

    private:
        string const DATA_TYPES[4] = {"Unknown", "Noise", "Border", "Core"};
        data_arrs DATA;
        vector <int> visit_order;
        int curr_idx = 0;
        float epsilon;
        int min_points;
        data_arrs data;
        int num_imus;
        set<DataPoint> CORE;
        set<DataPoint> BORDER;
        set<DataPoint> OUTLIER;

        void run();
        void parse_data(vector<vector<float>> data);
        void init_rand();
        void logic_handler();
        bool has_min_vertical_neighbours();
        datum visit_random();
        data_arr init_neighbours();
        bool require_replacement();
        datum best_approximate();
        void adjust_classifier();
        float correlation_coefficient(vector<float> x_vals, vector<float> y_vals);
        float euclidean_dist(datum p, datum q);

    };
}


#endif //DBSCAN_MODIFIED_DBSCAN_H
