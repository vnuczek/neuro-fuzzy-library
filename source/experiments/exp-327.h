/** @file */

#ifndef EXP_327_H
#define EXP_327_H


#include <filesystem>
#include <string>
#include <unordered_set>
#include <vector>

#include "../experiments/experiment.h"

namespace ksi {
	class datum;
	class dataset;
	class partition;
	/** EXPERIMENT 327  <br/>
     *
     *
     * @date 2026-02-22
     */
    class exp_327 : virtual public experiment
    {
    private:
        const std::string name = "exp-327";
		const std::string extention = ".txt";
        const std::vector<int> granules = { 2, 3 , 4, 5, 6, 7, 8, 9, 10, 15, 20, 25, 30, 40, 50, 60, 70, 80, 90, 100 };
        const std::vector<int> iterations = { 100 };
        const std::vector<double> thresholds = { 0.0001, 0.001, 0.01};
		const std::vector<int> sigmas = {2, 3, 5};

    public:

        /** The method executes an experiment. */
        virtual void execute();

    private:
        void process_file(const std::filesystem::path& filePath);

        /** The method processes a single file with granular outlier removal. */
        void process_granular(const std::string& outputDir, const std::filesystem::path& filePath, const ksi::dataset& originalData);

        /** The method processes a single file with sigma outlier removal. */
        void process_sigma(const std::string& outputDir, const std::filesystem::path& filePath, const ksi::dataset& originalData);

        /** The method saves cleaned data, outliers, and granule parameters to files. */
        void save_granular_results(
            const std::string& outputDir,
            const std::filesystem::path& filePath,
            int g, int it, double th,
            const ksi::dataset& originalData,
            ksi::dataset& data,
            const ksi::partition& part
        );

        /** The method saves partition granules (descriptor parameters) to a file. */
        void save_granules(const std::string& filePath, const ksi::partition& part);

        inline std::string make_granular_output_name(
            const std::filesystem::path& input,
            const int granules,
            const int iterations,
            const double threshold,
            const std::string& suffix
        );

        inline std::string make_sigma_output_name(const std::filesystem::path& input, const int n, const std::string& suffix);

        std::unordered_set<const ksi::datum*> collect_pointers(const ksi::dataset& ds);

    public:
        ksi::dataset extract_outliers(
            const ksi::dataset& original,
            const ksi::dataset& cleaned
        );
    };
}

#endif