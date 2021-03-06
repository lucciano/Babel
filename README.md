![](html/sacred_scarab.jpg)

_If you are looking for an overview of Babel, see the "Documentation" 
section below._

Compiling Babel 
===============

My scripts are set up using Cygwin and I'm currently compiling for a Win32 
target using the tcc C compiler (I have the compiler in the repo because I 
eventually plan to link to it). Babel might actually compile to *nix but I 
haven't tried it yet. Eventually, Babel will be cross-compiled on both
Windows and *nix. To compile Babel, just run:

>       perl make.pl

This will build babel.exe in the bin directory. You are now ready to run 
Babel. 

>       bin/babel.exe

Installing Cygwin
-----------------

[Go here](http://www.cygwin.com/install.html)

Installing Graphviz on Windows
------------------------------

If you want to use the dump (Babel-to-Graphviz) operator to provide 
visualization of the Babel data-structure, you will need to install 
Graphviz dot (on Windows; it's installed by default in any Linux 
distro I'm aware of). In order to view the output, you will need to 
redirect it to a file:

>       perl babel eg/bbl2gv.sp > eg/bbl2gv.dot

Download the Graphviz software for your version of Windows and install it. 
Then, run the following command:

>       dot -Tsvg eg/bbl2gv.dot > eg/bbl2gv.svg

Now, open the eg/bbl2gv.svg file in any web browser (Chrome and Firefox 
both work well for me).

Note that when you run the interpreter in debug mode, it automatically 
generates a "snapshot.dot" file, which is a visual snapshot of your entire 
Babel program.

Documentation
-------------

_doc/babel\_gsg.md_  

Getting Started Guide

_doc/babel\_phil.md_

Documents the "Philosophy of Babel"

_doc/babel\_op.md_

Documents the built-in Babel operators. This file is generated when you 
build Babel using the make.pl script.

_doc/babel\_guts.md_

Documents the internals of the Babel interpreter, as it is.



Git ID: 17a2c14a367047db9a27da458859b4cc78675e56

Generated: Wed, Aug 14, 2013 12:30:39 PM


Git ID: 17a2c14a367047db9a27da458859b4cc78675e56

Generated: Wed, Aug 14, 2013 12:33:14 PM


Git ID: 17a2c14a367047db9a27da458859b4cc78675e56

Generated: Wed, Aug 14, 2013  4:19:54 PM


Git ID: 791a40ed0cbfbe1d48e69d9c216f4af843f77088

Generated: Wed, Aug 14, 2013  5:43:09 PM


Git ID: e5f7d0e78d86ce29d5e034b3442f465d5c9a9445

Generated: Fri, Aug 16, 2013  7:30:59 PM


Git ID: 70f6ef8015bec0b6c9f7069e617c7f4662284ef3

Generated: Tue, Aug 27, 2013  2:26:29 PM


Git ID: 70f6ef8015bec0b6c9f7069e617c7f4662284ef3

Generated: Tue, Aug 27, 2013  2:28:17 PM


Git ID: 70f6ef8015bec0b6c9f7069e617c7f4662284ef3

Generated: Tue, Aug 27, 2013  2:30:16 PM


Git ID: 70f6ef8015bec0b6c9f7069e617c7f4662284ef3

Generated: Tue, Aug 27, 2013  2:31:51 PM


Git ID: 70f6ef8015bec0b6c9f7069e617c7f4662284ef3

Generated: Tue, Aug 27, 2013  2:43:09 PM


Git ID: 70f6ef8015bec0b6c9f7069e617c7f4662284ef3

Generated: Tue, Aug 27, 2013  2:44:19 PM


Git ID: 70f6ef8015bec0b6c9f7069e617c7f4662284ef3

Generated: Tue, Aug 27, 2013  2:46:41 PM


Git ID: 70f6ef8015bec0b6c9f7069e617c7f4662284ef3

Generated: Tue, Aug 27, 2013  5:31:46 PM


Git ID: 70f6ef8015bec0b6c9f7069e617c7f4662284ef3

Generated: Tue, Aug 27, 2013  9:39:45 PM


Git ID: 397f9195787b95a27a8c18fe64cfe981467fc073

Generated: Sun, Sep 22, 2013  2:31:16 PM


Git ID: 397f9195787b95a27a8c18fe64cfe981467fc073

Generated: Sun, Sep 22, 2013  2:32:44 PM


Git ID: 397f9195787b95a27a8c18fe64cfe981467fc073

Generated: Sun, Sep 22, 2013  2:43:19 PM


Git ID: 397f9195787b95a27a8c18fe64cfe981467fc073

Generated: Sun, Sep 22, 2013  2:57:22 PM

