#ifndef RADIUMENGINE_EVENTENUMS_HPP
#define RADIUMENGINE_EVENTENUMS_HPP

#include <Core/RaCore.hpp>

namespace Ra
{
    namespace Core
    {
        enum Modifier
        {
            Modifier_None  = 0,
            Modifier_Ctrl  = 1 << 0,
            Modifier_Shift = 1 << 1,
            Modifier_Alt   = 1 << 2,
        };

        enum MouseButton
        {
            MouseButton_Unknown = 1,
            MouseButton_Left = 0,
            MouseButton_Middle,
            MouseButton_Right,
            MouseButton_Count,
        };
    } // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_EVENTENUMS_HPP
