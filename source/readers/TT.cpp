/** @file */

#include "../readers/TT.h"
#include "../service/exception.h"
#include "../auxiliary/to_string.h"

#include <thread>
#include <fstream>
#include <ranges>
#include <regex>

ksi::TT::TT(ksi::reader& source_reader)
	: CV(source_reader) {}

ksi::TT::TT(const TT& other)
	: CV(other) {}

ksi::TT::TT(TT&& other) noexcept
	: CV(std::move(other)) {}

ksi::TT& ksi::TT::operator=(const TT& other)
{
    if (this != &other)
    {
        CV::operator=(other);
    }
    return *this;
}

ksi::TT& ksi::TT::operator=(TT&& other) noexcept
{
    if (this != &other)
    {
        CV::operator=(std::move(other));
    }
    return *this;
}

void ksi::TT::split(const ksi::dataset & base_dataset, const int n)
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

void ksi::TT::save(const std::filesystem::path& directory) const
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

ksi::dataset ksi::TT::read_file(const std::filesystem::path& file_directory)
{
    return pReader->read(file_directory.string());
}

void ksi::TT::read_directory(const std::filesystem::path& directory)
{
    /** @todo Pliki nie muszą mieć koniecznie rozszerzenia *.data.
     *        Lepiej byłoby, żeby przekazywać, jako parametr wzorzec nazwy pliku,
     *        np. domyślnie ".*", chyba że użytkownik poda swój wzorzec.
     */ 
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

std::shared_ptr<ksi::reader> ksi::TT::clone() const
{
    return std::shared_ptr<ksi::reader>(new ksi::TT(*this));
}

ksi::dataset ksi::TT::read(const std::string& filename)
{
    return pReader->read(filename);
}

auto ksi::TT::begin() -> ksi::TT::iterator
{
    return { this, datasets.begin() };
}

auto ksi::TT::end() -> ksi::TT::iterator
{
    return { this, datasets.end() };
}

auto ksi::TT::cbegin() const -> ksi::TT::const_iterator
{
    return { this, datasets.cbegin() };
}

auto ksi::TT::cend() const -> ksi::TT::const_iterator
{
    return { this, datasets.cend() };
}

ksi::TT::iterator::iterator(TT* tt, std::vector<ksi::dataset>::iterator test_it)
	: pTT(tt), test_iterator(test_it)
{
    initialize_test_dataset();
}

ksi::TT::iterator::iterator(const iterator& other)
	: pTT(other.pTT), test_iterator(other.test_iterator), train_dataset(other.train_dataset) {}

ksi::TT::iterator::iterator(iterator&& other) noexcept
	: pTT(other.pTT), test_iterator(std::move(other.test_iterator)), train_dataset(std::move(other.train_dataset)) {}

ksi::TT::iterator& ksi::TT::iterator::operator=(const iterator& other)
{
    if (this != &other)
    {
        pTT = other.pTT;
        test_iterator = other.test_iterator;
        train_dataset = other.train_dataset;
    }
    return *this;
}

ksi::TT::iterator& ksi::TT::iterator::operator=(iterator&& other) noexcept
{
    if (this != &other)
    {
        pTT = std::move(pTT);
        test_iterator = std::move(other.test_iterator);
        train_dataset = std::move(other.train_dataset);
    }
    return *this;
}

ksi::TT::iterator& ksi::TT::iterator::operator++()
{
    ++test_iterator;
    initialize_test_dataset();
    return *this;
}

ksi::TT::iterator ksi::TT::iterator::operator++(int)
{
    iterator temp = *this;
    ++(*this);
    return temp;
}

bool ksi::TT::iterator::operator==(const iterator& other) const
{
    return test_iterator == other.test_iterator;
}

bool ksi::TT::iterator::operator!=(const iterator& other) const
{
    return !(*this == other);
}

std::strong_ordering ksi::TT::iterator::operator<=>(const iterator& other) const
{
    return test_iterator <=> other.test_iterator;
}

std::tuple<ksi::dataset, ksi::dataset> ksi::TT::iterator::operator*() const
{
    return std::make_tuple(train_dataset, *test_iterator);
}

std::tuple<ksi::dataset, ksi::dataset> ksi::TT::iterator::operator->() const
{
    return std::make_tuple(train_dataset, *test_iterator);
}

void ksi::TT::iterator::initialize_test_dataset()
{
    train_dataset = ksi::dataset();

    for (auto it = pTT->datasets.cbegin(); it != pTT->datasets.cend(); ++it) {
        if (it != test_iterator) {
            for (std::size_t j = 0; j < it->size(); ++j) {
                train_dataset.addDatum(*it->getDatum(j));
            }
        }
    }
}

ksi::TT::const_iterator::const_iterator(const TT* tt, std::vector<ksi::dataset>::const_iterator test_it)
	: pTT(tt), test_iterator(test_it)
{
    initialize_test_dataset();
}

ksi::TT::const_iterator::const_iterator(const const_iterator& other)
    : pTT(other.pTT), test_iterator(other.test_iterator), train_dataset(other.train_dataset) {}

ksi::TT::const_iterator::const_iterator(const_iterator&& other) noexcept
    : pTT(other.pTT), test_iterator(std::move(other.test_iterator)), train_dataset(std::move(other.train_dataset)) {}

ksi::TT::const_iterator& ksi::TT::const_iterator::operator=(const const_iterator& other)
{
    if (this != &other)
    {
        pTT = other.pTT;
        test_iterator = other.test_iterator;
        train_dataset = other.train_dataset;
    }
    return *this;
}

ksi::TT::const_iterator& ksi::TT::const_iterator::operator=(const_iterator&& other) noexcept
{
    if (this != &other)
    {
        pTT = std::move(pTT);
        test_iterator = std::move(other.test_iterator);
        train_dataset = std::move(other.train_dataset);
    }
    return *this;
}

ksi::TT::const_iterator& ksi::TT::const_iterator::operator++()
{
    ++test_iterator;
    initialize_test_dataset();
    return *this;
}

ksi::TT::const_iterator ksi::TT::const_iterator::operator++(int)
{
    const_iterator temp = *this;
    ++(*this);
    return temp;
}

bool ksi::TT::const_iterator::operator==(const ksi::TT::const_iterator& other) const
{
    return test_iterator == other.test_iterator;
}

bool ksi::TT::const_iterator::operator!=(const const_iterator& other) const
{
    return !(*this == other);
}

std::strong_ordering ksi::TT::const_iterator::operator<=>(const ksi::TT::const_iterator& other) const
{
    return test_iterator <=> other.test_iterator;
}

std::tuple<const ksi::dataset&, const ksi::dataset&> ksi::TT::const_iterator::operator*() const
{
    return std::make_tuple(std::ref(train_dataset), std::ref(*test_iterator));
}

std::tuple<const ksi::dataset&, const ksi::dataset&> ksi::TT::const_iterator::operator->() const
{
    return std::make_tuple(std::ref(train_dataset), std::ref(*test_iterator));
}

void ksi::TT::const_iterator::initialize_test_dataset()
{
    train_dataset = ksi::dataset();

    for (auto it = pTT->datasets.cbegin(); it != pTT->datasets.cend(); ++it) {
        if (it != test_iterator) {
            for (std::size_t j = 0; j < it->size(); ++j) {
                train_dataset.addDatum(*it->getDatum(j));
            }
        }
    }
}
