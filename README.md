# Warning
This is game is under development.
~~But I'm expect that this warning will be removed soon.~~
But it is now playable.

What is Qtorus
===============
Qtorus is a handy game inspired by Torus for DOS.  The object of Torus is to 
clear falling toruses by aligning toruses with the same color on one of several 
protruding pipes. 

Have you seen this game before? yes, it is a port of DOS(R) torus game.
Yes, yes, it's another port of [wxTorus](https://github.com/berise/wxTorus)

<pre>
<img src=https://github.com/berise/Qtorus/blob/master/doc/qtorus_03.gif width=180> <img src=https://github.com/berise/Qtorus/blob/master/doc/qtorus_01.png width=180> <img src=https://github.com/berise/Qtorus/blob/master/doc/qtorus_02.png width=180>
</pre>

How to play
-----------
Use your arrow keys in your keyboard and align your toruses in a row with same color.
It is also able to use hjkl(VIM keys) for vim lovers.

MOVE : ARROW Keys or hjkl.
Space : to start or pause a game.
ESC to pause & quit a game.

Build
-----
Qtorus is currently using Qt 5.8 version.
So install Qt first and qmake and make.
If you use QCreator then load qtorus.pro and build.

HISTORY
-------
I started to make this game with Qt 0.98 to run in X-Windows
as a clone of dos game named Torus(R). And then port it to Win32 with VWCL uploaded
to sourceforge as a [ktorus](https://sourceforge.net/projects/ktorus).
Later on, I port it to use wxWidget.
