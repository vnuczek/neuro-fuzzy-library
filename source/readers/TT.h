#ifndef TT_H
#define TT_H

#include "CV.h"

namespace ksi {
    class TT : public CV {

        class Iterator;
        class ConstIterator;

        std::shared_ptr<reader> clone() const override;

        void split(int ncons = 10) override;

        void save(const std::filesystem::path& directory) const override;

        dataset read_file(const std::filesystem::path& file_name) override;

        dataset read_directory(const std::filesystem::path& directory) override;

        Iterator begin();

        Iterator end();

        ConstIterator begin() const;

        ConstIterator end() const;
    };

    class TT::Iterator {
    public:

        Iterator();

        Iterator& operator++();

        Iterator operator++(int);

        bool operator==(const Iterator& other) const;

        bool operator!=(const Iterator& other) const;

        auto operator<=>(const Iterator& other) const;

        TT& operator*() const;

        TT* operator->() const;

    };

    class TT::ConstIterator {
    public:

        ConstIterator();

        ConstIterator& operator++();

        ConstIterator operator++(int);

        bool operator==(const ConstIterator& other) const;

        bool operator!=(const ConstIterator& other) const;

        auto operator<=>(const ConstIterator& other) const;

        TT& operator*() const;

        TT* operator->() const;

    };
}

#endif

