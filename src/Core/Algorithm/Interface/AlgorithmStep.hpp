#ifndef RADIUMENGINE_ALGORITHM_STEP_DEFINITION_HPP
#define RADIUMENGINE_ALGORITHM_STEP_DEFINITION_HPP

#include <string>

namespace Ra {
namespace Core {

template < typename TYPE >
class AlgorithmStep {
public:
    /// CONSTRUCTOR
    AlgorithmStep( const std::string& name = "",
                   TYPE* const        data = nullptr );

    /// DESTRUCTOR
    ~AlgorithmStep();

    /// NAME
    inline std::string getName() const;

    /// DATA
    inline TYPE* getData();

protected:
    /// VARIABLE
    std::string m_name;
    TYPE*       m_data;
};

} // namespace Core
} // namespace Ra

#include <Core/Algorithm/Interface/AlgorithmStep.inl>

#endif // RADIUMENGINE_ALGORITHM_STEP_DEFINITION_HPP
