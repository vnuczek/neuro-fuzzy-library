/** @file */

#include "TT.h"
#include "../service/exception.h"

#include <fstream>
#include <ranges>
#include <regex>

void ksi::TT::split(const ksi::dataset & dataset, const int n)
{
    datasets.clear();
    datasets.resize(n);

    const auto total_size = dataset.size();
    const auto base_size = total_size / n;
    const auto remainder = total_size % n;

    std::size_t index = 0;
    for (auto i = 0; i < n; ++i)
    {
        const auto current_size = base_size + (i < remainder ? 1 : 0);

        for (auto j = 0; j < current_size; ++j)
        {
            datasets[i].addDatum(*dataset.getDatum(index));
            ++index;
        }
    }
}

void ksi::TT::save(const std::filesystem::path& directory) const
{
    try  
    {
        std::filesystem::create_directories(directory);

        for (auto i = 0; i < datasets.size(); ++i)
        {
            std::ofstream file(directory / ("dataset_" + std::to_string(i) + ".data"));   // ksi::to_string (i, 2)    i == 1 -->  "01" 

            if (file.is_open())
            {
                for (auto j = 0; j < datasets[i].size(); ++j)
                {
                    const datum* d = datasets[i].getDatum(j);
                    if (d)
                    {
                        file << *d << std::endl;   // KS: Czy na pewno zadziała dobrze? Cos w stylu save_print?
                    }
                }
                file.close();
            }
            else
            {
                throw ksi::exception("Unable to open file " + (directory / ("dataset_" + std::to_string(i) + ".data")).string());
            }
        }
    }
    CATCH; 
}

ksi::dataset ksi::TT::read_file(const std::filesystem::path& file_directory)
{
    return pReader->read(file_directory.string());
}

void ksi::TT::read_directory(const std::filesystem::path& directory)
{
    std::regex data_file_regex(".*\\.data");  // KS: Czy na pewno takie wyrażenie?

    const auto data_files = std::filesystem::directory_iterator(directory)
        | std::views::filter([](const auto& entry) { return entry.is_regular_file(); })
        | std::views::filter([&data_file_regex](const auto& entry)
            {
                return std::regex_match(entry.path().string(), data_file_regex);
            });

    std::vector<std::thread> threads;

    for (const auto& entry : data_files)
    {
        threads.emplace_back([this, file_path = entry.path()]()
            {
                ksi::dataset ds = read_file(file_path);

                std::lock_guard<std::mutex> lock(this->datasets_mutex);
                this->datasets.push_back(std::move(ds));
            });
    }

    for (auto& thread : threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

ksi::TT::iterator ksi::TT::iterator::operator++(int)
{
    iterator temp = *this;
    ++(*this);
    return temp;
}

bool ksi::TT::iterator::operator!=(const iterator& other) const
{
    return !(*this == other);
}

// std::tuple<ksi::dataset, ksi::dataset> ksi::TT:iterator:: itd
std::tuple<ksi::dataset&, std::vector<ksi::dataset>>& ksi::TT::iterator::operator*() const {
    std::vector<ksi::dataset> test;
    for (auto i = tt.datasets.begin(); i != tt.datasets.end(); ++i) {
        if (i != it) {
            test.push_back(*i);
        }
    }
    return std::make_tuple(std::ref(*it), test);
}

ksi::TT::const_iterator ksi::TT::const_iterator::operator++(int)
{
    const_iterator temp = *this;
    ++(*this);
    return temp;
}

bool ksi::TT::const_iterator::operator!=(const const_iterator& other) const
{
    return !(*this == other);
}
