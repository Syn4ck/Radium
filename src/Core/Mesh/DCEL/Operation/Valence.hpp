#ifndef RADIUMENGINE_DCEL_VALENCE_OPERATION_DEFINITION_HPP
#define RADIUMENGINE_DCEL_VALENCE_OPERATION_DEFINITION_HPP

#include <Core/CoreMacros.hpp>

#include <Core/Mesh/DCEL/Definition.hpp>

namespace Ra {
namespace Core {

uint valence( const Vertex_ptr& v );
uint optimalValence( const Vertex_ptr& v );
int dOptimalValence( const Vertex_ptr& v );
bool isExtraordinary( const Vertex_ptr& v );

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_DCEL_VALENCE_OPERATION_DEFINITION_HPP
