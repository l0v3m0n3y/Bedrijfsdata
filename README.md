# Bedrijfsdata
api for bedrijfsdata.nl site for checking personal data
# main
```cpp
#include "Bedrijfsdata.h"
#include <iostream>

int main() {
   Bedrijfsdata api;

    auto email = api.check_email("email").then([](json::value result) {
        std::cout << result<< std::endl;
    });
    email.wait();
    
    return 0;
}
```

# Launch (your script)
```
g++ -std=c++11 -o main main.cpp -lcpprest -lssl -lcrypto -lpthread -lboost_system -lboost_chrono -lboost_thread
./main
```
