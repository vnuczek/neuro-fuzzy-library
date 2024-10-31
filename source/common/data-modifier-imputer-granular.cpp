/** @file */

#include <cmath>

#include "../auxiliary/vector-operators.h"
#include "../common/data-modifier-imputer-granular.h"

#include <numeric>

#include "../common/data-modifier-imputer.h"
#include "../common/dataset.h"
#include "../common/datum.h"
#include "../partitions/partitioner.h"
#include "../service/exception.h"

ksi::dataset ksi::data_modifier_imputer_granular::granular_imputation(const dataset& ds)
{
    try
    {
        auto result(ds);
        extract_complete_dataset_and_incomplete_indices(ds);
        auto partitioned_data = _pPartitioner->doPartition(this->complete_dataset);

        set_granules_attributes(partitioned_data);

        this->number_of_tuple_attributes = ds.getNumberOfAttributes();

        for (const auto& tup_index: incomplete_indices) // for each incomplete tuple
        {
            auto [imputed_average_attributes, incomplete_attributes_indices] = handle_incomplete_tuple(*ds.getDatum(tup_index));
            result.getDatumNonConst(tup_index)->changeMissingAttributesValues(imputed_average_attributes, incomplete_attributes_indices);
        }

        return result;
    }
    CATCH;
}

void ksi::data_modifier_imputer_granular::extract_complete_dataset_and_incomplete_indices(const dataset& ds)
{
   try
   {
   	  this->complete_dataset = ksi::dataset();
      this->incomplete_indices.clear();

      for (std::size_t i = 0; i < ds.size(); ++i)
      {
         auto data_item = ds.getDatum(i);
         if (data_item->is_complete()) {
             complete_dataset.addDatum(*data_item);
         }
         else {
            this->incomplete_indices.push_back(i);
         }
      }
   }
   CATCH;
}

void ksi::data_modifier_imputer_granular::set_granules_attributes(const ksi::partition& partitioned_data)
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

std::pair<std::vector<double>, std::vector<std::size_t>> ksi::data_modifier_imputer_granular::handle_incomplete_tuple(const datum& incomplete_tuple)
{
   auto incomplete_tuple_attributes = incomplete_tuple.getVector();
   auto incomplete_attributes_indices = get_incomplete_attributes_indices(incomplete_tuple);

   auto [imputed_attributes, granule_membership] = calculate_granule_imputations_and_memberships(incomplete_tuple_attributes, incomplete_attributes_indices);

   auto imputed_average_attributes = weighted_average(imputed_attributes, granule_membership);

   return std::make_pair(imputed_average_attributes, incomplete_attributes_indices);
}

std::vector<std::size_t> ksi::data_modifier_imputer_granular::get_incomplete_attributes_indices(const ksi::datum& incomplete_tuple) const
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

std::pair<std::vector<std::vector<double>>, std::vector<double>> ksi::data_modifier_imputer_granular::calculate_granule_imputations_and_memberships(const std::vector<double>& incomplete_tuple_attributes, const std::vector<std::size_t>& incomplete_attributes_indices)
{
    std::vector<std::vector<double>> imputed_attributes;
    imputed_attributes.reserve(cluster_numbers);

    std::vector<double> granule_membership;
    granule_membership.reserve(cluster_numbers);

    for (std::size_t gran = 0; gran < cluster_numbers; ++gran) // for each granule 
    {
       auto imputed_granule_attributes = impute_granule_attributes(incomplete_attributes_indices, granules_centers[gran]);
       imputed_attributes.push_back(imputed_granule_attributes);

       auto imputed_tuple_attributes = insert_missing_attributes(incomplete_tuple_attributes, imputed_granule_attributes, incomplete_attributes_indices);

       // debug(imputed_tuple_attributes);

       granule_membership.push_back(calculate_granule_membership(imputed_tuple_attributes, granules_centers[gran], granules_fuzzifications[gran]));
    }

    /*if (std::accumulate(granule_membership.begin(), granule_membership.end(), 0.0) == 0.0)
    {
        debug(incomplete_tuple_attributes);
        debug(incomplete_attributes_indices);
        debug(imputed_attributes);
        debug(granule_membership);

        debug(granules_centers);
		debug(granules_fuzzifications);
    }*/

    return std::make_pair(imputed_attributes, granule_membership);
}

std::vector<double> ksi::data_modifier_imputer_granular::impute_granule_attributes(const std::vector<std::size_t>& incomplete_attributes_indices, const std::vector<double>& granule_centers) 
{
   std::vector<double> imputed_granule_attributes; 

   for (const auto& attr_index: incomplete_attributes_indices) // for each missing attribute in the incomplete tuple
   {
      imputed_granule_attributes.push_back(granule_centers[attr_index]);
   }

    return imputed_granule_attributes;
}


std::vector<double> ksi::data_modifier_imputer_granular::insert_missing_attributes(const std::vector<double>& incomplete_tuple_attributes, std::vector<double>& imputed_granule_attributes, const std::vector<std::size_t>& incomplete_attributes_indices)
{
   auto imputed_tuple_attributes = incomplete_tuple_attributes; 

   for (std::size_t index = 0; index < imputed_granule_attributes.size(); ++index) // for each missing attribute in the incomplete tuple
   {
   	  auto attr_index = incomplete_attributes_indices[index];
      imputed_tuple_attributes[attr_index] = imputed_granule_attributes[index];
   }

	return imputed_tuple_attributes;
}

double ksi::data_modifier_imputer_granular::calculate_granule_membership(const std::vector<double>& estimated_tuple_attributes, const std::vector<double>& granule_centers, const std::vector<double>& granule_fuzzifications)
{
   try
   {
      double total_membership = 1.0;

      for (std::size_t attr = 0; attr < estimated_tuple_attributes.size(); ++attr) // for each attribute of X tuple
      {
         auto attribute_membership = 
             std::exp( - (std::pow((estimated_tuple_attributes[attr] - granule_centers[attr]), 2) / (2 * std::pow(granule_fuzzifications[attr], 2))));

      	if (attribute_membership == 0.0)
		 {
			debug("-----------------");
			debug(estimated_tuple_attributes[attr]);
            debug(granule_centers[attr]);
            debug(granule_fuzzifications[attr]);
            debug((std::pow((estimated_tuple_attributes[attr] - granule_centers[attr]), 2) / (2 * std::pow(granule_fuzzifications[attr], 2))));
		 }
      	
         total_membership = _pTnorm->tnorm(total_membership, attribute_membership);
      }

	  /*if (total_membership == 0.0)
	  {
		  debug(estimated_tuple_attributes);
		  debug(granule_centers);
		  debug(granule_fuzzifications);
	  }*/

      return total_membership;
   }
   CATCH;
}

std::vector<double> ksi::data_modifier_imputer_granular::weighted_average(const std::vector < std::vector<double>>& estimated_values, const std::vector<double>& weights) 
{
   try
   {
      std::vector<double> numerator(estimated_values[0].size(), 0.0);
      double denominator = 0.0;

      for (std::size_t i = 0; i < weights.size(); ++i) {
         numerator += estimated_values[i] * weights[i];
         denominator += weights[i];
      }

      if (denominator == 0.0)
      {
          /*debug(estimated_values);
		  debug(weights);*/

          throw ksi::exception("Sum of weights is zero!");
      }

      return numerator / denominator;
   }
   CATCH;
}

ksi::data_modifier_imputer_granular::data_modifier_imputer_granular(partitioner& Partitioner, t_norm& Tnorm)
    : data_modifier_imputer(), complete_dataset(), incomplete_indices(),granules_fuzzifications(), cluster_numbers(), granules_centers(), number_of_tuple_attributes()
{
    _pPartitioner = std::shared_ptr<ksi::partitioner>(Partitioner.clone());
    _pTnorm = std::shared_ptr<ksi::t_norm>(Tnorm.clone());
}

ksi::data_modifier_imputer_granular::data_modifier_imputer_granular(const data_modifier_imputer_granular& other)
	: data_modifier_imputer(other), complete_dataset(other.complete_dataset), incomplete_indices(other.incomplete_indices), granules_fuzzifications(other.granules_fuzzifications), cluster_numbers(other.cluster_numbers), granules_centers(other.granules_centers), number_of_tuple_attributes(other.number_of_tuple_attributes)
{ 
    _pPartitioner = std::shared_ptr<ksi::partitioner>(other._pPartitioner->clone());
    _pTnorm = std::shared_ptr<ksi::t_norm>(other._pTnorm->clone());
}

ksi::data_modifier_imputer_granular::data_modifier_imputer_granular(data_modifier_imputer_granular&& other) noexcept
	: data_modifier_imputer(std::move(other)), _pPartitioner(std::move(other._pPartitioner)), _pTnorm(std::move(other._pTnorm)), complete_dataset(std::move(other.complete_dataset)), incomplete_indices(std::move(other.incomplete_indices)), granules_fuzzifications(std::move(other.granules_fuzzifications)), cluster_numbers(other.cluster_numbers), granules_centers(std::move(other.granules_centers)), number_of_tuple_attributes(other.number_of_tuple_attributes)
{
}

ksi::data_modifier_imputer_granular& ksi::data_modifier_imputer_granular::operator=(const data_modifier_imputer_granular& other)
{
	if (this != &other)
	{
		data_modifier_imputer::operator=(other);
		_pPartitioner = std::shared_ptr<ksi::partitioner>(other._pPartitioner->clone());
		_pTnorm = std::shared_ptr<ksi::t_norm>(other._pTnorm->clone());
		complete_dataset = other.complete_dataset;
		incomplete_indices =other.incomplete_indices;
		granules_fuzzifications = other.granules_fuzzifications;
		cluster_numbers = other.cluster_numbers;
		granules_centers =other.granules_centers;
		number_of_tuple_attributes = other.number_of_tuple_attributes;
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
        complete_dataset = std::move(other.complete_dataset);
		incomplete_indices = std::move(other.incomplete_indices);
		granules_fuzzifications = std::move(other.granules_fuzzifications);
		cluster_numbers = other.cluster_numbers;
		granules_centers = std::move(other.granules_centers);
		number_of_tuple_attributes = other.number_of_tuple_attributes;
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

std::string ksi::data_modifier_imputer_granular::getDescription() const
{
   return "granular imputation";
}

std::string ksi::data_modifier_imputer_granular::getName() const
{
    return "imputer_granular";
}

// end of file :-)
