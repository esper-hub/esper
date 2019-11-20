#ifndef OBSERVED_H
#define OBSERVED_H

#include <SmingCore.h>


template<typename T>
class Observed {
public:
    using Callback = Delegate<void(const T& value)>;

    explicit constexpr Observed(const T& initialValue,
                                const Callback& callback) :
            value(initialValue),
            callback(callback) {
    }

    inline operator const T&() const {
        return this->value;
    }

    inline bool set(const T& value) {
        if (this->value == value) {
            return false;
        }

        this->value = value;

        System.queueCallback([=]() {
            this->callback(this->value);
        });

        return true;
    }

private:
    T value;
    const Callback callback;
};


#endif // ESPER_OBSERVED_H
