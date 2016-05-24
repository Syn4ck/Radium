#include <Tests/CoreTests/Tests.hpp>
#include <Tests/CoreTests/RayCasts/RayCastTest.hpp>
#include <Tests/CoreTests/Base/BaseTests.hpp>

int main()
{
    RaTests::TestManager::createInstance();
    RaTests::TestManager::getInstance()->m_options.m_breakOnFailure = true;
    RaTests::DummyTestPass p;
    RaTests::BaseTests b;
    RaTests::RayCastAabbTests a;
    return RaTests::TestManager::getInstance()->run();
}

