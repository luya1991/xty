
# About xty

'xty' is an impl of a msg-based multi-thread communication c/s model.

To get started, you need to write a 'Makefile' for this repo. If you do
not want to write 'Makefile' by yourself, you can send E-mail to ginigit.

# About Author

This repo is built by ginigit, a student of UESTC, China.
For further information please send E-mail to 839256767@qq.com.

# Some anecdotes

When I first created this repository, I just wanted to learn something
about c++ programming and linux system calls. Therefore, I never really
thought about giving this repo a cool name. Yesterday, I had fun chit-
chatting with my old friends, and all the memories flashed back in my
mind. It occurs to me, why not name the repo after one of those idiots?!

'xty' is the name initials of my old friend Xu Tong Yang back in college.
He is a typical 'Gao Fu Shuai'(tall & rich & handsome, a winner in life).
He also has a very good temper that he won't get mad when we play tricks
on him. So, we set up a small gang called 'Messing with xty Group'(MWXG)
just to mess with him. It did add a lot of fun to our daily life.

This repo is dedicated to all the members of 'MWXG'. Anytime when I work
on this repo, I sure will remember them and the old happy time.

c++ rules all.

							        ginigit
							     2016-01-27

# Unsolved Problems

1. In msg server epoll thread, I found that when I set 'EPOLLET' flag
for epoll's event loop, there will be some packet loss. This might due
to the slowness of the msg deserialization and msg queue pushing.

2. SIGSEGV pops up in fundamental facility's clean up method when it is
about to clean up shared pool. I still don't know why the debugger says
that it cannot find where the code is(unknown addr ?? in addr space).

3. Heart-beat mechanism is needed for msg server to clean-up any dead
clients' information dynamically. I did write 'heart_interval' para in
the configuration file, but I did not impl the actual function.

4. Logging part is a total mess...
							        ginigit
							     2016-03-05

# Change history

2016-03-05
Initial commitment
