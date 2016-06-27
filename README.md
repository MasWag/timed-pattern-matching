Features
========
* Benchmark of Naive and Boyer-Moore type algorithms.
* The main implementations are in naive.hh and timed-boyer-moore.hh.
* naive.cc and timed-boyer-moore.cc are sample codes.

Requirements
============
* C++ compiler (C++11 support is required)
* boost

Usage
=====

> ./naive -t test_case < some_timed_word

* The programs read timed words from stdin.
* "test_case" is a number that indicates the pattern. When you want to use another pattern, please edit naive.cc or timed-boyer-moore.cc.

Reference
=========

* [Masaki Waga](http://www-mmm.is.s.u-tokyo.ac.jp/~mwaga/), [Takumi Akazaki](http://www-mmm.is.s.u-tokyo.ac.jp/~takazaki/), and [Ichiro Hasuo](http://www-mmm.is.s.u-tokyo.ac.jp/~ichiro/) **A Boyer-Moore Type Algorithm for Timed Pattern Matching** To appear in Proc. FORMATS 2016. [[preprint pdf](https://arxiv.org/abs/1606.07207#)]
