/** @file */

#include "../experiments/exp-327.h"

#include <filesystem>
#include <thread>
#include <fstream>

#include "../service/exception.h"
#include "../readers/reader-complete.h"
#include "../partitions/fcm.h"
#include "../partitions/partition.h"
#include "../partitions/cluster.h"
#include "../descriptors/descriptor.h"
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
		std::string outputDir = "../results/" + this->name;
        std::filesystem::create_directories(outputDir);

        ksi::reader_complete reader;
        const auto originalData = reader.read(filePath.string());

        process_granular(outputDir, filePath, originalData);
        process_sigma(outputDir, filePath, originalData);
	}
    CATCH;
}

void ksi::exp_327::process_granular(const std::string& outputDir, const std::filesystem::path& filePath, const ksi::dataset& originalData)
{
    try
    {
        for (const int g : this->granules)
        {
            for (const int it : this->iterations)
            {
                for (const auto th : this->thresholds)
                {
                    auto data = originalData;

                    ksi::fcm partitioner(g, it);
                    ksi::data_modifier_outlier_remove_granular remover(
                        partitioner,
                        ksi::t_norm_min(),
                        ksi::s_norm_max(),
                        th
                    );
                    remover.modify(data);

                    auto part = remover.get_partition();
                    save_granular_results(outputDir, filePath, g, it, th, originalData, data, part);
                }
            }
        }
    }
    CATCH;
}

void ksi::exp_327::save_granular_results(
    const std::string& outputDir,
    const std::filesystem::path& filePath,
    int g, int it, double th,
    const ksi::dataset& originalData,
    ksi::dataset& data,
    const ksi::partition& part)
{
    try
    {
        {
            std::ofstream cleaned_file(
                outputDir + "/" + make_granular_output_name(filePath, g, it, th, "cleaned")
            );
            cleaned_file << data;
        }

        ksi::dataset outliers = extract_outliers(originalData, data);
        {
            std::ofstream outliers_file(
                outputDir + "/" + make_granular_output_name(filePath, g, it, th, "outliers")
            );
            outliers_file << outliers;
        }

        {
            std::string granulesPath = outputDir + "/" + make_granular_output_name(filePath, g, it, th, "granules");
            save_granules(granulesPath, part);
        }
    }
    CATCH;
}

void ksi::exp_327::save_granules(const std::string& filePath, const ksi::partition& part)
{
    try
    {
        std::ofstream file(filePath);

        auto nClusters = part.getNumberOfClusters();
        for (std::size_t c = 0; c < nClusters; ++c)
        {
            auto* cl = part.getCluster(c);
            if (!cl)
                continue;

            auto nDescriptors = cl->get_number_of_desciptors();
            for (std::size_t d = 0; d < nDescriptors; ++d)
            {
                auto* desc = cl->getAddressOfDescriptor(d);
                if (!desc)
                    continue;

                auto params = desc->getParameters();
                for (std::size_t p = 0; p < params.size(); ++p)
                {
                    if (p > 0)
                        file << "\t";
                    file << params[p];
                }
                file << "\t";
            }
            file << "\n";
        }
    }
    CATCH;
}

void ksi::exp_327::process_sigma(const std::string& outputDir, const std::filesystem::path& filePath, const ksi::dataset& originalData)
{
    try
    {
        for (const auto n : this->sigmas)
        {
            auto data = originalData;

            ksi::data_modifier_outlier_remove_sigma remover(n);
            remover.modify(data);
            {
                std::ofstream cleaned_file(
                    outputDir + "/" + make_sigma_output_name(filePath, n, "cleaned")
                );
                cleaned_file << data;
            }

            ksi::dataset outliers = extract_outliers(originalData, data);
            {
                std::ofstream outliers_file(
                    outputDir + "/" + make_sigma_output_name(filePath, n, "outliers")
                );
                outliers_file << outliers;
            }
        }
    }
    CATCH;
}

std::string ksi::exp_327::make_granular_output_name(const std::filesystem::path& input, const int granules, const int iterations, const double threshold, const std::string& suffix)
{
    return input.stem().string()
        + "_g" + std::to_string(granules)
        + "_it" + std::to_string(iterations)
        + "_thr" + std::to_string(threshold)
        + "_" + suffix
        + this->extention;
}

std::string ksi::exp_327::make_sigma_output_name(const std::filesystem::path& input, const int n, const std::string& suffix)
{
    return input.stem().string()
        + "_n" + std::to_string(n)
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