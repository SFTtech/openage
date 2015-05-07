Any file in this project that doesn't state otherwise, and isn't listed as an
exception below, is Copyright 2013-2015 The openage authors, and licensed
under the terms of the GNU General Public License Version 3, or
(at your option) any later version ("GPL3+").
A copy of the license can be found in [legal/GPLV3](/legal/GPLv3).

_the openage authors_ are:

| Full name                   | aliases                     | E-Mail                                |
|-----------------------------|-----------------------------|---------------------------------------|
| Jonas Jelten                | TheJJ                       | jj@sft.mx                             |
| Michael Enßlin              | mic_e                       | michael@ensslin.cc                    |
| Andre Kupka                 | freakout                    | kupka@in.tum.de                       |
| Frank Schmidt               | gellardo                    | rubiccuber@googlemail.com             |
| Markus Otto                 | zuntrax                     | otto@fs.tum.de                        |
| Sascha Vincent Kurowski     | svkurowski                  | svkurowski@gmail.com                  |
| James Mintram               | JimmyJazz                   | jamesmintram@gmail.com                |
| Martin McGrath              | MartinMcGrath               | mcgrath.martin@gmail.com              |
| René Kooi                   | goto-bus-stop               | rene@kooi.me                          |
| Markus Elfring              | elfring                     | elfring@users.sourceforge.net         |
| Jimmy Berry                 | boombatower                 | jimmy@boombatower.com                 |
| João Roque                  | joaoroque                   | joaoroque@gmail.com                   |
| Julius Michaelis            | jcaesar                     | gitter@liftm.de                       |
| Matthias Bogad              | delirium, masterofjellyfish | matthias@bogad.at                     |
| Oliver Fawcett-Griffiths    | ollyfg                      | olly@ollyfg.com                       |
| Ross Murray                 | rossmurray                  | rm@egoorb.com                         |
| Alexandre Arpin             | AlexandreArpin              | arpin.alexandre@gmail.com             |
| Henry Snoek                 | snoek09                     | snoek09@gmail.com                     |
| Gabriel Scherer             | gasche                      | gasche.dylc@gmail.com                 |
| Austin Eyler                | awestin1                    | awestin1@gmail.com                    |
| Francisco Demartino         | franciscod                  | demartino.francisco@gmail.com         |
| Peter Piwowarski            | oldlaptop                   | oldlaptop654@aol.com                  |
| Charles Pigott              | LordAro                     | charlespigott@googlemail.com          |
| Andrew Eikum                | ColdPie1                    | coldpies@gmail.com                    |
| Michael Sebastiyan          | BugExplorer                 | sebastiyan.michael@outlook.com        |
| Adam Miartus                | miartad                     | adam.miartus@gmail.com                |
| Benoît Legat                | blegat                      | benoit.legat@gmail.com                |
| James Hagborg               | blucoat                     | jameshagborg@gmail.com                |
| Prashanth Jonnala           | jprashanth                  | prashanth.neo@gmail.com               |
| Jonathan Remnant            | Jon0                        | jono4728@gmail.com                    |
| Sam Schetterer              | schets                      | samschet@gmail.com                    |
| Georg Kilzer                | leper                       | leper@wildfiregames.com               |
| Florian Erler               | ethon                       | ethon@ethon.cc                        |
| Michał Janiszewski          | janisozaur                  | janisozaur+openage@gmail.com          |
| Lautaro Nahuel De León      | lndl                        | laudleon@gmail.com                    |
| Robin Kreis                 | rkreis                      | r.kreis@uni-bremen.de                 |
| Shion Ryuu                  | shion                       | shionryuu@outlook.com                 |
| Jonas Borchelt              | riotjones                   | jonasbr-github@bellatrix.uberspace.de |
| Jon Gelderloos              | jgelderloos                 | jgelderloos@gmail.com                 |
| Emmanuel Gil Peyrot         | Link Mauve                  | linkmauve@linkmauve.fr                |
| Danilo Bargen               | dbrgn                       | mail@dbrgn.ch                         |
| Niklas Fiekas               | niklasf                     | niklas.fiekas@tu-clausthal.de         |
| Charles Gould               | charlesrgould               | charles.r.gould@gmail.com             |
| Wilco Kusee                 | detrumi                     | wilcokusee@gmail.com                  |
| Timo Haas                   | alue-42                     | haastimo@gmx.de                       |

If you're a first-time commiter, add yourself to the above list. This is not
just for legal reasons, but also to keep an overview of all those nicknames.

For some authors, the full names and/or e-mail addresses are unknown. They have
been marked by "?". Luckily, those author's contributions are only small typo
fixes, so no copyright concerns should arise from this.
If your info is missing, wrong, or you want it to be removed for whatever
reason, please contact us.

A full list of all openage authors ("contributors") can also be determined
from the VCS, e.g. via `git shortlog -sne`, or conveniently looked up on
[the GitHub web interface](https://github.com/SFTtech/openage/graphs/contributors).

Details on individual authorships of files can be obtained via the VCS,
e.g. via `git blame`, or the GitHub web interface.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License Version 3 for more details.

If you wish to include a file from openage in your project, make sure to
include all required legal info. The easiest way to do this would probably
be to include a copy of this file (`copying.md`), and to leave the file's
copyright header untouched.

Per-file license header guidelines:

In addition to this file, to prevent legal caveats, every source file *must*
include a header.

**openage-native** source files, that is, files that were created by
_the openage authors_, require the following one-line header, preferrably in
the first line, as a comment:

    Copyright 20XX-20YY the openage authors. See copying.md for legal info.

`XXXX` is the year when the file was created, and `YYYY` is the year when the
file was last edited. When editing a file, make sure the last-modification year
is still correct.

**3rd-party** source files, that is, files that were taken from other open-
source projects, require the following, longer header:

    This file was ((taken|adapted)|contains (data|code)) from $PROJECT,
    Copyright 1337-2013 Your Mom.
    It's licensed under the terms of the 3-clause BSD license.
    < any amount of lines of further legal information required by $PROJECT,
      such as a reference to a copy of the $PROJECT's README or AUTHORS file >
    < if third-party files from more than the one project were used in this
      file, copy the above any number of times >
    (Modifications|Other (data|code)|Everything else) Copyright 2014-2014 the openage authors.
    See copying.md for further legal info.

For even more details, see the [regular expressions](/py/openage/codecompliance/legal.py).

In addition to the openage header, the file's original license header should
be retained if in doubt.

The "license" line is required only if the file is not licensed as
"GPLv3 or higher".

Authors of 3rd-party files should generally not be entered in the
"openage authors" list.

All 3rd-party files **must** be included in the following list:

List of all 3rd-party files in openage:

From [cabextract/libmspack](http://www.cabextract.org.uk/) ([LGPL 2.0](/legal/LGPLv2.0))

 - `py/openage/convert/cabextract/lzxd/lzxd.cpp`
 - `py/openage/convert/cabextract/lzxd/lzx_compression_info`

cmake modules ([3-clause BSD license](/legal/BSD-3-clause))

 - `buildsystem/modules/FindSDL2.cmake` (taken from [openmw](https://github.com/OpenMW/openmw))
 - `buildsystem/modules/FindFTGL.cmake` (taken from [ulrichard's FTGL fork](https://github.com/ulrichard/ftgl))
 - `buildsystem/modules/FindGPerfTools.cmake` (taken from [VAST](https://github.com/mavam/vast))
 - `buildsystem/modules/FindOpusfile.cmake` (taken from [Unvanquished](https://github.com/Unvanquished/Unvanquished))

Notes about this file:

I (mic_e) am not a lawyer. This is an open-source project, we're doing this for
fun. People convinced me that this legal shit must be done, so I did it, even
though I'd rather have spent the time on useful parts of the project.
If you see any legal issues, feel free to contact me.
I, personally, despise in-sourcefile legal text blocks. They're a pest,
and unlike many others, I don't simply accept them because
"that is what everybody does". Thus, I worked out the minimal 1-line text above,
which should be free of legal caveats, and a reasonable compromise.
I'd be happy to see it used in other projects; you're free to use this file
(`copying.md`) as a template for your project's legal documentation.
