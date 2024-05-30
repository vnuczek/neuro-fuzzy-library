#ifndef CV_H
#define CV_H

#include "reader.h"

#include <filesystem>

namespace ksi
{
	class CV : public reader {
    public:
        virtual ~CV() = default;
        
        virtual void split(const int ncons = 10) = 0;
        virtual void save(const std::filesystem::path& directory) const = 0;
        virtual dataset read_file(const std::filesystem::path& file_name) = 0;
        virtual dataset read_directory(const std::filesystem::path& directory) = 0;
    };
}

#endif
