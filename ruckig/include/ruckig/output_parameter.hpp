#pragma once

#include <array>
#include <type_traits>

#include <ruckig/trajectory.hpp>


namespace ruckig {

//! Output type of Ruckig
template<size_t DOFs>
class OutputParameter {
    template<class T> using Vector = typename std::conditional<DOFs >= 1, std::array<T, DOFs>, std::vector<T>>::type;

public:
    size_t degrees_of_freedom;

    //! Current trajectory
    Trajectory<DOFs> trajectory;

    // Current kinematic state
    Vector<double> new_position, new_velocity, new_acceleration;

    //! Current time on trajectory
    double time;

    //! Index of the current section between two intermediate positions (only relevant in Ruckig Pro)
    size_t new_section {0};

    //! Was an intermediate position reached in the last cycle? (only relevant in Ruckig Pro)
    bool did_section_change {false};

    //! Was a new trajectory calculation performed in the last cycle?
    bool new_calculation {false};

    //! Was the trajectory calculation interrupted? (only in Ruckig Pro)
    bool was_calculation_interrupted {false};

    //! Computational duration of the last update call
    double calculation_duration; // [µs]

    template <size_t D = DOFs, typename std::enable_if<D >= 1, int>::type = 0>
    OutputParameter(): degrees_of_freedom(DOFs) { }

    template <size_t D = DOFs, typename std::enable_if<D == 0, int>::type = 0>
    OutputParameter(size_t dofs): degrees_of_freedom(dofs), trajectory(Trajectory<0>(dofs)) {
        new_position.resize(dofs);
        new_velocity.resize(dofs);
        new_acceleration.resize(dofs);
    }

    void pass_to_input(InputParameter<DOFs>& input) const {
        input.current_position = new_position;
        input.current_velocity = new_velocity;
        input.current_acceleration = new_acceleration;

        // Remove first intermediate waypoint if section did change
        if (did_section_change && !input.intermediate_positions.empty()) {
            input.intermediate_positions.erase(input.intermediate_positions.begin());
        }
    }
};

} // namespace ruckig
