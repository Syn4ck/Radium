#ifndef RADIUMENGINE_ALGORITHM_INFO_DEFINITION_HPP
#define RADIUMENGINE_ALGORITHM_INFO_DEFINITION_HPP

#include <string>

namespace Ra {
namespace Core {

class AlgorithmInfo {
public:
    /// CONSTRUCTOR
    AlgorithmInfo( const std::string& name        = "",
                   const std::string& acronym     = "",
                   const std::string& description = "" );

    /// DESTRUCTOR
    ~AlgorithmInfo();

    /// NAME
    inline std::string getName() const;
    inline void        setName( const std::string& name );

    /// ACRONYM
    inline std::string getAcronym() const;
    inline void        setAcronym( const std::string& acronym );

    /// DESCRIPTION
    inline std::string getDescription() const;
    inline void        setDescription( const std::string& description );

protected:
    /// VARIABLE
    std::string m_name;
    std::string m_acronym;
    std::string m_description;
};

} // namespace Core
} // namespace Ra

#include <Core/Algorithm/Interface/AlgorithmInfo.inl>

#endif // RADIUMENGINE_ALGORITHM_INFO_DEFINITION_HPP

