#pragma once

namespace nr {


class NetherReactor {
public:
    NetherReactor()                                = default;
    NetherReactor(NetherReactor&&)                 = delete;
    NetherReactor(const NetherReactor&)            = delete;
    NetherReactor& operator=(NetherReactor&&)      = delete;
    NetherReactor& operator=(const NetherReactor&) = delete;

    static NetherReactor& getInstance();

    void init();
    void release();
};


} // namespace nr
