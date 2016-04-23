#ifndef RADIUMENGINE_APP_EVENTTRANSLATOR_HPP
#define RADIUMENGINE_APP_EVENTTRANSLATOR_HPP

#include <Core/Event/KeyEvent.hpp>
#include <Core/Event/MouseEvent.hpp>

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

namespace Ra
{
    void qtToCore(const QKeyEvent* qe, Core::KeyEvent* re);
    void qtToCore(const QMouseEvent* qe, Core::MouseEvent* re);
    void qtToCore(const QWheelEvent* qe, Core::MouseEvent* re);
}

#endif // RADIUMENGINE_APP_EVENTTRANSLATOR_HPP
