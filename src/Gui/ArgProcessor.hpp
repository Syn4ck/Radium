#ifndef RADIUMENGINE_GUIBASE_ARGPROCESSOR_HPP
#define RADIUMENGINE_GUIBASE_ARGPROCESSOR_HPP

#include <vector>
#include <string>
#include <sstream>

namespace Ra
{
    namespace Guibase
    {
        struct Option
        {
            std::vector<std::string> options; // -, --
            std::string value;

            inline char valueAsChar() const { return valueAs<char>(); }
            inline unsigned char valueAsUChar() const { return valueAs<unsigned char>(); }
            inline int valueAsInt() const { return valueAs<int>(); }
            inline uint valueAsUint() const { return valueAs<uint>(); }
            inline float valueAsFloat() const { return valueAs<float>(); }
            inline double valueAsDouble() const { return valueAs<double>(); }

            template <typename T> inline T valueAs() const
            {
                T result;
                std::stringstream ss(value);
                ss >> result;
                return result;
            }
        };
    }
}

#endif // RADIUMENGINE_GUIBASE_ARGPROCESSOR_HPP
