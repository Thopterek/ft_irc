<img width="672" height="186" alt="ft_irc" src="https://github.com/user-attachments/assets/3650840e-c5bf-4d04-ba4e-18a69cecba3f" />

<h1>IRC Server working with reference irssi client</h1>

done by myself (<strong>@Thopterek</strong>), <strong>@Rexbrainz</strong> and <strong>@cuhlig</strong>, below you can see the basic showcase

<img width="1709" height="471" alt="showcase" src="https://github.com/user-attachments/assets/34766e21-19d1-428e-93b8-2b99fb7d7bdb" />

<h2>Following rules required in 42 school with sweet additions from us</h2>

<h3>Project required to follow the IRC protocol, allowing us to create a working server.</h3>

Handling multiple connections at the same time, without blocking sockets

Handling various IRC commands

PASS / USER / NICK: to register the user with all of the informations

PRIVMSG: allowing for private message and group chats

We made a bonus that uses the DCC protocol used by irssi, allowing bot to transfer chosen files and act as client.

<h6>Used reference</h6>
https://datatracker.ietf.org/doc/html/rfc2813

https://www.rfc-editor.org/rfc/rfc2812.html

https://standards.nasa.gov/sites/default/files/standards/NASA/Baseline/0/nasa-gb-871913.pdf

https://medium.com/@mohcin.ghalmi/irc-server-internet-relay-chat-bd08e4f469f8


/connect -4 -! -nocap -noautosendcmd 10.12.7.9 6667 123


10.12.7.9
nc -v 10.12.4.X 6667
PASS 123\r\n
NICK test\r\n
USER test * 0 real\r\n

make irssi-docker

also control n or p in irssi client to swap taps

USER test * 0 :real name^M
