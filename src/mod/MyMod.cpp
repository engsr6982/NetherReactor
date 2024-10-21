#include "mod/MyMod.h"

#include <memory>

#include "ll/api/mod/RegisterHelper.h"

#include "NetherReactorManager.h"

namespace my_mod {

static std::unique_ptr<MyMod> instance;

MyMod& MyMod::getInstance() { return *instance; }

bool MyMod::load() {
    getSelf().getLogger().debug("Loading...");
    // Code for loading the mod goes here.
    return true;
}

bool MyMod::enable() {
    getSelf().getLogger().debug("Enabling...");

    nr::NetherReactorManager::getInstance().init();

    return true;
}

bool MyMod::disable() {
    getSelf().getLogger().debug("Disabling...");

    nr::NetherReactorManager::getInstance().release();

    return true;
}
bool MyMod::unload() { return true; }

} // namespace my_mod

LL_REGISTER_MOD(my_mod::MyMod, my_mod::instance);
