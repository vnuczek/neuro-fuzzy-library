/** @file */

#include <vector>
#include <algorithm>
#include <cmath>

#include "data-modifier.h"
#include "data-modifier-standardiser.h"
#include "../service/debug.h"

ksi::data_modifier* ksi::data_modifier_standardiser::clone() const
{
    return new ksi::data_modifier_standardiser(*this);
}


ksi::data_modifier_standardiser::data_modifier_standardiser (ksi::data_modifier_standardiser  && dm): ksi::data_modifier(dm) 
{
}

ksi::data_modifier_standardiser::data_modifier_standardiser(const ksi::data_modifier_standardiser & dm): data_modifier(dm)
{
}

ksi::data_modifier_standardiser::data_modifier_standardiser() : data_modifier()
{
}

ksi::data_modifier_standardiser & ksi::data_modifier_standardiser::operator=(ksi::data_modifier_standardiser && dm)
{
   if (this == & dm)
      return *this;
   
   ksi::data_modifier::operator=(dm);
     
   return *this;
}

ksi::data_modifier_standardiser & ksi::data_modifier_standardiser::operator=(const ksi::data_modifier_standardiser& dm)
{
   if (this == & dm)
      return *this;
   
   ksi::data_modifier::operator=(dm);
   
   return *this;   
}

ksi::data_modifier_standardiser::~data_modifier_standardiser()
{
}

void ksi::data_modifier_standardiser::modify(ksi::dataset & ds)
{
   try
   {
      // no i tu sie zaczyna zabawa :-) 
      size_t nAttributes = ds.getNumberOfAttributes();
      size_t nDataItems = ds.getNumberOfData();
      std::vector<double> sums   (nAttributes, 0);
      std::vector<double> sqsums (nAttributes, 0);
      std::vector<std::size_t> number_of_items_in_each_attribute (nAttributes, 0);
      
      if (nDataItems < 2)
         return; // finito :-)
         
      for (size_t k = 0; k < nAttributes; k++)
         sums[k] = sqsums[k] = 0.0;

      for (size_t w = 0; w < nDataItems; w++)
      {
         for (size_t k = 0; k < nAttributes; k++)
         {
            if (ds.exists(w, k))
            {
               double value = ds.get(w, k);

               sums[k] += value;
               sqsums[k] += (value * value);
               number_of_items_in_each_attribute[k]++;
            }
         }
      }
      // no i teraz standardyzacja:
      
      std::vector<double> averages (nAttributes, 0);
      std::vector<double> stddevs  (nAttributes, 0);
      
      for (size_t k = 0; k < nAttributes; k++)
      {
         auto number_of_items = number_of_items_in_each_attribute[k];
         averages[k] = sums[k] / number_of_items;
         stddevs[k]  = std::sqrt (sqsums[k] / number_of_items - pow(averages[k], 2));
      }
      // no i standaryzacja wlasciwa:
      for (size_t k = 0; k < nAttributes; k++)
      {
         if (stddevs[k] == 0.0)   
         {
            for (size_t w = 0; w < nDataItems; w++)
            {
               if (ds.exists(w, k))
                  ds.set (w, k, 0);
            }
         }
         else
         {
            double avg = averages[k];
            double dev = stddevs[k];
            for (size_t w = 0; w < nDataItems; w++)
            {
               if (ds.exists(w, k))
               {
                  double value = ds.get (w, k);
                  ds.set (w, k, (value - avg) / dev);
               }
            }
         }
      }
      
      // and call pNext modifier
      if (pNext)
         pNext->modify(ds);
      
   }
   CATCH;   
}


std::string ksi::data_modifier_standardiser::getDescription() const
{
   return std::string("standardiser");
}



