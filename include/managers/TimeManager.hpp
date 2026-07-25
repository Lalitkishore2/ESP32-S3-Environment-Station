#ifndef TIME_MANAGER_HPP
#define TIME_MANAGER_HPP

#include <ctime>
#include <cstdint>

namespace managers {

class TimeManager {
public:
    TimeManager();
    void init();
    bool syncNTP();
    void getFormattedTime(char* timeBuffer, size_t bufLen) const;
    void getFormattedDate(char* dateBuffer, size_t bufLen) const;
    [[nodiscard]] bool isSynced() const;

private:
    bool synced_;
};

} // namespace managers

#endif // TIME_MANAGER_HPP
