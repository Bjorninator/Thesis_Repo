#include <iostream>
#include <chrono>
#include <thread>
 
auto now() { return std::chrono::steady_clock::now(); }
 
auto awake_time() {
    using std::chrono::operator""ms;
    return now();
}
 
int main()
{
    using namespace std::chrono;
    
    typedef time_point<steady_clock, milliseconds> MyTimePoint;
    MyTimePoint startTimePoint = time_point_cast<MyTimePoint::duration>(steady_clock::time_point(steady_clock::now()));
    MyTimePoint endTimePoint = startTimePoint;
    startTimePoint += milliseconds(10);

 //   std::cout << steady_clock::to_time_t(startTimePoint) << std::endl;
 //   std::cout << steady_clock::to_time_t(endTimePoint) << std::endl;

    duration<double, std::milli> elap {startTimePoint - endTimePoint};
    std::cout << "Deadline in " << elap.count() << " ms\n";

    for (size_t i = 0; i < 10; i++)
    {   
        duration<double, std::milli> elap {startTimePoint - steady_clock::now()};
        std::cout << "Deadline in " << elap.count() << " ms\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
}

void other()
{   
    using namespace std::chrono;

    std::cout << "Hello, waiters...\n" << std::flush;
    const auto start {now()};
    std::this_thread::sleep_until(awake_time());
    duration<double, std::milli> elapsed {now() - start};
    std::cout << "Waited " << elapsed.count() << " ms\n";

    auto first {now()};
    duration<double, std::milli> second {first - start}; 
    
    std::cout << "Waited " << second.count() << " ms\n";

    auto time_point = system_clock::now();
    std::time_t now_c = system_clock::to_time_t(time_point);
    std::cout << std::ctime(&now_c) << "\n";
}
