<h1>IRC Server working with reference irssi client</h1>

done by myself (<strong>@Thopterek</strong>), <strong>@Rexbrainz</strong> and <strong>@cuhlig</strong>, below you can see the basic usage showcase

<img width="1709" height="471" alt="showcase" src="https://github.com/user-attachments/assets/34766e21-19d1-428e-93b8-2b99fb7d7bdb" />

<h2>Following rules required in 42 school with sweet additions from us</h2>

We also made a bonus that uses the DCC protocol used by irssi, allowing bot to transfer chosen files and act as client.

<img width="819" height="158" alt="DCC" src="https://github.com/user-attachments/assets/1972460a-8bba-42eb-a93e-cb8ff05da1c2" />

<h3>Project required to follow the IRC protocol, allowing us to create a working server.</h3>

Handling multiple connections at the same time, without blocking sockets

Handling various IRC commands per the subject and bonus
* PASS / USER / NICK: to register the user with all of the informations
* PRIVMSG: allowing for private message and group chats
* Operations related to channels and its operators:
  * JOIN: allowing to create (first person) or join existing channel
  * INVITE: inviting another user to channel
  * TOPIC: changing the channel descritption
  * KICK: ban a connected client from a channel
  * MODE, with varius sub modes setting (prefix +) and removing them (prefix -):
     * i: channels is set to invite only
     * t: allowing non operators to change topic
     * k: using the password to join the channel
     * o: give operator privilage to another user (usage of MODE)
     * l: limiting the number of clients on channel
* Bot that handles the file transfer and gets all needed information for DCC
* Setting up environment through docker with irssi for ease of use 

<img width="1500" height="446" alt="talk" src="https://github.com/user-attachments/assets/b92692ae-9fad-47eb-bf02-b1e4308e02e6" />

<h5>Makefile instructions</h5>

* make: to compile the server
  * ./ircserv <em>port-number-to-be-used-by-server</em> <em>server-password-required-to-connect</em>
* make bot: to compile the bot
  * ./ircbot, and then go through shown instructions to set it up:
      * choosing the name for the bot and channel he will create
      * file to transfer or txt file which content you want to receive in private message
      * description that will help understand to other users what they receive
* make irssi-docker: to start the docker with installed irssi
  * make new-term: using running docker to create new terminals with irssi
* make clean-docker / make fcleanbot / make fclean: to cleanup after use

<img width="672" height="186" alt="ft_irc" src="https://github.com/user-attachments/assets/3650840e-c5bf-4d04-ba4e-18a69cecba3f" />

<h6>Used reference</h6>

[RFC 2813, talking about the IRC server protocol](https://datatracker.ietf.org/doc/html/rfc2813)

[RFC 2812, describing the IRC client protocol](https://www.rfc-editor.org/rfc/rfc2812.html)

[NASA-GB-871913, taking the good practices for safety critical code, keep server running at all costs](https://standards.nasa.gov/sites/default/files/standards/NASA/Baseline/0/nasa-gb-871913.pdf)

[Great general introduction to the project](https://medium.com/@mohcin.ghalmi/irc-server-internet-relay-chat-bd08e4f469f8)

[More modern take on client protocol, with some proper updates](https://modern.ircdocs.horse/)

I would love to add here the irssi documentation but sadly its bad.

If you would like to make this project or similiar choose your reference client wisely.

IF you are going to choose the irssi [go for the source code, and good luck!](https://github.com/irssi/irssi)
