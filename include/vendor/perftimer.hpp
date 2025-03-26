//
// A general purpose perf timer in a header-only library with delayed logging.
//

#pragma once

#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <string>

namespace perftimer {
    constexpr auto VERSION = "0.6.4";

    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    struct PerfTimer {
    private:
        constexpr static auto billions = 1'000'000'000;
        constexpr static auto millions = 1'000'000;

        TimePoint t0;
        TimePoint t1;

        std::string name;
        

    public:
        // Construct with a name for this timer.  Make it unique if used in a map
        PerfTimer(const std::string timer_name) : name(timer_name) { };

        // log messages in a buffered way to delay console log delays. use like cout << "message"
        std::ostringstream log;

        // set the show precision, defaults to 9 places
        int prec = 9;

        // start or re-start the timer; invoke end to capture the duration
        void start() {
            t0 = Clock::now();
        }

        void stop() {
            t1 = Clock::now();
        }


        // returns the nanos between t0 and t1
        auto get_duration() {
            const std::chrono::duration<double, std::nano> dur = t1 - t0;
            return dur;
        }

        // show the duration
        void show_duration(const std::string& message = ": process took: ") {
            auto dur = get_duration();
            if (dur.count() > billions) {
                std::cout << name << message << std::setprecision(prec) << dur.count() / billions << " seconds" << '\n';
            } else if (dur.count() > millions) {
                std::cout << name << message << std::setprecision(prec) << dur.count() / millions << " milliseconds" << '\n';
            } else {
                std::cout << name << message << std::setprecision(prec) << dur.count() / 1'000 << " microseconds" << '\n';
            }

        }

    };
}
