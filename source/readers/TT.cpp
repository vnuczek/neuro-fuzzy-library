#include "TT.h"

ksi::TT::Iterator ksi::TT::Iterator::operator++(int)
{
    Iterator temp = *this;
    ++(*this);
    return temp;
}

bool ksi::TT::Iterator::operator!=(const Iterator& other) const
{
    return !(*this == other);
}

ksi::TT::ConstIterator ksi::TT::ConstIterator::operator++(int)
{
    ConstIterator temp = *this;
    ++(*this);
    return temp;
}

bool ksi::TT::ConstIterator::operator!=(const ConstIterator& other) const
{
    return !(*this == other);
}