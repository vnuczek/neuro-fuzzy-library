/** @file */

#include "../experiments/exp-327.h"

#include <filesystem>
#include <thread>
#include <fstream>

#include "../service/exception.h"
#include "../readers/reader-complete.h"
#include "../partitions/fcm.h"
#include "../common/data_modifier_outlier_remove_granular.h"
#include "../tnorms/t-norm-min.h"
#include "../snorms/s-norm-max.h"
#include "../common/dataset.h"
#include "../common/data-modifier-outlier-remove-sigma.h"


void ksi::exp_327::execute()
{
    try
    {
        std::vector<std::thread> workers;

        std::string data_dir = "../data/" + this->name;
        for (const auto& entry : std::filesystem::directory_iterator(data_dir))
        {
            if (entry.is_regular_file() && entry.path().extension() == this->extention)
            {
                workers.emplace_back(
                    &exp_327::process_file,
                    this,
                    entry.path()
                );
            }
        }

        for (auto& worker : workers)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
    }
    CATCH;
}

void ksi::exp_327::process_file(const std::filesystem::path& filePath)
{
	try
	{
		std::string outputDir_ = "../results/" + this->name;
        std::filesystem::create_directories(outputDir_);

        ksi::reader_complete reader;
        const auto originalData = reader.read(filePath.string());

        for (const int g : this->granules)
        {
            for (const int it : this->iterations)
            {
                auto data = originalData;

                ksi::fcm partitioner(g, it);
                ksi::data_modifier_outlier_remove_granular remover(
                    partitioner,
                    ksi::t_norm_min(),
                    ksi::s_norm_max(),
                    this->threshold
                );

                remover.modify(data);
                {
                    std::ofstream cleaned_file(
                        outputDir_ + "/" + make_output_name(filePath, g, it, "cleaned")
                    );
                    cleaned_file << data;
                }

                ksi::dataset outliers = extract_outliers(originalData, data);
                {
                    std::ofstream outliers_file(
                        outputDir_ + "/" + make_output_name(filePath, g, it, "outliers")
                    );
                    outliers_file << outliers;
                }
            }
        }

        ksi::data_modifier_outlier_remove_sigma remover(this->n);
		auto data = originalData;
        std::string base_name = filePath.stem().string() + "_n_" + std::to_string((this->n)) + "_";

        remover.modify(data);
        {
            std::ofstream cleaned_file(
                outputDir_ + "/" + (base_name + "cleaned_sigma")
            );
            cleaned_file << data;
        }
        ksi::dataset outliers = extract_outliers(originalData, data);
         {
            std::ofstream outliers_file(
                outputDir_ + "/" + (base_name + "outliers_sigma")
            );
            outliers_file << outliers;
		}
	}
    CATCH;
}

std::string ksi::exp_327::make_output_name(const std::filesystem::path& input, int granules, int iterations, const std::string& suffix)
{
    return input.stem().string()
        + "_g" + std::to_string(granules)
        + "_it" + std::to_string(iterations)
        + "_thr" + std::to_string(this->threshold)
        + "_" + suffix
        + this->extention;
}

std::unordered_set<const ksi::datum*> ksi::exp_327::collect_pointers(const ksi::dataset& ds)
{
    std::unordered_set<const ksi::datum*> ptrs;
    for (std::size_t i = 0; i < ds.size(); ++i)
        ptrs.insert(ds.getDatum(i));
    return ptrs;
}

ksi::dataset ksi::exp_327::extract_outliers(const ksi::dataset& original, const ksi::dataset& cleaned)
{
    auto original_ptrs = collect_pointers(original);
    auto cleaned_ptrs = collect_pointers(cleaned);

    ksi::dataset outliers;

    for (const auto* d : original_ptrs)
    {
        if (cleaned_ptrs.find(d) == cleaned_ptrs.end())
        {
            outliers.addDatum(*d);  // copy the datum to the outliers dataset
        }
    }

    return outliers;
}