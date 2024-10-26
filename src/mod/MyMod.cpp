#include "mod/MyMod.h"

#include <fstream>
#include <memory>

#include "ll/api/mod/RegisterHelper.h"

#include "NetherReactorManager.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/world/level/levelgen/structure/StructureTemplate.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace my_mod {

static std::unique_ptr<MyMod> instance;

MyMod& MyMod::getInstance() { return *instance; }

bool MyMod::load() {
    getSelf().getLogger().debug("Loading...");

    fs::path structure = getSelf().getModDir() / nr::NetherReactorManager::NetherTowerFilePath;
    if (!fs::exists(structure)) {
        getSelf().getLogger().error("Nether Tower structure file not found: {}", structure.string());
        return false;
    }

    return true;
}

bool MyMod::enable() {
    getSelf().getLogger().debug("Enabling...");

    {
        fs::path structure = getSelf().getModDir() / nr::NetherReactorManager::NetherTowerFilePath;
        if (!fs::exists(structure)) {
            getSelf().getLogger().error("Nether Tower structure file not found: {}", structure.string());
            return false;
        }

        // load structure
        std::ifstream ifs(structure, std::ios::binary | std::ios::ate);
        if (!ifs.is_open()) {
            getSelf().getLogger().error("Failed to open Nether Tower structure file: {}", structure.string());
            return false;
        }

        auto end = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        auto size = std::size_t(end - ifs.tellg());
        if (size == 0) {
            getSelf().getLogger().error("Nether Tower structure file is empty: {}", structure.string());
            return false;
        }

        std::string binaryNbt(size, '\0');
        ifs.read(binaryNbt.data(), size);
        ifs.close();

        // parse structure
        auto nbt = CompoundTag::fromBinaryNbt(binaryNbt)->clone();
        if (nbt->isEmpty()) {
            getSelf().getLogger().error("Failed to parse Nether Tower structure file: {}", structure.string());
            return false;
        }

        nr::NetherReactorManager::mReactorTowerTemplate = StructureTemplate::create("NetherTower", *nbt);
        getSelf().getLogger().info("Loaded Nether Tower structure file: {}", structure.string());
    }

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
