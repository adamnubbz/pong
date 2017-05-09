Pong! by Matthew Guthrie, Niko Takeyasu, and Adam Hudson for CSC-213

Instructions for downloading and executing Pong!

Clone the repository from https://github.com/adamnubbz/pong.git by moving
to an acceptable directory and entering `git clone
https://github.com/adamnubbz/pong.git`. After downloading, you will need to
have one machine operating as the server and identify the IP address for
hosting. Move to the server directory, and list the IP address by entering
`ip addr show`; then enter `make` to compile the server. Finally, enter
`./server` to start the server.

On two seperate machines, navigate to the client directory. Once there,
enter `make` to compile the client, then enter `./client` to start the
client. The program will prompt the user for the IP address; enter the IP
address of the host server. Once both players connect, the game will
begin. The first player to connect will be player 1 (red paddle) on the
right, player 2 (blue paddle) on the left.

Controls for operating the paddle:
Up Key = Move up
Down Key = Move down

