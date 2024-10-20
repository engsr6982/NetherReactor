#include "NetherReactor.h"

namespace nr {
NetherReactor& NetherReactor::getInstance() {
    static NetherReactor instance;
    return instance;
}

void NetherReactor::init() {}
void NetherReactor::release() {}


} // namespace nr
