/** @file */

#include "../readers/train_test_model.h"
#include "../service/exception.h"
#include "../auxiliary/to_string.h"

#include <filesystem>
#include <thread>
#include <fstream>
#include <ranges>
#include <regex>

ksi::train_test_model::train_test_model(ksi::reader& source_reader)
	: cross_validation_model(source_reader) {}

ksi::train_test_model::train_test_model(const train_test_model& other)
	: cross_validation_model(other) {}

ksi::train_test_model::train_test_model(train_test_model&& other) noexcept
	: cross_validation_model(std::move(other)) {}

ksi::train_test_model& ksi::train_test_model::operator=(const train_test_model& other)
{
    if (this != &other)
    {
        cross_validation_model::operator=(other);
    }
    return *this;
}

ksi::train_test_model& ksi::train_test_model::operator=(train_test_model&& other) noexcept
{
    if (this != &other)
    {
        cross_validation_model::operator=(std::move(other));
    }
    return *this;
}

void ksi::train_test_model::split(const ksi::dataset & base_dataset, const unsigned int n)
{
    try
    {
        if (n < 2)
        {
            throw ksi::exception("Number of subsets must be grater than (1).");
        }

        const auto total_size = base_dataset.size();
        if (n > total_size)
        {
            throw ksi::exception("Number of subsets (" + std::to_string(n) + ") cannot be greater than the number of data points (" + std::to_string(total_size) + ").");
        }

        datasets.clear();
        datasets.resize(n);

        const auto base_size = total_size / n;
        const auto remainder = total_size % n;

        std::size_t index = 0;
        for (int i = 0; i < n; ++i)
        {
            const auto current_size = base_size + (i < remainder ? 1 : 0);

            for (auto j = 0; j < current_size; ++j)
            {
                datasets[i].addDatum(*base_dataset.getDatum(index));
                ++index;
            }
        }
    }
    CATCH;
}

void ksi::train_test_model::save(const std::filesystem::path& directory, const std::filesystem::path& filename, const std::filesystem::path& extension, const bool overwrite) const
{
    try  
    {
        std::filesystem::create_directories(directory);
        const auto num_files = datasets.size();
        const auto num_digits = std::to_string(num_files).length();

        for (auto i = 0; i < datasets.size(); ++i)
        {
            auto file_path = directory / (filename.string() + "_" + ksi::to_string(i, num_digits) + extension.string());
            if (std::filesystem::exists(file_path) && !overwrite)
            {
                throw ksi::exception("File " + file_path.string() + " already exists. To overwrite, set the overwrite parameter to true.");
            }

            std::ofstream file(file_path);

            if (file.is_open())
            {
                for (auto j = 0; j < datasets[i].size(); ++j)
                {
                    const datum* d = datasets[i].getDatum(j);
                    if (d)
                    {
                        d->save_print(file);
                    }
                }
                file.close();
            }
            else
            {
                throw ksi::exception("Unable to open file " + file_path.string());
            }
        }
    }
    CATCH; 
}

ksi::dataset ksi::train_test_model::read_file(const std::filesystem::path& file_directory)
{
    return pReader->read(file_directory.string());
}

void ksi::train_test_model::read_directory(const std::filesystem::path& directory, const std::string& file_regex_pattern)
{
    std::regex data_file_regex(file_regex_pattern);
    auto data_files = std::filesystem::directory_iterator(directory)
        | std::views::filter([](const auto& entry) { return entry.is_regular_file(); })
        | std::views::filter([&data_file_regex](const auto& entry)
            {
                return std::regex_match(entry.path().string(), data_file_regex);
            });

    std::vector<std::filesystem::path> filtered_files;
    for (const auto& file : data_files) {
        filtered_files.push_back(file.path());
    }

    std::vector<std::thread> threads;
    for (const auto& file_path : filtered_files) {
        threads.emplace_back([this, file_path]()
            {
                ksi::dataset ds = read_file(file_path);

                std::lock_guard<std::mutex> lock(this->datasets_mutex);
                this->datasets.push_back(std::move(ds));
            });
    }

    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

std::shared_ptr<ksi::reader> ksi::train_test_model::clone() const
{
    return std::shared_ptr<ksi::reader>(new ksi::train_test_model(*this));
}

ksi::dataset ksi::train_test_model::read(const std::string& filename)
{
    return pReader->read(filename);
}

void ksi::train_test_model::read_and_split_file(const std::filesystem::path& file_path, const unsigned int n)
{
    dataset base_dataset = read_file(file_path);
    split(base_dataset, n);
}

auto ksi::train_test_model::begin() -> ksi::train_test_model::iterator
{
    return { this, datasets.begin() };
}

auto ksi::train_test_model::end() -> ksi::train_test_model::iterator
{
    return { this, datasets.end() };
}

auto ksi::train_test_model::cbegin() const -> ksi::train_test_model::const_iterator
{
    return { this, datasets.cbegin() };
}

auto ksi::train_test_model::cend() const -> ksi::train_test_model::const_iterator
{
    return { this, datasets.cend() };
}

ksi::train_test_model::iterator::iterator(train_test_model* tt, std::vector<ksi::dataset>::iterator test_it)
	: pTT(tt), test_iterator(test_it)
{
    initialize_test_dataset();
}

ksi::train_test_model::iterator::iterator(const iterator& other)
	: pTT(other.pTT), test_iterator(other.test_iterator), train_dataset(other.train_dataset) {}

ksi::train_test_model::iterator::iterator(iterator&& other) noexcept
	: pTT(other.pTT), test_iterator(std::move(other.test_iterator)), train_dataset(std::move(other.train_dataset)) {}

ksi::train_test_model::iterator& ksi::train_test_model::iterator::operator=(const iterator& other)
{
    if (this != &other)
    {
        pTT = other.pTT;
        test_iterator = other.test_iterator;
        train_dataset = other.train_dataset;
    }
    return *this;
}

ksi::train_test_model::iterator& ksi::train_test_model::iterator::operator=(iterator&& other) noexcept
{
    if (this != &other)
    {
        pTT = std::move(pTT);
        test_iterator = std::move(other.test_iterator);
        train_dataset = std::move(other.train_dataset);
    }
    return *this;
}

ksi::train_test_model::iterator& ksi::train_test_model::iterator::operator++()
{
    ++test_iterator;
    initialize_test_dataset();
    return *this;
}

ksi::train_test_model::iterator ksi::train_test_model::iterator::operator++(int)
{
    iterator temp = *this;
    ++(*this);
    return temp;
}

bool ksi::train_test_model::iterator::operator==(const iterator& other) const
{
    return test_iterator == other.test_iterator;
}

std::strong_ordering ksi::train_test_model::iterator::operator<=>(const iterator& other) const
{
    return test_iterator <=> other.test_iterator;
}

std::tuple<ksi::dataset, ksi::dataset> ksi::train_test_model::iterator::operator*() const
{
    return std::make_tuple(train_dataset, *test_iterator);
}

void ksi::train_test_model::iterator::initialize_test_dataset()
{
    train_dataset = ksi::dataset();

    for (auto it = pTT->datasets.cbegin(); it != pTT->datasets.cend(); it++) {
        if (it != test_iterator) {
            for (std::size_t j = 0; j < it->size(); ++j) {
                train_dataset.addDatum(*it->getDatum(j));
            }
        }
    }
}

ksi::train_test_model::const_iterator::const_iterator(const train_test_model* tt, std::vector<ksi::dataset>::const_iterator test_it)
	: pTT(tt), test_iterator(test_it)
{
    initialize_test_dataset();
}

ksi::train_test_model::const_iterator::const_iterator(const const_iterator& other)
    : pTT(other.pTT), test_iterator(other.test_iterator), train_dataset(other.train_dataset) {}

ksi::train_test_model::const_iterator::const_iterator(const_iterator&& other) noexcept
    : pTT(other.pTT), test_iterator(std::move(other.test_iterator)), train_dataset(std::move(other.train_dataset)) {}

ksi::train_test_model::const_iterator& ksi::train_test_model::const_iterator::operator=(const const_iterator& other)
{
    if (this != &other)
    {
        pTT = other.pTT;
        test_iterator = other.test_iterator;
        train_dataset = other.train_dataset;
    }
    return *this;
}

ksi::train_test_model::const_iterator& ksi::train_test_model::const_iterator::operator=(const_iterator&& other) noexcept
{
    if (this != &other)
    {
        pTT = std::move(pTT);
        test_iterator = std::move(other.test_iterator);
        train_dataset = std::move(other.train_dataset);
    }
    return *this;
}

ksi::train_test_model::const_iterator& ksi::train_test_model::const_iterator::operator++()
{
    ++test_iterator;
    initialize_test_dataset();
    return *this;
}

ksi::train_test_model::const_iterator ksi::train_test_model::const_iterator::operator++(int)
{
    const_iterator temp = *this;
    ++(*this);
    return temp;
}

bool ksi::train_test_model::const_iterator::operator==(const ksi::train_test_model::const_iterator& other) const
{
    return test_iterator == other.test_iterator;
}

std::strong_ordering ksi::train_test_model::const_iterator::operator<=>(const ksi::train_test_model::const_iterator& other) const
{
    return test_iterator <=> other.test_iterator;
}

std::tuple<const ksi::dataset&, const ksi::dataset&> ksi::train_test_model::const_iterator::operator*() const
{
    return std::make_tuple(std::ref(train_dataset), std::ref(*test_iterator));
}

void ksi::train_test_model::const_iterator::initialize_test_dataset()
{
    train_dataset = ksi::dataset();

    for (auto it = pTT->datasets.cbegin(); it != pTT->datasets.cend(); it++) {
        if (it != test_iterator) {
            for (std::size_t j = 0; j < it->size(); ++j) {
                train_dataset.addDatum(*it->getDatum(j));
            }
        }
    }
}
