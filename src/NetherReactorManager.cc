#include "NetherReactorManager.h"
#include "NetherReactor.h"
#include "gmlib/mod/recipe/CustomRecipe.h"
#include "ll/api/chrono/GameChrono.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/ListenerBase.h"
#include "ll/api/event/player/PlayerInteractBlockEvent.h"
#include "ll/api/schedule/Scheduler.h"
#include "ll/api/schedule/Task.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/level/BlockSource.h"


ll::event::ListenerPtr          LISTENER_PlayerInteractBlock;
ll::schedule::GameTickScheduler TICK_Scheduler;

namespace nr {
std::string_view const             NetherReactorManager::NetherTowerFilePath = "structure/NetherTower.nbt";
std::unique_ptr<StructureTemplate> NetherReactorManager::mReactorTowerTemplate;
NetherReactorManager&              NetherReactorManager::getInstance() {
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
                auto& player = ev.self();

                // 防抖
                string const& playerName = player.getRealName();
                if (!mClickEventStabilization.contains(playerName)) {
                    std::time_t future                   = std::time(nullptr) + 30;
                    mClickEventStabilization[playerName] = future;
                }
                std::time_t now  = std::time(nullptr);
                auto        last = mClickEventStabilization[playerName];
                if (now < last) {
                    return;
                }

                auto bl = ev.block();
                if (bl && bl->getTypeName() == NetherReactor::Minecraft_NetherReactor) {
                    if (!hasReactor(ev.blockPos(), ev.self().getDimensionId())) {
                        addReactor(NetherReactor(ev.blockPos(), ev.self().getDimensionId())); // 创建反应堆
                    }

                    auto reactor = getReactor(ev.blockPos(), ev.self().getDimensionId());
                    if (reactor) {
                        reactor->activate(ev.self());
                    }
                }
            }
        );

    TICK_Scheduler.add<ll::schedule::RepeatTask>(ll::chrono::ticks(1), [this]() {
        for (auto& reactor : mReactors) {
            auto& sec = reactor.second;
            sec.tick();
            if (sec.isDepleted()) {
                sec._replaceWithObsidian();
                sec._crumblingNetherTower();
                removeReactor(sec); // 移除反应堆
            }
        }
    });
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
