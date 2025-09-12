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
	class data_modifier;
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

        ksi::dataset loadAndPrepareData(const std::filesystem::path& file_path, double missing_ratio) const;

    	std::vector<std::unique_ptr<ksi::data_modifier>> makeClassicalImputers() const;

        ksi::exp_227::ResultRow applyImputer(const ksi::dataset& base, ksi::data_modifier* imputer, const std::filesystem::path& outDir, std::string_view ratio_str) const;

        ksi::exp_227::ResultRow applyGranularImputer(const ksi::dataset& base, int granules, int iteration, const std::filesystem::path& datasetResultDir, std::string_view ratio_str) const;

    	void writeDatasetToFile(const ksi::dataset& ds, const std::filesystem::path& outFilePath) const;

    	void appendPairwiseFrobenius(const std::string& datasetName, std::string_view ratio_str, const std::vector<ResultRow>& results, const std::filesystem::path& csvPath);
    };
}

#endif 
