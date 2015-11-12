#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <snappy.h>
#include <snappy-sinksource.h>

class StreamSink : public snappy::Sink {
public:
  StreamSink(std::ostream &out) : os_(out) {}
  ~StreamSink() {}
  void Append(const char *bytes, size_t n) { os_.write(bytes, n); }

private:
  std::ostream &os_;
};

class StreamSource : public snappy::Source {
public:
  static constexpr size_t max_buffer_size = 32 * 1024;

  StreamSource(std::istream &in, size_t n)
      : is_(in), left_(n), buf_size_(0), buf_begin_(buffer_) {}

  ~StreamSource() {}

  size_t Available() const { return left_; }

  const char *Peek(size_t *len) {
    if (Available() > 0 && buf_size_ == 0) {
      Get();
    }
    *len = buf_size_;
    return buf_begin_;
  }

  void Skip(size_t n) {
    if (n <= 0)
      return;

    assert(n <= left_);
    left_ -= n;

    if (buf_size_ > n) {
      buf_begin_ += n;
      buf_size_ -= n;
    } else {
      n -= buf_size_;
      buf_size_ = 0;
      buf_begin_ = buffer_;
      if (n > 0) {
        is_.ignore(n);
      }
    }
  }

  void Get() {
    size_t sz = std::min(max_buffer_size, left_);
    if (sz > 0) {
      is_.read(buffer_, sz);
      buf_begin_ = buffer_;
      buf_size_ = is_.gcount();
    }
  }

private:
  std::istream &is_;
  char buffer_[max_buffer_size];
  size_t left_;
  size_t buf_size_;
  char *buf_begin_;
};

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

void uncompress(std::istream &in, std::ostream &out) {
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
    StreamSource ssrc(in, compress_size);
    StreamSink ssink(out);
    snappy::Uncompress(&ssrc, &ssink);
  }
}

int main(int argc, char **argv) {
  if (argc == 1) {
    uncompress(std::cin, std::cout);
  } else {
    for (int i = 1; i < argc; ++i) {
      std::ifstream snappy_file(argv[i], std::ios::in | std::ios::binary);
      uncompress(snappy_file, std::cout);
    }
  }
  return 0;
}
