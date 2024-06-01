#include "TVT.h"

ksi::TVT::iterator ksi::TVT::iterator::operator++(int)
{
    iterator temp = *this;
    ++(*this);
    return temp;
}

bool ksi::TVT::iterator::operator!=(const iterator& other) const
{
    return !(*this == other);
}

ksi::TVT::const_iterator ksi::TVT::const_iterator::operator++(int)
{
    const_iterator temp = *this;
    ++(*this);
    return temp;
}

bool ksi::TVT::const_iterator::operator!=(const const_iterator& other) const
{
    return !(*this == other);
}
