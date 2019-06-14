//! clang++ -std=c++14 -Wall -Wextra start_with_a.cc
#include <sys/time.h>

#include <cstdio>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

constexpr char kNicknamesTextPath[] = "nicknames.txt";

class Timer {
 public:
  Timer(const std::string& tag) : tag_(tag) {
    timeval tv;
    gettimeofday(&tv, nullptr);
    begin_ = tv.tv_sec + tv.tv_usec / 1E6;
    std::cout << "==== Begin: " << tag_ << " ====" << std::endl;
  }

  ~Timer() {
    timeval tv;
    gettimeofday(&tv, nullptr);
    double end = tv.tv_sec + tv.tv_usec / 1E6;

    std::cout << "Elapsed: " << std::setprecision(3)
              << end - begin_ << " sec" << std::endl;
    std::cout << "==== End: " << tag_ << " ====" << std::endl;
  }

 private:
  double begin_;
  std::string tag_;
};

int main() {
  std::vector<std::string> nicknames;
  {
    Timer t("Read nicknames");
    std::fstream nickname_file(kNicknamesTextPath);
    if (nickname_file.fail()) {
      std::cerr << "file not found:" << kNicknamesTextPath << std::endl;
      return 1;
    }
    while (true) {
      int id;
      std::string nickname;
      nickname_file >> id >> nickname;
      if (nickname_file.eof())
        break;
      if (nickname.empty()) {
        std::cerr << "unexpected error in the file" << std::endl;
        return 1;
      }
      nicknames.emplace_back(std::move(nickname));
    }
  }

  {
    Timer t("sort (while it's already sorted by nickname)");
    std::sort(nicknames.begin(), nicknames.end());
  }

  {
    Timer t("print a-nicknames");
    for (auto iter = nicknames.begin(); (*iter)[0] == 'a'; ++iter) {
      std::cout << *iter << std::endl;
    }
  }

  return 0;
}
