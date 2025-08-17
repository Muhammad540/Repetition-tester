#ifndef REPETITION_TESTER_H
#define REPETITION_TESTER_H

#include <time.h>
#include <vector>

// utility timer class
struct Timer {
    static uint64_t now() {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return ts.tv_sec * 1000000000ULL + ts.tv_sec;
    }
    static double toSeconds(uint64_t ns){
        return static_cast<double>(ns)/1e9;
    }
    static double toMilliSeconds(uint64_t ms){
        return static_cast<double>(ms)/1e6;
    }
};

class ScopedTimer{
    private:
        uint64_t start_time;
        uint64_t *result;

    public:
        ScopedTimer(uint64_t *storage) : result(storage){
            start_time = Timer::now();
        }
        ~ScopedTimer(){
            uint64_t end_time = Timer::now();
            *result = end_time - start_time;
        }
};

class TestStatistics{
    private:
        std::vector<uint64_t> measurements;
        std::string test_name;
        uint64_t total_bytes = 0.0;
    
    public:
        TestStatistics(const std::string &name): test_name{name} {}

        void addMeasurement(uint64_t time_ns, uint64_t bytes){
            measurements.push_back(time_ns);
            total_bytes = bytes;
        }
        
        void reset() {
            measurements.clear();
        }

        uint64_t getMin() const {
            return measurements.empty() ? 0 : *std::min_element(measurements.begin(), measurements.end());
        }

        uint64_t getMax() const {
            return measurements.empty() ? 0 : *std::max_element(measurements.begin(), measurements.end());
        }

        double getMean() const {
            if (measurements.empty()) return 0.0;
            uint64_t sum = 0;
            for (uint64_t m : measurements){
                sum+=m;
            }
            return (double)sum/measurements.size();
        }

        double getThroughputGBps() const{
            if(measurements.empty() || total_bytes == 0) return 0.0;

            double avgTime = Timer::toSeconds(getMean());
            double gigabytes = (double)total_bytes / (1024.0*1024.0*1024.0);
            return gigabytes/avgTime;
        }

        const std::string& getName() const { return test_name; }
        size_t getTestCount() const { return measurements.size(); }
        
        void printReport() const {
            printf("\n=== %s ===\n", test_name.c_str());
            printf("Tests run: %zu\n", measurements.size());
            
            if (measurements.empty()) {
                printf("No measurements recorded\n");
                return;
            }
            
            printf("Min:    %.3f ms\n", LinuxTimer::toMilliseconds(getMin()));
            printf("Max:    %.3f ms\n", LinuxTimer::toMilliseconds(getMax()));
            printf("Mean:   %.3f ms\n", LinuxTimer::toMilliseconds(getMean()));
            
            if (total_bytes > 0) {
                printf("Throughput: %.3f GB/s\n", getThroughputGBps());
            }
        }
};

class RepetitionTester {
    private:
        struct TestFunction{
            std::string name;
            std::function<bool(uint64_t&)> test_func;
            TestStatistics stats;

            TestFunction(const std::string& name, std::function<bool(uint64_t&)> f): name{name}, test_func{f}, stats{n} {};
        };

        std::vector<TestFunction> registered_tests;
        uint64_t test_duration_seconds = 10;
        uint64_t min_iterations = 100;
        bool show_progress = true;
    
    public:
        void registerTest(const std::string& name, std::function<bool(uint64_t&)> test_func) {
            registered_tests.emplace_back(name,test_func);
        }

        void setTestDuration(uint64_t seconds){
            test_duration_seconds = seconds;
        }
        void setMinIterations(uint64_t iterations){
            min_iterations = iterations;
        }
        void setShowProgress(bool show){
            show_progress = show;
        }

        void runAllTests(){
            printf("Running %zu registered tests...\n", registered_tests.size());
            printf("Test duration: %llu seconds per test\n\n", test_duration_seconds);

            for (auto& test: registered_tests){
                runSingleTest(test);
            }
            printComparisonReport();
        }

        ScopedTimer createTimer(uint64_t *storage){
            return ScopedTimer(storage)
        }
    private:
        void runSingleTest(TestFunction& test){
            std::cout << "Running " << test.name << "... " << std::endl;
            test.stats.reset();

            uint64_t test_start = Timer::now();
            uint64_t duration_ns = test_duration_seconds * 1000000000ULL;
            uint64_t iterations = 0;

            while (true){
                uint64_t elapsed = Timer::now() - test_start;
                // continue
            }
        }
};


#endif // REPETITION_TESTER_H

