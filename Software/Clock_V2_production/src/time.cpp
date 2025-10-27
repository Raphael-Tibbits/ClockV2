#include "time.hpp"

volatile uint64_t miliseconds = 0;

datetime mils_to_datetime(uint64_t ms){
    datetime t;
}

class Mr_wolf{
public:
    Mr_wolf(datetime boot){
        boot_datetime = boot;
    };

    ~Mr_wolf(){};

    inline uint64_t milis(){
        return this->miliseconds;
    }

    date whats_the_date(){
        return 
    }
    

private:
    static datetime boot_datetime;
    uint64_t miliseconds;
};
