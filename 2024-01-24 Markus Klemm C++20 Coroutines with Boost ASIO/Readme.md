C++20 Coroutines, with Boost ASIO in production: Frightening but awesome
=================

von Markus Klemm
www.markusklemm.net
https://www.linkedin.com/in/markus-klemm-aa3886105/


Abstract
----------

Writing asynchronous code, for example with Boost ASIO, breaks a lot of basic patterns, often resulting in a lambda callback hell or lifetime issues. C++20 coroutines are a major help to reducing this complexity as other languages have done with „async functions“ or so called await semantics.
However if one tries to use out of the box coroutines they are up for a lot of questions and confusion.
 
In this talk aimed at C++ intermediate to expert level audiences I will demonstrate, in conjunction with Boost ASIO, how one can write safer and orders of magnitude more readable asynchronous code thanks to the new compiler and library features.
I will also elaborate the many culprits and surprises that came along when I adapted C++20 coroutines for productive real world usage.


Online
------

[View Slides](talk_keynote.pdf)

[![Video Aufnahme](https://www.youtube.com/live/RBldGKfLb9I)
[View Code](https://github.com/Superlokkus/coroutine)
https://www.scs.stanford.edu/~dm/blog/c++-coroutines.html
https://think-async.com/Asio/asio-1.22.1/doc/asio/overview/composition/coro.html
https://www.boost.org/doc/libs/1_82_0/doc/html/boost_asio/overview/composition/cpp20_coroutines.html
https://stackoverflow.com/q/76950493/3537677
https://stackoverflow.com/q/77060868/3537677
https://stackoverflow.com/q/76922347/3537677