Features
========
* Benchmark of Naive, BM, and FJS type algorithms.
* The main implementations are in naive.hh, timedBM.hh, and timedFJS.hh.
* naive.cc, timedBM.cc, timedFJS are sample codes.

Requirements
============
* C++ compiler (C++11 support is required)
* boost

Usage
=====

> ./naive -t test_case < some_timed_word

* The programs read timed words from stdin.
* "test_case" is a number that indicates the pattern. When you want to use another pattern, please edit naive.cc, timedBM.cc or timedFJS.cc.

Reference
=========

* [Masaki Waga](http://group-mmm.org/~mwaga/), [Ichiro Hasuo](http://group-mmm.org/~ichiro/), and [Kohei Suenaga](http://www.fos.kuis.kyoto-u.ac.jp/~ksuenaga/) **Efficient Online Timed Pattern Matching by Automata-Based Skipping** To appear in Proc. FORMATS 2017.
* [Masaki Waga](http://group-mmm.org/~mwaga/), [Takumi Akazaki](http://group-mmm.org/~takazaki/), and [Ichiro Hasuo](http://group-mmm.org/~ichiro/) **A Boyer-Moore Type Algorithm for Timed Pattern Matching** In Proc. FORMATS 2016. [[preprint pdf](https://arxiv.org/abs/1606.07207#)]
