#include <Core/Algorithm/Interface/AlgorithmExecution.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
AlgorithmExecution::AlgorithmExecution( const uchar priority ) {
    for( uint i = 0; i < OVERALL; ++i ) {
          m_run[i] = true;
        m_force[i] = false;
    }
}

/// DESTRUCTOR
AlgorithmExecution::~AlgorithmExecution() { }

} // namespace Core
} // namespace Ra
