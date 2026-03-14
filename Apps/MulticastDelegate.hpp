#ifndef LAUNCHERA_MULTICASTDELEGATE_HPP
#define LAUNCHERA_MULTICASTDELEGATE_HPP

#include <vector>
#include <functional>
#include <algorithm>

template<typename... Args>
class MulticastDelegate {
public:
    using FuncType = std::function<void(Args...)>;

    // 添加委托
    void operator+=(const FuncType& f) {
        _delegates.push_back(f);
    }

    // 移除委托（地址相同的第一个）
    void operator-=(const FuncType& f) {
        auto it = std::find_if(_delegates.begin(), _delegates.end(),
            [&](const FuncType& fun) {
                // std::function不可直接比较，只能比较目标地址
                return fun.target_type() == f.target_type();
            });
        if (it != _delegates.end())
            _delegates.erase(it);
    }

    // 调用所有委托
    void operator()(Args... args) const {
        for (const auto& f : _delegates)
            f(args...);
    }

    // 可选: 清空所有委托
    void clear() {
        _delegates.clear();
    }

private:
    std::vector<FuncType> _delegates;
};

#endif //LAUNCHERA_MULTICASTDELEGATE_HPP