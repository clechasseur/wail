# Wail: a Marathon Sound Editor for Classic MacOS

Back when I was young and full of ~~memory leaks~~ energy, I wrote some utilities to edit the data files for the [Marathon 1/2/Infinity games](https://marathon.bungie.org/indexclassic.html) for [Classic MacOS](https://en.wikipedia.org/wiki/Classic_Mac_OS).
The games were made by [Bungie](https://www.bungie.net/) (yes, that Bungie) and were very popular on the old Macintosh computers back then.
Eventually, the Marathon 2 source code was released as open source and this gave birth to the [Aleph One](https://alephone.lhowon.org/) project, which allows you to play the old Marathon games on today's systems.
Marathon 2 is also available to purchase [for Xbox](https://www.xbox.com/en-US/games/store/marathon-durandal/BNQLQBLKR2MN) (for some reason).

My last Marathon project was called **Wail** and it was a UI editor for the Marathon Sound data file format.
It was the second such utility I worked on and also my first project using C++ (whereas previously I mostly worked in Pascal).
I wrote Wail during my [Cégep](https://en.wikipedia.org/wiki/CEGEP) years, before I studied Computer Science and learned important things like ~~Smalltalk~~ using source control and writing tests.
I maintained Wail for a few years and eventually released the source code as free software under a [BSD-like license](./Wail_2.5b1_source/LICENSE).
The program was developed using [CodeWarrior](https://en.wikipedia.org/wiki/CodeWarrior) and the [PowerPlant framework](https://en.wikipedia.org/wiki/PowerPlant) and has been used by a few people working on custom Marathon makeovers.

Recently, I decided to check if the source code was still available online since I lost all trace of it when I lost my old [Macintosh G3 All-in-one](https://everymac.com/systems/apple/powermac_g3/specs/powermac_g3_233_aio.html).
I was surprised to find it online on [a site at lhowon.org](https://citadel.lhowon.org/litterbox/utilities/infinity.html), so I grabbed the [archives](./Archives) for the program as well as the [source code](./Wail_2.5b1_source) and pushed them to this repo.
The files are pretty much as-is, except I changed the line endings so that they render correctly on GitHub (Classic MacOS used `CR` as like endings, proving that if there are many possible ways of doing something, humans will find and use all possible ways, and then some).
Some files will be unreadable because they are either binary files (like `*.mcp` files, which are [CodeWarrior](https://en.wikipedia.org/wiki/CodeWarrior) project files) or because everything was in the [resource fork](https://en.wikipedia.org/wiki/Resource_fork) which isn't really accessible on modern systems.

So if you wanted to see what it was like writing C++ before we had those new-fangled goodies like smart pointers and _Insert Spaces_ support, now's your chance! 👴

![The Old Man Yells At Cloud meme](https://github.com/clechasseur/wail/assets/1435551/fd897ef6-aa50-4e03-b923-b1701b970fd0)
