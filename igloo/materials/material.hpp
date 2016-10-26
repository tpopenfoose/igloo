#pragma once

#include <igloo/geometry/differential_geometry.hpp>
#include <igloo/scattering/scattering_distribution_function.hpp>

namespace igloo
{


class material
{
  public:
    /*! \return A string containing the name of this material.
     */
    virtual const char *name() const = 0;

    /*! \return true if this material is emissive; false, otherwise.
     */
    virtual bool is_emitter() const;

    /*! \return true if this material is a sensor; false, otherwise.
     */
    virtual bool is_sensor() const;

    virtual scattering_distribution_function evaluate_scattering(const differential_geometry& dg) const;

    virtual scattering_distribution_function evaluate_emission(const differential_geometry& dg) const;
}; // end material


} // end igloo
