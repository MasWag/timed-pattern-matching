#pragma once

#include <cstdio>
#include <deque>
#include <stdexcept>
#include <utility>

#include "types.hh"

void getOne(FILE* file, std::pair<Alphabet, double> &p) {
  fscanf(file, " %c %lf\n", &p.first, &p.second);
}

void getOneBinary(FILE* file, std::pair<Alphabet, double> &p) {
  fread(&p.first, sizeof(char), 1, file);
  fread(&p.second, sizeof(double), 1, file);
}

template<class T>
class LazyDeque : public std::deque<T>
{
private:
  std::size_t front = 0;
  const std::size_t N;
  FILE* file;
  void (*getElem)(FILE*, std::pair<Alphabet, double>&);
public:
  LazyDeque (const std::size_t N, FILE* file, bool isBinary = false) : N(N), file(file) {
    getElem = isBinary ? getOneBinary : getOne;
  }
  T operator[](std::size_t n) {
    if (n < front || n >= N) {
      throw std::out_of_range("thrown at LazyDeque::operator[] ");
    }
    const std::size_t indInDeque = n - front;
    const int allocTimes = indInDeque - std::deque<T>::size() + 1;
    for (int i = 0; i < allocTimes; i++) {
      T elem;
      getElem(file, elem);
      this->push_back(elem);
    }
    return std::deque<T>::at(indInDeque);
  }
  T at(std::size_t n) {
    *this[n];
  }
  std::size_t size() const {
    return N;
  }
  void setFront(std::size_t newFront) {
    if (newFront < front) {
      throw std::out_of_range("thrown at LazyDeque::setFront ");
    }
    const std::size_t eraseSize = std::min(newFront - front, std::deque<T>::size());
    const int readTimes = (newFront - front) - eraseSize;
    front = newFront;
    this->erase(this->begin(), this->begin() + eraseSize);
    for (int i = 0; i < readTimes; i++) {
      T elem;
      getElem(file, elem);
    }
  }
};
