#include <cfloat>
#include <cmath>
#include <ctime>
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
            throw std::logic_error("Point size mismatch");
        }

        double value = 0;
        for (int i = 0; i < values.size(); i++) {
            value += pow(values.at(i) - p.values.at(i), 2);
        }

        return sqrt(value);
    }

    static double euclidean_distance(const Point &p1, const Point &p2) {
        if (p1.values.size() != p2.values.size()) {
            throw std::logic_error("Point size mismatch");
        }

        double value = 0;
        for (int i = 0; i < p1.values.size(); i++) {
            value += pow(p1.values.at(i) - p2.values.at(i), 2);
        }

        return sqrt(value);
    }

    void zero() {
        std::fill(values.begin(), values.end(), 0);
    }

    Point operator+(const Point &p) const {
        if (values.size() != p.values.size()) {
            throw std::logic_error("Point size mismatch");
        }

        Point point(values.size());

        for (int i = 0; i < values.size(); ++i) {
            point.values[i] = values[i] + p.values[i];
        }

        return point;
    }

    Point &operator+=(const Point &p) {
        if (values.size() != p.values.size()) {
            throw std::logic_error("Point size mismatch");
        }

        for (int i = 0; i < values.size(); ++i) {
            values[i] += p.values[i];
        }

        return *this;
    }

    Point operator/(double divider) const {
        Point point(values.size());

        for (int i = 0; i < values.size(); ++i) {
            point.values[i] = values[i] / divider;
        }

        return point;
    }

    Point &operator/=(double divider) {
        for (double &value : values) {
            value /= divider;
        }

        return *this;
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

        // First, create random cluster centers.
        for (int i = 0; i < n_clusters; i++) {
            Cluster cluster;

            Point random_point = samples[rand() % samples.size()];
            cluster.values = random_point.values;

            clusters.push_back(cluster);
        }

        // Main loop of the algorithm
        for (int i = 0; i < epochs; ++i) {

            // Split sample into nearest clusters.
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

            // Recalculate cluster centers
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

    bool read_csv(const std::string &file_path) {
        std::string line;

        std::ifstream file(file_path);

        if (!file.is_open()) {
            return false;
        }

        while (getline(file, line)) {
            char delimiter = ',';

            size_t start, end = 0;
            std::string token;

            Sample sample;

            while ((start = line.find_first_not_of(delimiter, end)) != std::string::npos) {
                end = line.find(delimiter, start);

                token = line.substr(start, end - start);
                sample.values.push_back(std::stod(token));
            }

            samples.push_back(sample);
        }

        return true;
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

    Data data, truth_data;

    if (!data.read_csv("breast_data.csv")) {
        std::cout << "breast_data.csv must be in the same folder as the executable file\n";
        return 1;
    }

    if (!truth_data.read_csv("breast_truth.csv")) {
        std::cout << "breast_truth.csv must be in the same folder as the executable file\n";
        return 1;
    }

    data.k_means_clustering(100, 2);

    int valid = 0;
    for (int j = 0; j < data.samples.size(); j++) {

        int prediction = data.predict(data.samples[j]);

        if (prediction == truth_data.samples[j].values[0]) {
            valid++;
        }
    }

    std::cout << "Since the K Means algorithm is unsupervised, the result may be completely "
                 "opposite to what was expected. \nIn this case, the result must be "
                 "subtracted from 100. \nOr, run the algorithm (program) several times until the "
                 "desired result is obtained. \nSpecifically for these data, the result is 0.85413 or 0.14587, "
                 "depending on the random initial arrangement of the clusters.\n";

    std::cout << "Result: " << (double) valid / data.samples.size() << std::endl;

    std::cout << "Press Enter to finish" << std::endl;

    getchar();

    return 0;
}
