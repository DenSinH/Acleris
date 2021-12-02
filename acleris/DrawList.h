#pragma once

#include "Acleris.h"

#include <future>
#include <queue>


struct FragmentImplBase {
    virtual ~FragmentImplBase() = default;

    virtual void DrawImpl(Acleris& acleris) = 0;
};


struct DrawList {
    DrawList(Acleris& acleris) : acleris(acleris) {

    }

    Acleris& acleris;

private:
    static constexpr bool SingleThread = false;

    std::queue<std::pair<std::future<void>, std::unique_ptr<FragmentImplBase>>> futures{};

public:
    void operator<<(std::unique_ptr<FragmentImplBase>&& frag) {
        if constexpr(SingleThread) {
            frag->DrawImpl(acleris);
        }
        else {
            futures.emplace(
                std::async(std::launch::async, &FragmentImplBase::DrawImpl, frag.get(), std::ref(acleris)),
                std::move(frag)
            );
        }
    }

    void Wait() {
        while (!futures.empty()) {
            futures.back().first.wait();
            futures.pop();
        }
    }
};