=========
Bangerang
=========

The final project for Computational Structures in Computer Graphics (CAP4730). It was a team project, but I lost the final product, so only my part of the game is included here, which includes the basic physics engine and controls.

Other weapons like the missles mentioned below are missing. It's also missing the networking feature that allows the 2nd player to join in on their own screen. And finally, all the cool (I assume) graphics are missing, so everything is just simple polygons covered in simple textures.

However, what remains is still pretty fun. The style engine is also included, so you can zoom around doing tricks and beating the hell out of your idle opponent :)

Gameplay
-----------
The main goal of the game is to knock your opponent out of the arena.  The more damage that you take, the farther you will go flying when you are attacked, making it easier for you to get knocked out.  Each car is armed with a basic tackle accelerator that can give the car a short burst of speed when activated.  Don’t use it recklessly though, because every time you try to tackle and miss there will be about a one second cooldown where your car will come to a stop and not move, making you an easy target.  Also remember that getting hit in the back does twice the damage as a side collision, and getting hit in the front does almost no damage at all.  To add to the mayhem, missles and mines have been added to the arena to help you blast your opponent away.  However, you won’t just be handed these amazing weapons.  Instead you’ll need to earn them by doing cool tricks in the arena such as jumping over gaps or doing spins in the air.  Only by mastering the arena will you truly be able to master Bangerang.

Controls
________
* A, D         ➜ Turn left/right, respectively.
* W, S         ➜ Accelerate/decelerate, respectively.
* J            ➜ Jump.
* Space        ➜ Lunge forward to attack.

Create The Executable
-----------
To run the makefile, you'll need g++, which you can easily get by installing MinGW.

This game's graphics are built using freeglut, a modern replacement for the original OpenGL GLUT library. You can see how to install freeglut to work with MinGW at https://www.transmissionzero.co.uk/computing/using-glut-with-mingw/.

Once you have g++ and freeglut working, just run the makefile to create the Windows executable
