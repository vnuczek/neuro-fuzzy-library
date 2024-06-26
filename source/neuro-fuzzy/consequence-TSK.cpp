/** @file */ 
 
#include <vector> 
#include <string>
#include <iostream>
#include <sstream>
#include "consequence-TSK.h"
#include "../service/debug.h"

const std::array<std::string, 7> ksi::consequence_TSK::TSKLocationDescription
{
    "micro",
        "tiny",
        "small",
        "medium",
        "large",
        "huge",
        "giant"
};

ksi::consequence_TSK::consequence_TSK(const std::vector<double> & params)
{
   _params = params;
}

ksi::consequence_TSK::~consequence_TSK()
{ 
}



std::pair<double, double> 
ksi::consequence_TSK::getLocalisationWeight (const std::vector< double >& X, double firing) 
{
   if (_params.size() == 0)
   {
      ksi::exception exc(__FILE__, __FUNCTION__, __LINE__,
                        std::string ("empty parameters"));
      throw exc;
   }
   
   if (X.size() + 1 != _params.size())
   {
      std::stringstream ss;
      ss << "Size of data and size of parameters do not match:" << std::endl;
      ss << "data length:      " << X.size() << " + 1 = " 
                                 << X.size() + 1 << std::endl;
      ss << "parameter number: " << _params.size() << std::endl;
      ksi::exception exc (__FILE__, __FUNCTION__, __LINE__, ss.str());
      throw exc;
   }
   
   //-------------------------
   std::vector<double> X1 (X);
   X1.push_back(1.0); 
   
   double sum_prod = 0.0;
   std::size_t size = X1.size();
   for (std::size_t i = 0; i < size; i++)
      sum_prod += (X1[i] * _params[i]);
   
   return last_localisationWeight = { sum_prod, firing };
} 

ksi::consequence_TSK::consequence_TSK(ksi::consequence_TSK && wzor) : consequence(wzor)
{
   std::swap (_params, wzor._params);
}

ksi::consequence_TSK::consequence_TSK(const ksi::consequence_TSK & wzor)
{
   _params = wzor._params;
}

ksi::consequence_TSK::consequence_TSK()
{
}

ksi::consequence * ksi::consequence_TSK::clone() const
{
   return new consequence_TSK (*this);
}


void ksi::consequence_TSK::setLinearParameters(std::vector< double >& coefficients)
{
   this->_params = coefficients;
}

std::ostream& ksi::consequence_TSK::Print(std::ostream& ss)
{
   for (auto & d : _params)
      ss << d << "  "; 
   return ss;
}

std::ostream& ksi::consequence_TSK::printLinguisticDescription(std::ostream& ss, const DescriptorStatistics& descStat)
{
    for (std::size_t i = 0; i < _params.size() - 1; ++i) {
        ss << " input " << i+1 << " has " << (_params[i] <= descStat.std_dev ? "low " : "high ") << (_params[i] < 0 ? "negative " : "positive ") << "importance AND";
    }

    int locationIndex = (_params.back() - descStat.average) / descStat.std_dev + TSKLocationDescription.size() / 2;
    locationIndex = std::min(std::max(locationIndex, 0), int(TSKLocationDescription.size() - 1));

    return ss << " constant term is " << TSKLocationDescription[locationIndex];
}

