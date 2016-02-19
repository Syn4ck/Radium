#include <Core/Algorithm/Interface/AlgorithmSettings.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
AlgorithmSettings::AlgorithmSettings( const uchar priority ) :
    m_priority          ( priority ),
    m_runSetup          ( true  ),
    m_runConfigCheck    ( true  ),
    m_runPreProcessing  ( true  ),
    m_runProcessing     ( true  ),
    m_runPostProcessing ( true  ),
    m_forceSetup        ( false ),
    m_forceConfigCheck  ( false ),
    m_step              ( false ),
    m_multiThread       ( false ),
    m_verbose           ( false ),
    m_failAssert        ( false )
{ }

/// DESTRUCTOR
AlgorithmSettings::~AlgorithmSettings() { }

} // namespace Core
} // namespace Ra
