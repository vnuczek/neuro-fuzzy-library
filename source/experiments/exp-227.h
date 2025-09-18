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

#include <optional>

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

        const std::vector<int> num_granules = {
        	2, 3, 5, 10, 20, 25, 30
        };
        const std::vector<double> missing_ratios = {
			0.01, 0.02, 0.03, 0.04, 0.05, 0.10, 0.15, 0.20, 0.25, 0.30, 0.35, 0.40, 0.45, 0.50
        };

		const bool runInParallel = true; // if true, each dataset will be processed in a separate thread

        std::mutex csv_mutex;

        struct ResultRow {
            ksi::dataset dataset;
            std::string imputerName;
            int granules;
            int iteration;
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
        void createCsvHeader(const std::filesystem::path& path, std::string_view header) const;

        void processDataset(const std::filesystem::directory_entry& entry);

        void runMissingRatio(
            const std::filesystem::path& file_path, 
            const std::string& datasetName, 
            const std::filesystem::path& datasetResultDir,
            const double missing_ratio
        );

        std::pair<ksi::dataset, ksi::dataset> loadCompleteDataAndPrepareData(const std::filesystem::path& file_path, double missing_ratio) const;

    	std::vector<std::unique_ptr<ksi::data_modifier>> makeClassicalImputers() const;

        ResultRow applyImputer(const ksi::dataset& data, ksi::data_modifier* imputer, const std::filesystem::path& datasetResultDir, std::string_view ratio_str) const;

        std::optional<ResultRow> applyGranularImputer(const ksi::dataset& data, int granules, int iteration, const std::filesystem::path& datasetResultDir, std::string_view ratio_str) const;

    	void writeDatasetToFile(const ksi::dataset& ds, const std::filesystem::path& outFilePath) const;

    	void appendPairwiseFrobenius(const ksi::dataset& completeDataset, const std::vector<ResultRow>& results, const std::string& datasetName, std::string_view ratio_str, const std::filesystem::path& csv_path);
    };
}

#endif 
