
#include "../common/data-modifier-imputer-granular.h"
#include "../common/data-modifier-imputer.h"
#include "../partitions/partitioner.h"
#include "../common/dataset.h"
#include "../common/datum.h"

std::pair<ksi::dataset, ksi::dataset> ksi::data_modifier_imputer_granular::dataset_marginalisation(const dataset& ds)
{
	dataset complete_data, incomplete_data;

	for (std::size_t i = 0; i < ds.size(); ++i)
	{
		auto tuple = ds.getDatum(i);
		if (tuple->is_complete())
			complete_data.addDatum(*tuple);
		else
			incomplete_data.addDatum(*tuple);
	}

	return std::make_pair(complete_data, incomplete_data);
}

ksi::dataset ksi::data_modifier_imputer_granular::granular_imputation(const dataset& ds)
{
	auto [complete_dataset, incomplete_dataset] = dataset_marginalisation(ds);
	
	auto partitioned_data = _pPartitioner->doPartition(complete_dataset);
	
	auto U = partitioned_data.getPartitionMatrix();
	auto V = partitioned_data.getClusterCentres();
	

	for (auto i = 0; i < incomplete_dataset.size(); ++i) {
		auto incmplete_datum = incomplete_dataset.getDatum(i);
		auto attributes = incmplete_datum->getVector();
		for (auto j = 0; j < partitioned_data.getNumberOfClusters(); ++j) {
			for (auto k = 0; k < incmplete_datum->getNumberOfAttributes(); ++k) {
				if (incmplete_datum->is_complete_attribute(k)) {

				}
			}
		}
	}

	return dataset();
}

ksi::data_modifier_imputer_granular::data_modifier_imputer_granular(const partitioner& Partitioner) 
	: _pPartitioner(std::make_shared<partitioner>(Partitioner))
{
}

ksi::data_modifier_imputer_granular::data_modifier_imputer_granular(const data_modifier_imputer_granular& other)
	: data_modifier_imputer(other), _pPartitioner(other._pPartitioner)
{
}

ksi::data_modifier_imputer_granular::data_modifier_imputer_granular(data_modifier_imputer_granular&& other) noexcept
	: data_modifier_imputer(std::move(other)), _pPartitioner(std::move(other._pPartitioner))

{
}

ksi::data_modifier_imputer_granular& ksi::data_modifier_imputer_granular::operator=(const data_modifier_imputer_granular& other)
{
	if (this != &other)
	{
		data_modifier_imputer::operator=(other);
		_pPartitioner = other._pPartitioner;
	}
	return *this;
}

ksi::data_modifier_imputer_granular& ksi::data_modifier_imputer_granular::operator=(data_modifier_imputer_granular&& other) noexcept
{
	if (this != &other)
	{
		data_modifier_imputer::operator=(std::move(other));
		_pPartitioner = std::move(other._pPartitioner);
	}
	return *this;
}

ksi::data_modifier_imputer_granular::~data_modifier_imputer_granular()
{
}

ksi::data_modifier* ksi::data_modifier_imputer_granular::clone() const
{
	return new data_modifier_imputer_granular(*_pPartitioner->clone());
}

void ksi::data_modifier_imputer_granular::modify(dataset& ds)
{
}
