#include <cfloat>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

struct Point {

    std::vector<double> values;

    Point() = default;

    explicit Point(int size) : values(size, 0) {
    }

    double euclidean_distance(const Point &p) {
        if (values.size() != p.values.size()) {
            throw "Point size mismatch";
        }

        double value = 0;
        for (int i = 0; i < values.size(); i++) {
            value += pow(values.at(i) - p.values.at(i), 2);
        }

        return sqrt(value);
    }

    static double euclidean_distance(const Point &p1, const Point &p2) {
        if (p1.values.size() != p2.values.size()) {
            throw "Point size mismatch";
        }

        double value = 0;
        for (int i = 0; i < p1.values.size(); i++) {
            value += pow(p1.values.at(i) - p2.values.at(i), 2);
        }

        return sqrt(value);
    }

    void zero() {
        for (auto d : values) {
            d = 0;
        }
    }

    Point operator+(const Point &p) {
        if (values.size() != p.values.size()) {
            throw "Point size mismatch";
        }

        Point point(values.size());

        for (int i = 0; i < values.size(); ++i) {
            point.values[i] = values[i] + p.values[i];
        }

        return point;
    }

    Point operator+=(const Point &p) {
        if (values.size() != p.values.size()) {
            throw "Point size mismatch";
        }

        Point point(values.size());

        for (int i = 0; i < values.size(); ++i) {
            point.values[i] = values[i] + p.values[i];
        }

        return point;
    }

    Point operator/(double divider) const {
        Point point(this->values.size());

        for (int i = 0; i < values.size(); ++i) {
            point.values[i] /= divider;
        }

        return point;
    }

    Point operator/=(double divider) const {
        Point point(this->values.size());

        for (int i = 0; i < values.size(); ++i) {
            point.values[i] /= divider;
        }

        return point;
    }
};

struct Sample : Point {
};

struct Cluster : Point {
    std::vector<Sample> samples;
};

struct Data {
    std::vector<Sample> samples;
    std::vector<Cluster> clusters;

    void k_means_clustering(int epochs, int n_clusters) {


        for (int i = 0; i < n_clusters; i++) {
            Cluster cluster;

            Point random_point = samples.at(rand() % samples.size());
            cluster.values = random_point.values;

            clusters.push_back(cluster);
        }

        for (int i = 0; i < epochs; ++i) {

            for (const auto &s : samples) {

                auto min_dist = DBL_MAX;
                Cluster *min_cluster;

                for (auto &c : clusters) {
                    double dist = c.euclidean_distance(s);

                    if (dist < min_dist) {
                        min_dist = dist;
                        min_cluster = &c;
                    }
                }

                min_cluster->samples.push_back(s);
            }


            for (auto &c : clusters) {
                c.zero();

                for (const auto &s : c.samples) {
                    c += s;
                }

                c /= c.samples.size();

                if (i != epochs - 1)
                    c.samples.clear();
            }
        }
    }

    int predict(Point point) {
        auto min_dist = DBL_MAX;
        int min_cluster = -1;
        for (int i = 0; i < clusters.size(); i++) {
            double dist = point.euclidean_distance(clusters[i]);

            if (dist < min_dist) {
                min_dist = dist;
                min_cluster = i;
            }
        }

        return min_cluster;
    }

    void read_csv(const std::string &file_path) {
        std::string line;

        std::ifstream file(file_path);


        while (getline(file, line)) {
            char delimiter = ',';

            size_t start = 0, end = 0;
            std::string token;

            Sample sample;

            while ((start = line.find_first_not_of(delimiter, end)) != std::string::npos) {
                end = line.find(delimiter, start);

                token = line.substr(start, end - start);
                sample.values.push_back(std::stod(token));
            }

            samples.push_back(sample);
        }
    }

    void print() {
        for (const auto &s : samples) {
            for (auto f : s.values) {
                std::cout << f << ", ";
            }

            std::cout << "\b\b\n";
        }
    }
};


int main() {

    srand(time(nullptr));


    for (int i = 0; i < 100; ++i) {
        Data data, truth_data;
        data.read_csv("../breast_data.csv");
        truth_data.read_csv("../breast_truth.csv");

        data.k_means_clustering(10000, 2);

        int valid = 0;
        for (int i = 0; i < data.samples.size(); i++) {

            int prediction = data.predict(data.samples[i]);

            if (prediction == truth_data.samples[i].values[0]) {
                valid++;
            }
        }

        std::cout << (double) valid / data.samples.size() << std::endl;
    }


    return 0;
}
