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
        const std::vector<int> iterations = { 10, 100, 1000 };
        const double threshold = 0.001;
		const int n = 3;

    public:

        /** The method executes an experiment. */
        virtual void execute();

    private:
        void process_file(const std::filesystem::path& filePath);

        std::string make_output_name(
            const std::filesystem::path& input,
            int granules,
            int iterations,
            const std::string& suffix
        );

    private:
        static std::unordered_set<const ksi::datum*> collect_pointers(const ksi::dataset& ds);

    public:
        ksi::dataset extract_outliers(
            const ksi::dataset& original,
            const ksi::dataset& cleaned
        );
    };
}

#endif