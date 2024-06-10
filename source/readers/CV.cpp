/** @file */

#include "CV.h"

ksi::CV::CV(ksi::reader& source_reader)
	: pReader(source_reader.clone()) {}

ksi::CV::CV(const CV& other)
	: pReader(other.pReader)
{
    std::lock_guard<std::mutex> lock(other.datasets_mutex);
    datasets = other.datasets;
}

ksi::CV::CV(CV&& other) noexcept
	: pReader(std::move(other.pReader))
{
    std::lock_guard<std::mutex> lock(other.datasets_mutex);
    datasets = std::move(other.datasets);
}

ksi::CV& ksi::CV::operator=(const CV& other)
{
    if (this != &other)
    {
        pReader = other.pReader;
        std::lock_guard<std::mutex> lock(other.datasets_mutex);
        datasets = other.datasets;
    }
    return *this;
}

ksi::CV& ksi::CV::operator=(CV&& other) noexcept
{
    if (this != &other)
    {
        pReader = std::move(other.pReader);
        std::lock_guard<std::mutex> lock(other.datasets_mutex);
        datasets = std::move(other.datasets);
    }
    return *this;
}
