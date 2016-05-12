#ifndef RADIUMENGINE_APP_EVENTTRANSLATOR_HPP
#define RADIUMENGINE_APP_EVENTTRANSLATOR_HPP

#include <Core/Event/Key.hpp>
#include <Core/Event/MouseEvent.hpp>

class QKeyEvent;
class QMouseEvent;

namespace QRadium
{
    Ra::Core::Key getCoreKey(const QKeyEvent* e);
    Ra::Core::MouseButton getCoreButton(const QMouseEvent* e);
}

#endif // RADIUMENGINE_APP_EVENTTRANSLATOR_HPP
