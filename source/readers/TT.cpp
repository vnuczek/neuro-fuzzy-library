#include "TT.h"

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