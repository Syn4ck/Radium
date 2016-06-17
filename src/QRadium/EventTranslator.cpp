#include <QRadium/EventTranslator.hpp>

#include <QKeyEvent>
#include <QMouseEvent>

#include <Core/Event/EventEnums.hpp>
#include <Core/Event/Key.hpp>

namespace QRadium
{
    Ra::Core::Key getCoreKey(const QKeyEvent* e)
    {
        using namespace Ra::Core;
        auto k = e->key();
        switch (k)
        {
            case Qt::Key_Escape:
                return Key_Escape;
            case Qt::Key_Tab:
                return Key_Tab;
            case Qt::Key_Period:
                return Key_Period;
            case Qt::Key_Comma:
                return Key_Comma;
            case Qt::Key_Backtab:
                return Key_BackTab;
            case Qt::Key_Backspace:
                return Key_BackSpace;
            case Qt::Key_Return:
                return Key_Return;
            case Qt::Key_Enter:
                return Key_Enter;
            case Qt::Key_Insert:
                return Key_Insert;
            case Qt::Key_Delete:
                return Key_Delete;
            case Qt::Key_Pause:
                return Key_Pause;
            case Qt::Key_Home:
                return Key_Home;
            case Qt::Key_End:
                return Key_End;
            case Qt::Key_Left:
                return Key_Left;
            case Qt::Key_Up:
                return Key_Up;
            case Qt::Key_Right:
                return Key_Right;
            case Qt::Key_Down:
                return Key_Down;
            case Qt::Key_PageUp:
                return Key_PageUp;
            case Qt::Key_PageDown:
                return Key_PageDown;
            case Qt::Key_Shift:
                return Key_Shift;
            case Qt::Key_Control:
                return Key_Control;
            case Qt::Key_Alt:
                return Key_Alt;
            case Qt::Key_F1:
                return Key_F1;
            case Qt::Key_F2:
                return Key_F2;
            case Qt::Key_F3:
                return Key_F3;
            case Qt::Key_F4:
                return Key_F4;
            case Qt::Key_F5:
                return Key_F5;
            case Qt::Key_F6:
                return Key_F6;
            case Qt::Key_F7:
                return Key_F7;
            case Qt::Key_F8:
                return Key_F8;
            case Qt::Key_F9:
                return Key_F9;
            case Qt::Key_F10:
                return Key_F10;
            case Qt::Key_F11:
                return Key_F11;
            case Qt::Key_F12:
                return Key_F12;
            case Qt::Key_F13:
                return Key_F13;
            case Qt::Key_F14:
                return Key_F14;
            case Qt::Key_F15:
                return Key_F15;
            case Qt::Key_Space:
                return Key_Space;
            case Qt::Key_Plus:
                return Key_Plus;
            case Qt::Key_Minus:
                return Key_Minus;
            case Qt::Key_0:
                return Key_Num0;
            case Qt::Key_1:
                return Key_Num1;
            case Qt::Key_2:
                return Key_Num2;
            case Qt::Key_3:
                return Key_Num3;
            case Qt::Key_4:
                return Key_Num4;
            case Qt::Key_5:
                return Key_Num5;
            case Qt::Key_6:
                return Key_Num6;
            case Qt::Key_7:
                return Key_Num7;
            case Qt::Key_8:
                return Key_Num8;
            case Qt::Key_9:
                return Key_Num9;
            case Qt::Key_A:
                return Key_A;
            case Qt::Key_B:
                return Key_B;
            case Qt::Key_C:
                return Key_C;
            case Qt::Key_D:
                return Key_D;
            case Qt::Key_E:
                return Key_E;
            case Qt::Key_F:
                return Key_F;
            case Qt::Key_G:
                return Key_G;
            case Qt::Key_H:
                return Key_H;
            case Qt::Key_I:
                return Key_I;
            case Qt::Key_J:
                return Key_J;
            case Qt::Key_K:
                return Key_K;
            case Qt::Key_L:
                return Key_L;
            case Qt::Key_M:
                return Key_M;
            case Qt::Key_N:
                return Key_N;
            case Qt::Key_O:
                return Key_O;
            case Qt::Key_P:
                return Key_P;
            case Qt::Key_Q:
                return Key_Q;
            case Qt::Key_R:
                return Key_R;
            case Qt::Key_S:
                return Key_S;
            case Qt::Key_T:
                return Key_T;
            case Qt::Key_U:
                return Key_U;
            case Qt::Key_V:
                return Key_V;
            case Qt::Key_W:
                return Key_W;
            case Qt::Key_X:
                return Key_X;
            case Qt::Key_Y:
                return Key_Y;
            case Qt::Key_Z:
                return Key_Z;

            default:
                return Ra::Core::Key_Unknown;
        }
    }

    Ra::Core::MouseButton getCoreButton(const QMouseEvent* e)
    {
        switch (e->button())
        {
            case Qt::LeftButton:
                return Ra::Core::MouseButton_Left;
            case Qt::MiddleButton:
                return Ra::Core::MouseButton_Middle;
            case Qt::RightButton:
                return Ra::Core::MouseButton_Right;
            default:
                return Ra::Core::MouseButton_Unknown;
        }
    }
}
