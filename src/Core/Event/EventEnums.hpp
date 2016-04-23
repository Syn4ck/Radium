#ifndef RADIUMENGINE_EVENTENUMS_HPP
#define RADIUMENGINE_EVENTENUMS_HPP

#include <Core/RaCore.hpp>

namespace Ra
{
    namespace Core
    {
        enum Modifier
        {
            Modifier_Null  = 0x1,
            Modifier_Ctrl  = 0x2,
            Modifier_Shift = 0x4,
            Modifier_Alt   = 0x8
        };

        enum KeyEventType
        {
            KeyEvent_Pressed  = 0x1,
            KeyEvent_Released = 0x2
        };

        enum MouseEventType
        {
            MouseEvent_Pressed  = 0x1,
            MouseEvent_Released = 0x2,
            MouseEvent_Moved    = 0x4,
            MouseEvent_Wheel    = 0x8
        };

        enum MouseButton
        {
            MouseButton_Left   = 0x1,
            MouseButton_Middle = 0x2,
            MouseButton_Right  = 0x4
        };
    } // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_EVENTENUMS_HPP
