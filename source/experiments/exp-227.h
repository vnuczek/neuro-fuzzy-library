/** @file */

#ifndef EXP_227_H
#define EXP_227_H

#include "../experiments/experiment.h"
#include "../experiments/exp-027.h"
#include "../common/dataset.h"

#include <filesystem>
#include <map>
#include <mutex>
#include <thread>
#include <vector>
#include <string>

namespace ksi
{
    /** EXPERIMENT 227  <br/>

    granular imputation of missing values

    @date 2025-09-12
	@author Konrad Wnuk
    */

    class exp_227 : virtual public experiment
    {
    private:
        const std::string exp_number = "exp-227";

        const std::filesystem::path dataDir = "../data/" + exp_number;
        const std::filesystem::path resultDir = "../results/" + exp_number;
        const std::filesystem::path csvPath = resultDir / (exp_number + "-results.csv");

        const int k;
        const int ITERATIONS;
        const int NUMBER_OF_CLUSTERING_ITERATIONS;

        std::vector<std::thread> threads;
        std::mutex csv_mutex;

        struct ResultRow {
            std::string imputerName;
            int granules;
            ksi::dataset dataset;
        };

    public:
        exp_227(
            const int k_val = 5,
            const int iterations = 10,
            const int num_clustering_iters = 100
        );

        /** The method executes an experiment. */
        virtual void execute();

    private:
        void processDataset(const std::filesystem::directory_entry& entry);

        void runMissingRatio(
            const std::filesystem::path& file_path, 
            const std::string& datasetName, 
            const std::filesystem::path& datasetResultDir, 
            const std::vector<int>& num_granules,
            const double missing_ratio
        );
    };
}

#endif 
