/** @file */

#include "../readers/TVT.h"
#include "../service/exception.h"
#include "../auxiliary/to_string.h"

#include <thread>
#include <fstream>
#include <ranges>
#include <regex>

ksi::TVT::TVT(ksi::reader& source_reader)
	: CV(source_reader) {}

ksi::TVT::TVT(ksi::reader& reader, int validation_dataset_size)
    : CV(reader), validation_size(validation_dataset_size) {}

ksi::TVT::TVT(const TVT& other)
	: CV(other) {}

ksi::TVT::TVT(TVT&& other) noexcept
	: CV(std::move(other)) {}

ksi::TVT& ksi::TVT::operator=(const TVT& other)
{
    if (this != &other)
    {
        CV::operator=(other);
    }
    return *this;
}

ksi::TVT& ksi::TVT::operator=(TVT&& other) noexcept
{
    if (this != &other)
    {
        CV::operator=(std::move(other));
    }
    return *this;
}

void ksi::TVT::split(const ksi::dataset& base_dataset, const int n)
{
    datasets.clear();
    datasets.resize(n);

    const auto total_size = base_dataset.size();
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

void ksi::TVT::save(const std::filesystem::path& directory) const
{
    try
    {
        std::filesystem::create_directories(directory);

        for (auto i = 0; i < datasets.size(); ++i)
        {
            std::ofstream file(directory / ("dataset_" + ksi::to_string(i, 2) + ".data"));

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
                throw ksi::exception("Unable to open file " + (directory / ("dataset_" + std::to_string(i) + ".data")).string());
            }
        }
    }
    CATCH;
}

ksi::dataset ksi::TVT::read_file(const std::filesystem::path& file_directory)
{
    return pReader->read(file_directory.string());
}

void ksi::TVT::read_directory(const std::filesystem::path& directory)
{
    std::regex data_file_regex(".*\\.data$");
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

std::shared_ptr<ksi::reader> ksi::TVT::clone() const
{
    return std::shared_ptr<ksi::reader>(new ksi::TVT(*this));
}

ksi::dataset ksi::TVT::read(const std::string& filename)
{
    return pReader->read(filename);
}

auto ksi::TVT::begin() -> ksi::TVT::iterator
{
    return { this, datasets.begin() };
}

auto ksi::TVT::end() -> ksi::TVT::iterator
{
    return { this, datasets.end() };
}

auto ksi::TVT::cbegin() const -> ksi::TVT::const_iterator
{
    return { this, datasets.cbegin() };
}

auto ksi::TVT::cend() const -> ksi::TVT::const_iterator
{
    return { this, datasets.cend() };
}

ksi::TVT::iterator::iterator(TVT* tvt, std::vector<ksi::dataset>::iterator test_it)
    : pTVT(tvt), test_iterator(test_it)
{
    initialize_train_and_validation_datasets();
}

ksi::TVT::iterator::iterator(const iterator& other)
    : pTVT(other.pTVT), test_iterator(other.test_iterator), validation_dataset(other.validation_dataset), train_dataset(other.train_dataset) {}

ksi::TVT::iterator::iterator(iterator&& other) noexcept
    : pTVT(other.pTVT), test_iterator(std::move(other.test_iterator)), validation_dataset(std::move(other.validation_dataset)), train_dataset(std::move(other.train_dataset)) {}

ksi::TVT::iterator& ksi::TVT::iterator::operator=(const iterator& other)
{
    if (this != &other)
    {
        pTVT = other.pTVT;
        test_iterator = other.test_iterator;
        validation_dataset = other.validation_dataset;
        train_dataset = other.train_dataset;
    }
    return *this;
}

ksi::TVT::iterator& ksi::TVT::iterator::operator=(iterator&& other) noexcept
{
    if (this != &other)
    {
        pTVT = std::move(other.pTVT);
        test_iterator = std::move(other.test_iterator);
        validation_dataset = std::move(other.validation_dataset);
        train_dataset = std::move(other.train_dataset);
    }
    return *this;
}

ksi::TVT::iterator& ksi::TVT::iterator::operator++()
{
    ++test_iterator;
    initialize_train_and_validation_datasets();
    return *this;
}

ksi::TVT::iterator ksi::TVT::iterator::operator++(int)
{
    iterator temp = *this;
    ++(*this);
    return temp;
}

bool ksi::TVT::iterator::operator==(const iterator& other) const
{
    return test_iterator == other.test_iterator;
}

bool ksi::TVT::iterator::operator!=(const iterator& other) const
{
    return !(*this == other);
}

std::strong_ordering ksi::TVT::iterator::operator<=>(const iterator& other) const
{
    return test_iterator <=> other.test_iterator;
}

std::tuple<ksi::dataset, ksi::dataset, ksi::dataset> ksi::TVT::iterator::operator*() const
{
    return std::make_tuple(train_dataset, validation_dataset, *test_iterator);
}

std::tuple<ksi::dataset, ksi::dataset, ksi::dataset> ksi::TVT::iterator::operator->() const
{
    return std::make_tuple(train_dataset, validation_dataset, *test_iterator);
}

void ksi::TVT::iterator::initialize_train_and_validation_datasets()
{
    auto total_datasets = pTVT->datasets.size();
    auto validation_start_index = std::distance(pTVT->datasets.begin(), test_iterator) + 1;
    auto current_validation_count = 0;
    
    auto current_iterator = pTVT->datasets.begin();
    std::advance(current_iterator, validation_start_index % total_datasets);

    for (auto it = pTVT->datasets.begin(); it != pTVT->datasets.end(); ++it) {
        if (it == test_iterator) {
            continue;
        }

        if (current_validation_count < pTVT->validation_size && it == current_iterator) {
            for (std::size_t j = 0; j < it->size(); ++j) {
                validation_dataset.addDatum(*it->getDatum(j));
            }
            ++current_validation_count;
            ++current_iterator;
            if (current_iterator == pTVT->datasets.end()) {
                current_iterator = pTVT->datasets.begin();
            }
        }
        else {
            for (std::size_t j = 0; j < it->size(); ++j) {
                train_dataset.addDatum(*it->getDatum(j));
            }
        }
    }
}

ksi::TVT::const_iterator::const_iterator(const TVT* tvt, std::vector<ksi::dataset>::const_iterator test_it)
    : pTVT(tvt), test_iterator(test_it)
{
    initialize_train_and_validation_datasets();
}

ksi::TVT::const_iterator::const_iterator(const const_iterator& other)
    : pTVT(other.pTVT), test_iterator(other.test_iterator), validation_dataset(other.validation_dataset), train_dataset(other.train_dataset) {}

ksi::TVT::const_iterator::const_iterator(const_iterator&& other) noexcept
    : pTVT(other.pTVT), test_iterator(std::move(other.test_iterator)), validation_dataset(std::move(other.validation_dataset)), train_dataset(std::move(other.train_dataset)) {}

ksi::TVT::const_iterator& ksi::TVT::const_iterator::operator=(const const_iterator& other)
{
    if (this != &other)
    {
        pTVT = other.pTVT;
        test_iterator = other.test_iterator;
        validation_dataset = other.validation_dataset;
        train_dataset = other.train_dataset;
    }
    return *this;
}

ksi::TVT::const_iterator& ksi::TVT::const_iterator::operator=(const_iterator&& other) noexcept
{
    if (this != &other)
    {
        pTVT = std::move(other.pTVT);
        test_iterator = std::move(other.test_iterator);
        validation_dataset = std::move(other.validation_dataset);
        train_dataset = std::move(other.train_dataset);
    }
    return *this;
}

ksi::TVT::const_iterator& ksi::TVT::const_iterator::operator++()
{
    ++test_iterator;
    initialize_train_and_validation_datasets();
    return *this;
}

ksi::TVT::const_iterator ksi::TVT::const_iterator::operator++(int)
{
    const_iterator temp = *this;
    ++(*this);
    return temp;
}

bool ksi::TVT::const_iterator::operator==(const const_iterator& other) const
{
    return test_iterator == other.test_iterator;
}

bool ksi::TVT::const_iterator::operator!=(const const_iterator& other) const
{
    return !(*this == other);
}

std::strong_ordering ksi::TVT::const_iterator::operator<=>(const const_iterator& other) const
{
    return test_iterator <=> other.test_iterator;
}

std::tuple<const ksi::dataset&, const ksi::dataset&, const ksi::dataset&> ksi::TVT::const_iterator::operator*() const
{
    return std::make_tuple(std::ref(train_dataset), std::ref(validation_dataset), std::ref(*test_iterator));
}

std::tuple<const ksi::dataset&, const ksi::dataset&, const ksi::dataset&> ksi::TVT::const_iterator::operator->() const
{
    return std::make_tuple(std::ref(train_dataset), std::ref(validation_dataset), std::ref(*test_iterator));
}

void ksi::TVT::const_iterator::initialize_train_and_validation_datasets()
{
    train_dataset = ksi::dataset();
    validation_dataset = ksi::dataset();
    
    auto total_datasets = pTVT->datasets.size();
    auto validation_start_index = std::distance(pTVT->datasets.cbegin(), test_iterator) + 1;
    auto current_validation_count = 0;
    
    auto current_iterator = pTVT->datasets.cbegin();
    std::advance(current_iterator, validation_start_index % total_datasets);

    for (auto it = pTVT->datasets.cbegin(); it != pTVT->datasets.cend(); ++it) {
        if (it == test_iterator) {
            continue;
        }

        if (current_validation_count < pTVT->validation_size && it == current_iterator) {
            for (std::size_t j = 0; j < it->size(); ++j) {
                validation_dataset.addDatum(*it->getDatum(j));
            }
            ++current_validation_count;
            ++current_iterator;
            if (current_iterator == pTVT->datasets.cend()) {
                current_iterator = pTVT->datasets.cbegin();
            }
        }
        else {
            for (std::size_t j = 0; j < it->size(); ++j) {
                train_dataset.addDatum(*it->getDatum(j));
            }
        }
    }
}