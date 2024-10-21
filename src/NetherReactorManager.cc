#include "NetherReactorManager.h"
#include "NetherReactor.h"
#include "gmlib/mod/recipe/CustomRecipe.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/ListenerBase.h"
#include "ll/api/event/player/PlayerInteractBlockEvent.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/level/BlockSource.h"


ll::event::ListenerPtr LISTENER_PlayerInteractBlock;

namespace nr {
NetherReactorManager& NetherReactorManager::getInstance() {
    static NetherReactorManager instance;
    return instance;
}

void NetherReactorManager::init() {
    // 注册自定义配方
    gmlib::mod::recipe::RecipeRegistry::registerShapedCraftingTableRecipe(
        Recipe_NetherRector_ID,
        Recipe_NetherRector_Shape,
        Recipe_NetherRector_Ingredients,
        NetherReactor::Minecraft_NetherReactor,
        1
    );

    LISTENER_PlayerInteractBlock =
        ll::event::EventBus::getInstance().emplaceListener<ll::event::PlayerInteractBlockEvent>(
            [this](ll::event::PlayerInteractBlockEvent& ev) {
                auto bl = ev.block();
                if (bl && bl->getTypeName() == NetherReactor::Minecraft_NetherReactor) {
                    if (!hasReactor(ev.blockPos(), ev.self().getDimensionId())) {
                        addReactor(NetherReactor(ev.blockPos(), ev.self().getDimensionId())); // 创建反应堆
                    }

                    // TODO: 检查是否可以启动反应堆
                }
            }
        );
}

void NetherReactorManager::release() {
    // 卸载自定义配方
    gmlib::mod::recipe::RecipeRegistry::unregisterRecipe(Recipe_NetherRector_ID);

    // 卸载事件监听器
    ll::event::EventBus::getInstance().removeListener(LISTENER_PlayerInteractBlock);
}


// func
bool NetherReactorManager::hasReactor(string const& ID) const { return mReactors.contains(ID); }
bool NetherReactorManager::hasReactor(BlockPos const& pos, int dimensionId) const {
    return hasReactor(NetherReactor::formatID(pos, dimensionId));
}

NetherReactor* NetherReactorManager::getReactor(string const& ID) {
    return mReactors.contains(ID) ? &mReactors.at(ID) : nullptr;
}
NetherReactor* NetherReactorManager::getReactor(BlockPos const& pos, int dimensionId) {
    return getReactor(NetherReactor::formatID(pos, dimensionId));
}

bool NetherReactorManager::addReactor(NetherReactor reactor) {
    if (hasReactor(reactor)) {
        return false;
    }
    mReactors.emplace(reactor, std::move(reactor));
    return true;
}
bool NetherReactorManager::removeReactor(string const& ID) {
    if (!hasReactor(ID)) {
        return false;
    }
    mReactors.erase(ID);
    return true;
}
bool NetherReactorManager::removeReactor(BlockPos const& pos, int dimensionId) {
    return removeReactor(NetherReactor::formatID(pos, dimensionId));
}


} // namespace nr
