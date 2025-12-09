#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <print>
#include <locale>
#include <string>


int main(){
  std::println("Ciao, {}!", "Mondo");
  const int INTERVAL = 100;
  long dial{50};
  long answer{};
  std::string cl{}; // current line
  while(std::getline(std::cin, cl)){
    // int rotation;
    // std::from_chars()
    auto rotation = std::strtol(cl.c_str()+1, nullptr, 10);
    if( cl[0] == 'L'){
      rotation *= -1;
    }
    answer += std::abs(rotation/INTERVAL);
    rotation = rotation % INTERVAL;
    dial += rotation;
    if( dial != rotation && /* if dial was 0, don't count it twice */
        dial <= 0 ||
        dial >= 100){
      ++answer;
    }
    dial = (INTERVAL + dial) % INTERVAL;
    // if(dial == 0){ ++answer; }
    if(dial < 0)
      std::cout << dial << std::endl;
  }
  std::cout << "Answer: " << answer << std::endl;
}
