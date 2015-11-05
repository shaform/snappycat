#include <cstring>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <snappy.h>

int32_t rawReadInt(std::istream &in) {
  int b1 = in.get();
  int b2 = in.get();
  int b3 = in.get();
  int b4 = in.get();
  if ((b1 | b2 | b3 | b4) < 0) {
    return -1;
  }
  return ((b1 << 24) + (b2 << 16) + (b3 << 8) + (b4 << 0));
}

void uncompress(std::istream &in, FILE *out) {
  while (in) {
    int32_t uncompress_size = rawReadInt(in);
    if (uncompress_size == 0) {
      // empty block
      rawReadInt(in);
      continue;
    } else if (uncompress_size < 0) {
      // end of file
      break;
    }

    int32_t compress_size = rawReadInt(in);
    std::vector<char> buffer(compress_size);
    in.read(&buffer[0], compress_size);

    std::string uncompressed;
    snappy::Uncompress(&buffer[0], compress_size, &uncompressed);
    std::fwrite(uncompressed.c_str(), sizeof(char), uncompress_size, out);
  }
}

int main(int argc, char **argv) {
  if (argc == 1) {
    uncompress(std::cin, stdout);
  } else {
    for (int i = 1; i < argc; ++i) {
      std::ifstream snappy_file(argv[i], std::ios::in | std::ios::binary);
      uncompress(snappy_file, stdout);
    }
  }
  return 0;
}
