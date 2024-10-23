/** @file */

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

#include "../experiments/exp-027.h"

#include "../readers/reader-complete.h"
#include "../readers/reader-incomplete.h"
#include "../readers/train_test_model.h"
#include "../readers/train_validation_test_model.h"
#include "../common/data-modifier-marginaliser.h"
#include "../common/data-modifier-normaliser.h"
#include "../common/data-modifier-imputer.h"
#include "../common/data-modifier-imputer-average.h"
#include "../common/data-modifier-imputer-median.h"
#include "../common/data-modifier-imputer-knn-average.h"
#include "../common/data-modifier-imputer-knn-median.h"
#include "../common/data-modifier-imputer-values-from-knn.h"
#include "../common/data_modifier_incompleter_random_without_last.h"
#include "../common/data-modifier-imputer-granular.h"
#include "../partitions/fcm.h"

#include "../tnorms/t-norm-product.h"
#include "../tnorms/t-norm-min.h"
#include "../tnorms/t-norm-lukasiewicz.h"
#include "../tnorms/t-norm-einstein.h"

#include "../implications/imp-reichenbach.h"

#include "../neuro-fuzzy/neuro-fuzzy-system.h"
#include "../neuro-fuzzy/annbfis.h"
#include "../neuro-fuzzy/tsk.h"


void ksi::exp_027::execute()
{
    try
    {
        std::string exp_number = "exp-027";

        std::filesystem::path dataDir = "../data/" + exp_number;
        std::filesystem::path resultDir = "../results/" + exp_number;

        const int NUMBER_OF_RULES = 5;
        const int NUMBER_OF_CLUSTERING_ITERATIONS = 100;
        const int NUMBER_OF_TUNING_ITERATIONS = 100;
		const int NUMBER_OF_DATAPARTS = 10;
        const int k = 5;
        const double ETA = 0.001;

        const bool NORMALISATION = false;

        std::vector<std::thread> threads;
        for (int iteration = 0; iteration < 13; iteration++) {
            for (const auto& entry : std::filesystem::directory_iterator(dataDir))
            {
                if (entry.is_regular_file())
                {
                    threads.emplace_back([=]() {
                        std::filesystem::path file_path = entry.path();
                        std::vector<int> num_granules;

                        if (file_path.filename().string().find("BoxJ290") != std::string::npos) {
                            num_granules = { 10, 20, 50, 100 };
                        }
                        else {
                            num_granules = { 10, 20, 50, 100, 200, 500, 750, 1000 };
                        }

                        reader_complete DataReader;
                        ksi::train_test_model tt(DataReader);
                        auto data = tt.read_file(file_path);

                        data_modifier_normaliser normaliser;
                        normaliser.modify(data);
                        tt.split(data, NUMBER_OF_DATAPARTS);

                        ksi::t_norm_product tnorm;
                        ksi::imp_reichenbach implication;
                        ksi::tsk t(NUMBER_OF_RULES, NUMBER_OF_CLUSTERING_ITERATIONS, NUMBER_OF_TUNING_ITERATIONS, ETA, NORMALISATION, tnorm);
                        ksi::annbfis a(NUMBER_OF_RULES, NUMBER_OF_CLUSTERING_ITERATIONS, NUMBER_OF_TUNING_ITERATIONS, ETA, NORMALISATION, tnorm, implication);

                        std::vector<std::unique_ptr<ksi::data_modifier>> imputers;
                        imputers.push_back(std::make_unique<ksi::data_modifier_imputer_average>());
                        imputers.push_back(std::make_unique<ksi::data_modifier_imputer_knn_median>(k));
                        imputers.push_back(std::make_unique<ksi::data_modifier_imputer_knn_average>(k));
                        imputers.push_back(std::make_unique<ksi::data_modifier_imputer_values_from_knn>(k));
                        imputers.push_back(std::make_unique<ksi::data_modifier_marginaliser>());

                        for (const auto missing_ratio : { 0.01, 0.02, 0.05, 0.10, 0.20, 0.50, 0.75 }) {
                            data_modifier_incompleter_random_without_last incomplete(missing_ratio);

                            for (auto [train, test] : tt) {
                                incomplete.modify(test);

                                for (auto& imputer : imputers) {
                                    imputer->modify(train);

                                    //**@todo Zaimplementowac imputer.getName() */

                                    std::string output_name = imputer->getName() + '-' + std::to_string(missing_ratio) + "-r-" + std::to_string(iteration) + ".txt";
                                    std::string output_file_t = resultDir.string() + "/tsk-" + output_name;
                                    std::string output_file_a = resultDir.string() + "/annbfis-" + output_name;

                                    auto result_t = t.experiment_regression(train, test, output_file_t);
                                    auto result_a = a.experiment_regression(train, test, output_file_a);
                                }

                                for (const auto granules : num_granules) {
                                    ksi::fcm test_partitioner(granules, NUMBER_OF_CLUSTERING_ITERATIONS);
                                    std::unique_ptr<ksi::data_modifier> imputer = std::make_unique < data_modifier_imputer_granular>(test_partitioner, tnorm);
                                    imputer->modify(train);

                                    std::string output_name = std::format("{}-{}-g-{}-r-{}.txt", imputer->getName(), missing_ratio, granules, iteration);
                                    std::string output_file_t = resultDir.string() + "/tsk-" + output_name;
                                    std::string output_file_a = resultDir.string() + "/annbfis-" + output_name;

                                    auto result_t = t.experiment_regression(train, test, output_file_t);
                                    auto result_a = a.experiment_regression(train, test, output_file_a);
                                }
                            }
                        }
                        });
                }
                for (auto& thread : threads) {
                    if (thread.joinable()) {
                        thread.join();
                    }
                }
            }
        }
    }
	CATCH;

    return;
}
