/** @file */

#include "../experiments/exp-127.h"

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <map>
#include <thread>
#include <future>

#include "../readers/reader-complete.h"
#include "../readers/reader-incomplete.h"
#include "../readers/train_test_model.h"
#include "../readers/train_validation_test_model.h"
#include "../common/data-modifier-marginaliser.h"
#include "../common/data-modifier-normaliser.h"
// #include "../common/data-modifier-imputer.h"
#include "../common/data-modifier-imputer-average.h"
#include "../common/data-modifier-imputer-median.h"
#include "../common/data-modifier-imputer-knn-average.h"
#include "../common/data-modifier-imputer-knn-median.h"
// #include "../common/data-modifier-imputer-values-from-knn.h"
#include "../common/data_modifier_incompleter_random_without_last.h"
#include "../common/data-modifier-imputer-granular.h"
#include "../partitions/fcm.h"
#include "../auxiliary/tempus.h"

#include "../tnorms/t-norm-product.h"
// #include "../tnorms/t-norm-min.h"
// #include "../tnorms/t-norm-lukasiewicz.h"
// #include "../tnorms/t-norm-einstein.h"

#include <regex>

#include "../implications/imp-reichenbach.h"

#include "../neuro-fuzzy/neuro-fuzzy-system.h"
#include "../neuro-fuzzy/annbfis.h"
#include "../neuro-fuzzy/tsk.h"

#include "../auxiliary/utility-math.h"

ksi::exp_127::exp_127(const int num_rules, const int num_clustering_iters, const int num_tuning_iters, const int num_dataparts, const int k_val, const double eta_val, const bool normalisation, const int iteration, const std::vector<int>& granules) :
	NUMBER_OF_RULES(num_rules),
	NUMBER_OF_CLUSTERING_ITERATIONS(num_clustering_iters),
	NUMBER_OF_TUNING_ITERATIONS(num_tuning_iters),
	NUMBER_OF_DATAPARTS(num_dataparts),
	k(k_val),
	ETA(eta_val),
	NORMALISATION(normalisation),
	ITERATIONS(iteration),
	num_granules(granules)
{}

void ksi::exp_127::execute()
{
	try
	{
		thdebug("Start exp_127: " + ksi::tempus::getDateTimeNowSafe());

		std::filesystem::create_directories(resultDir);

		for (const auto& datasetFolder : std::filesystem::directory_iterator(dataDir)) {
			// thdebugid(entry, entry);

			if (datasetFolder.is_directory())
			{
				threads.emplace_back(&ksi::exp_127::processDatasetFolder, this, datasetFolder);
			}
		}

		for (auto& thread : threads)
		{
			if (thread.joinable())
			{
				thread.join();
			}
		}

		thdebug("End exp_127: " + ksi::tempus::getDateTimeNowSafe());
		thdebug("dze end");
	}
	CATCH;

	return;
}

void ksi::exp_127::processDatasetFolder(const std::filesystem::directory_entry& datasetFolder) {
	try {
		const std::string datasetName = datasetFolder.path().filename().string();

		auto datasetResultDir = resultDir / datasetName;
		std::filesystem::create_directories(datasetResultDir);

		RESULTS results;
		RESULTS_GR results_gr;

		std::regex re("^m-([0-9.]+)$");

		for (auto& ratioFolder : std::filesystem::directory_iterator(datasetFolder))
		{
			if (!ratioFolder.is_directory()) continue;

			std::smatch match;
			std::string folderName = ratioFolder.path().filename().string();
			if (std::regex_match(folderName, match, re))
			{
				double missing_ratio = std::stod(match[1].str());

				processMissingRatioFolder(ratioFolder.path(), datasetName, missing_ratio);
			}
		}
	}
	CATCH;	
}

void ksi::exp_127::processMissingRatioFolder(const std::filesystem::path& missingRatioPath, const std::string& datasetName, const double missing_ratio)
{
	try {
		ksi::RESULTS results;

		reader_complete DataReader;

	}
	CATCH;
}


