/** @file */

#include <cmath>

#include "../auxiliary/vector-operators.h"
#include "../common/data-modifier-imputer-granular.h"
#include "../common/data-modifier-imputer.h"
#include "../common/dataset.h"
#include "../common/datum.h"
#include "../partitions/partitioner.h"
#include "../service/exception.h"

std::pair<ksi::dataset, ksi::dataset> ksi::data_modifier_imputer_granular::split_complete_incomplete(const dataset& ds)
{
   /// KW: Czy zmieniamy metodę by niezwracała zbiorów?

   try
   {
      this->complete_indices.clear();
      this->incomplete_indices.clear();
      dataset complete_data, incomplete_data;

      for (std::size_t i = 0; i < ds.size(); ++i)
      {
         auto data_item = ds.getDatum(i);
         if (data_item->is_complete()) {
            complete_data.addDatum(*data_item);
            this->complete_indices.push_back(i);
         }
         else {
            incomplete_data.addDatum(*data_item);
            this->incomplete_indices.push_back(i);
         }
      }

      return std::make_pair(complete_data, incomplete_data);
   }
   CATCH;
}

ksi::dataset ksi::data_modifier_imputer_granular::granular_imputation(const dataset& ds)
{
   /// KW: Czy usuwamy incomplete_dataset?

    try
    {
        auto result(ds);
        auto [complete_dataset, incomplete_dataset] = split_complete_incomplete(ds);
        auto partitioned_data = _pPartitioner->doPartition(complete_dataset);

        set_granules_atributes(partitioned_data);

        this->number_of_tuple_attributes = ds.getDatum(0)->getNumberOfAttributes();

        for (const auto& tup_index: incomplete_indices) // for each incomplete tuple
        {
            auto imputed_tuple = handle_incomplete_tuple(*ds.getDatum(tup_index));
            result.getDatumNonConst(tup_index)->changeAttributesValues(imputed_tuple);
        }

        return result;
    }
    CATCH;
}

std::vector<double> ksi::data_modifier_imputer_granular::handle_incomplete_tuple(const datum& incomplete_tuple)
{
   auto incomplete_attributes_indice = get_incomplete_attributes_indices(incomplete_tuple);

   auto [imputed_tuples, granule_membership] = calculate_granule_imputations_and_memberships(incomplete_tuple, incomplete_attributes_indice);

   return weighted_average(imputed_tuples, granule_membership);
}

std::vector<std::size_t> ksi::data_modifier_imputer_granular::get_incomplete_attributes_indices(const ksi::datum& incomplete_tuple)
{
   std::vector<std::size_t> incomplete_attributes_indices;

   for (std::size_t attr = 0; attr < number_of_tuple_attributes; ++attr) // for each attribute in the incomplete tuple
    {
        if (!incomplete_tuple.is_attribute_complete(attr))
        {
            incomplete_attributes_indices.push_back(attr);
        }
    }

    return incomplete_attributes_indices;
}

std::pair<std::vector<std::vector<double>>, std::vector<double>> ksi::data_modifier_imputer_granular::calculate_granule_imputations_and_memberships(const datum& incomplete_tuple, const std::vector<std::size_t>& incomplete_attributes_indice)
{
    std::vector<std::vector<double>> imputed_attributes;
    imputed_attributes.reserve(cluster_numbers);

    std::vector<double> granule_membership;
    granule_membership.reserve(cluster_numbers);

    for (auto gran = 0; gran < cluster_numbers; ++gran) // for each granule 
    {
       auto attributes = impute_attributes(incomplete_attributes_indice, granules_centers[gran]);
       imputed_attributes.push_back(attributes);

       granule_membership.push_back(calculate_granule_membership(attributes, granules_centers[gran], granules_fuzzifications[gran]));
    }

    return std::make_pair(imputed_tuples, granule_membership);
}

std::vector<double> ksi::data_modifier_imputer_granular::impute_attributes(const std::vector<std::size_t>& incomplete_attributes_indice, const std::vector<double>& granule_centers)
{
   std::vector<std::size_t> incomplete_attributes_indices;

   for (const auto& attr_index: incomplete_attributes_indice) // for each missing attribute in the incomplete tuple
   {
      attributes.push_back(granule_centers[attr_index]);
   }

    return attributes;
}

void ksi::data_modifier_imputer_granular::set_granules_atributes(const ksi::partition& partitioned_data)
{  
   this->granules_fuzzifications = partitioned_data.getClusterFuzzifications();

   try 
   {
      if (granules_fuzzifications.empty())
      {
         throw ksi::exception("The partitioner has not set the fuzzification of granules (the S variable) that I need here. "
                              "It is empty. Please use another partitioner.");
      }
   }
   CATCH; 

   this->cluster_numbers = partitioned_data.getNumberOfClusters();
   this->granules_centers = partitioned_data.getClusterCentres();   
}

ksi::data_modifier_imputer_granular::data_modifier_imputer_granular(partitioner& Partitioner, t_norm& Tnorm)
    : data_modifier_imputer()
{
    _pPartitioner = std::shared_ptr<ksi::partitioner>(Partitioner.clone());
    _pTnorm = std::shared_ptr<ksi::t_norm>(Tnorm.clone());
}

ksi::data_modifier_imputer_granular::data_modifier_imputer_granular(const data_modifier_imputer_granular& other)
	: data_modifier_imputer(other), incomplete_indices(other.incomplete_indices)
{ 
    _pPartitioner = std::shared_ptr<ksi::partitioner>(other._pPartitioner->clone());
    _pTnorm = std::shared_ptr<ksi::t_norm>(other._pTnorm->clone());
}

ksi::data_modifier_imputer_granular::data_modifier_imputer_granular(data_modifier_imputer_granular&& other) noexcept
	: data_modifier_imputer(std::move(other)), _pPartitioner(std::move(other._pPartitioner)), _pTnorm(std::move(other._pTnorm))
{
	std::swap(incomplete_indices, other.incomplete_indices);
}

ksi::data_modifier_imputer_granular& ksi::data_modifier_imputer_granular::operator=(const data_modifier_imputer_granular& other)
{
	if (this != &other)
	{
		data_modifier_imputer::operator=(other); 
		_pPartitioner = std::shared_ptr<ksi::partitioner>(other._pPartitioner->clone());
        _pTnorm = std::shared_ptr<ksi::t_norm>(other._pTnorm->clone());
		incomplete_indices = other.incomplete_indices;
	}
	return *this;
}

ksi::data_modifier_imputer_granular& ksi::data_modifier_imputer_granular::operator=(data_modifier_imputer_granular&& other) noexcept
{
	if (this != &other)
	{
		data_modifier_imputer::operator=(std::move(other));
		_pPartitioner = std::move(other._pPartitioner);
		_pTnorm = std::move(other._pTnorm);
		std::swap(incomplete_indices, other.incomplete_indices);
	}
	return *this;
}

ksi::data_modifier_imputer_granular::~data_modifier_imputer_granular()
{
}

ksi::data_modifier* ksi::data_modifier_imputer_granular::clone() const
{
	return new data_modifier_imputer_granular(*this);
}

void ksi::data_modifier_imputer_granular::modify(dataset& ds)
{
	ds = this->granular_imputation(ds);
    
    // and call pNext modifier
    if (pNext)
       pNext->modify(ds);                                    
}

std::vector<double> ksi::data_modifier_imputer_granular::weighted_average(const std::vector < std::vector<double>>& estimated_values, const std::vector<double>& weights) 
{
   try
   {
      std::vector<double> numerator(estimated_values[0].size(), 0.0);
      double denominator = 0.0;

      for (auto i = 0; i < weights.size(); ++i) {
         numerator += estimated_values[i] * weights[i];
         denominator += weights[i];
      }

      if (denominator == 0.0)
      {
          throw ksi::exception("Sum of weights is zero!");
      }

      return numerator / denominator;
   }
   CATCH;
}

double ksi::data_modifier_imputer_granular::calculate_granule_membership(const std::vector<double>& estimated_values, const std::vector<double>& granule_centers, const std::vector<double>& granule_fuzzifications)
{
   try
   {
      double total_membership = 1.0;

      for (std::size_t attr = 0; attr < estimated_values.size(); ++attr) // for each attribute of X tuple
      {
         auto attribute_membership = std::exp( - (std::pow((estimated_values[attr] - granule_centers[attr]), 2) / (2 * std::pow(granule_fuzzifications[attr], 2))));
         total_membership = _pTnorm->tnorm(total_membership, attribute_membership);
      }

      return total_membership;
   }
   CATCH;
}

std::string ksi::data_modifier_imputer_granular::getDescription() const
{
   return "granular imputation";
}

// end of file :-)
