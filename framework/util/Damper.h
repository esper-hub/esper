
#ifndef DAMPER_H
#define DAMPER_H

template<uint16_t damper_time = 0>
class Damper {
public:
    Damper() :
            lastChange(RTC.getRtcNanoseconds() / 1000000) {
    }

    bool isDamped() {
        if (damper_time == 0) {
            return false;
        }

        const uint64_t now = RTC.getRtcNanoseconds() / 1000000;

        if (this->lastChange + ((uint64_t) damper_time) > now) {
            return true;
        }

        this->lastChange = now;

        return false;
    }

private:
    uint64_t lastChange;
};

#endif
