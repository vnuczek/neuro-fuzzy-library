#include "TVT.h"

ksi::TVT::Iterator ksi::TVT::Iterator::operator++(int)
{
    Iterator temp = *this;
    ++(*this);
    return temp;
}

bool ksi::TVT::Iterator::operator!=(const Iterator& other) const
{
    return !(*this == other);
}

ksi::TVT::ConstIterator ksi::TVT::ConstIterator::operator++(int)
{
    ConstIterator temp = *this;
    ++(*this);
    return temp;
}

bool ksi::TVT::ConstIterator::operator!=(const ConstIterator& other) const
{
    return !(*this == other);
}
